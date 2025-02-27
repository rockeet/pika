// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#include "include/pika_client_conn.h"

#include <vector>
#include <algorithm>

#include <glog/logging.h>

#include "include/pika_conf.h"
#include "include/pika_server.h"
#include "include/pika_cmd_table_manager.h"
#include "include/pika_admin.h"
#include "include/pika_rm.h"
#include "include/pika_proxy.h"
#include "pink/include/pika_cmd_time_histogram.h"
#include "terark/util/profiling.hpp"
#include "terark/util/function.hpp"

extern PikaConf* g_pika_conf;
extern PikaServer* g_pika_server;
extern PikaReplicaManager* g_pika_rm;
extern PikaCmdTableManager* g_pika_cmd_table_manager;
extern time_histogram::PikaCmdRunTimeHistogram* g_pika_cmd_run_time_histogram;
extern PikaProxy* g_pika_proxy;

PikaClientConn::PikaClientConn(int fd, const std::string& ip_port,
                               pink::Thread* thread,
                               pink::PinkEpoll* pink_epoll,
                               const pink::HandleType& handle_type,
                               int max_conn_rbuf_size)
      : RedisConn(fd, ip_port, thread, pink_epoll, handle_type, max_conn_rbuf_size),
        server_thread_(reinterpret_cast<pink::ServerThread*>(thread)),
        current_table_(g_pika_conf->default_table()),
        is_pubsub_(false) {
  auth_stat_.Init();
}

PikaClientConn::~PikaClientConn() = default;

std::shared_ptr<Cmd> PikaClientConn::DoCmd(
    const PikaCmdArgsType& argv,
    const fstring opt,
    const std::shared_ptr<std::string>& resp_ptr) {
  // Get command info
  std::shared_ptr<Cmd> c_ptr = g_pika_cmd_table_manager->GetCmd(opt);
  if (!c_ptr) {
    std::shared_ptr<Cmd> tmp_ptr = std::make_shared<DummyCmd>(DummyCmd());
    tmp_ptr->res().SetRes(CmdRes::kErrOther,
        "unknown or unsupported command \"" + opt + "\"");
    return tmp_ptr;
  }
  argv.cmd_idx = c_ptr->cmd_idx();
  c_ptr->SetConn(std::dynamic_pointer_cast<PikaClientConn>(shared_from_this()));
  c_ptr->SetResp(resp_ptr);

  // Check authed
  // AuthCmd will set stat_
  if (!auth_stat_.IsAuthed(c_ptr)) {
    if (c_ptr->is_admin_require()) {
      c_ptr->res().SetRes(CmdRes::kErrOther, "NOAUTH Authentication required.");
    }
    else {
      c_ptr->res().SetRes(CmdRes::kErrOther, "Command is in blacklist.");
    }
    return c_ptr;
  }

  uint64_t start_us = 0;
  if (g_pika_conf->slowlog_slower_than() >= 0) {
    start_us = slash::NowMicros();
  }

  bool is_monitoring = g_pika_server->HasMonitorClients();
  if (is_monitoring) {
    ProcessMonitor(argv);
  }

  // Initial
  c_ptr->Initial(argv, current_table_);
  if (!c_ptr->res().ok()) {
    return c_ptr;
  }

  g_pika_server->UpdateQueryNumAndExecCountTable(current_table_, *c_ptr);

  // PubSub connection
  // (P)SubscribeCmd will set is_pubsub_
  if (this->IsPubSub()) {
    if (opt != kCmdNameSubscribe &&
        opt != kCmdNameUnSubscribe &&
        opt != kCmdNamePing &&
        opt != kCmdNamePSubscribe &&
        opt != kCmdNamePUnSubscribe) {
      c_ptr->res().SetRes(CmdRes::kErrOther,
          "only (P)SUBSCRIBE / (P)UNSUBSCRIBE / PING / QUIT allowed in this context");
      return c_ptr;
    }
  }

/* // for topling, skip these useless check
  if (g_pika_conf->consensus_level() != 0 && c_ptr->is_write()) {
    c_ptr->SetStage(Cmd::kBinlogStage);
  }
  if (!g_pika_server->IsCommandSupport(opt)) {
    c_ptr->res().SetRes(CmdRes::kErrOther,
        "This command is not supported in current configuration");
    return c_ptr;
  }

  if (!g_pika_server->IsTableExist(current_table_)) {
    c_ptr->res().SetRes(CmdRes::kErrOther, "Table not found");
    return c_ptr;
  }
*/

  // TODO: Consider special commands, like flushall, flushdb?
  if (c_ptr->is_write()) {
    if (g_pika_server->IsTableBinlogIoError(current_table_)) {
      c_ptr->res().SetRes(CmdRes::kErrOther, "Writing binlog failed, maybe no space left on device");
      return c_ptr;
    }
    std::vector<std::string> cur_key = c_ptr->current_key();
    if (cur_key.empty()) {
      c_ptr->res().SetRes(CmdRes::kErrOther, "Internal ERROR");
      return c_ptr;
    }
    if (g_pika_server->readonly(current_table_, cur_key.front())) {
      c_ptr->res().SetRes(CmdRes::kErrOther, "Server in read-only");
      return c_ptr;
    }
    /* // for topling, skip these useless check
    if (!g_pika_server->ConsensusCheck(current_table_, cur_key.front())) {
      c_ptr->res().SetRes(CmdRes::kErrOther, "Consensus level not match");
    }
    */
  }

  // Process Command
  c_ptr->Execute();

  if (g_pika_conf->slowlog_slower_than() >= 0) {
    ProcessSlowlog(argv, start_us, c_ptr->GetDoDuration());
  }
/* // for topling, skip these useless check
  if (g_pika_conf->consensus_level() != 0 && c_ptr->is_write()) {
    c_ptr->SetStage(Cmd::kExecuteStage);
  }
*/
  return c_ptr;
}

void PikaClientConn::ProcessSlowlog(const PikaCmdArgsType& argv, uint64_t start_us, uint64_t do_duration) {
  int32_t start_time = start_us / 1000000;
  int64_t duration = slash::NowMicros() - start_us;
  if (duration > g_pika_conf->slowlog_slower_than()) {
    g_pika_server->SlowlogPushEntry(argv, start_time, duration);
    if (g_pika_conf->slowlog_write_errorlog()) {
      bool trim = false;
      std::string slow_log;
      uint32_t cmd_size = 0;
      for (unsigned int i = 0; i < argv.size(); i++) {
        cmd_size += 1 + argv[i].size(); // blank space and argument length
        if (!trim) {
          slow_log.append(" ");
          slow_log.append(slash::ToRead(argv[i]));
          if (slow_log.size() >= 1000) {
            trim = true;
            slow_log.resize(1000);
            slow_log.append("...\"");
          }
        }
      }
      LOG(ERROR) << "ip_port: " << ip_port() << ", table: " << current_table_
        << ", command:" << slow_log << ", command_size: " << cmd_size - 1
        << ", arguments: " << argv.size() << ", start_time(s): " << start_time
        << ", duration(us): " << duration << ", do_duration_(us): " << do_duration;
    }
  }
}

void PikaClientConn::ProcessMonitor(const PikaCmdArgsType& argv) {
  std::string monitor_message;
  std::string table_name = g_pika_conf->classic_mode()
    ? current_table_.substr(2) : current_table_;
  monitor_message = std::to_string(1.0*slash::NowMicros()/1000000) +
    " [" + table_name + " " + this->ip_port() + "]";
  for (PikaCmdArgsType::const_iterator iter = argv.begin(); iter != argv.end(); iter++) {
    monitor_message += " " + slash::ToRead(*iter);
  }
  g_pika_server->AddMonitorMessage(monitor_message);
}

static terark::profiling pf;
void PikaClientConn::ProcessRedisCmds(std::vector<pink::RedisCmdArgsType>&& argvs, bool async) {
  if (async) {
    BgTaskArg* arg = new BgTaskArg();
    arg->redis_cmds = std::move(argvs);
    arg->conn_ptr = std::dynamic_pointer_cast<PikaClientConn>(shared_from_this());
    metric_info.parse_end_time = pf.now();
    g_pika_server->ScheduleClientPool(&DoBackgroundTask, arg);
  }
  else {
    BatchExecRedisCmd(argvs);
  }
}

void PikaClientConn::DoBackgroundTask(void* arg) {
  BgTaskArg* bg_arg = reinterpret_cast<BgTaskArg*>(arg);
  std::shared_ptr<PikaClientConn> conn_ptr = bg_arg->conn_ptr;
  conn_ptr->metric_info.schedule_end_time = pf.now();
  if (bg_arg->redis_cmds.size() == 0) {
    delete bg_arg;
    conn_ptr->NotifyEpoll(false);
    return;
  }

  int num = 1;
  for (const auto& argv : bg_arg->redis_cmds) {
    if (argv.size() == 0) {
      delete bg_arg;
      conn_ptr->NotifyEpoll(false);
      return;
    }

    if (argv[0] == "quit") {
      bg_arg->redis_cmds.resize(num);
      break;
    }
    num++;
  }
  std::vector<Node> dst;
  bool all_local = true;
  Status s = g_pika_server->GetCmdRouting(
      bg_arg->redis_cmds, &dst, &all_local);
  if (!s.ok()) {
    delete bg_arg;
    conn_ptr->NotifyEpoll(false);
    return;
  }
  if (!all_local) {
    g_pika_proxy->ScheduleForwardToBackend(conn_ptr, bg_arg->redis_cmds, dst);
  } else {
    conn_ptr->BatchExecRedisCmd(bg_arg->redis_cmds);
  }
  delete bg_arg;
}

void PikaClientConn::DoExecTask(void* arg) {
  BgTaskArg* bg_arg = reinterpret_cast<BgTaskArg*>(arg);
  std::shared_ptr<Cmd> cmd_ptr = bg_arg->cmd_ptr;
  std::shared_ptr<PikaClientConn> conn_ptr = bg_arg->conn_ptr;
  std::shared_ptr<std::string> resp_ptr = bg_arg->resp_ptr;
  LogOffset offset = bg_arg->offset;
  std::string table_name = bg_arg->table_name;
  uint32_t partition_id = bg_arg->partition_id;
  delete bg_arg;

  uint64_t start_us = 0;
  if (g_pika_conf->slowlog_slower_than() >= 0) {
    start_us = slash::NowMicros();
  }
  cmd_ptr->SetStage(Cmd::kExecuteStage);
  cmd_ptr->Execute();
  if (g_pika_conf->slowlog_slower_than() >= 0) {
     conn_ptr->ProcessSlowlog(cmd_ptr->argv(), start_us, cmd_ptr->GetDoDuration());
  }

  std::shared_ptr<SyncMasterPartition> partition =
    g_pika_rm->GetSyncMasterPartitionByName(PartitionInfo(table_name, partition_id));
  if (partition == nullptr) {
    LOG(WARNING) << "Sync Master Partition not exist " << table_name << partition_id;
    return;
  }
  partition->ConsensusUpdateAppliedIndex(offset);

  if (conn_ptr == nullptr || resp_ptr == nullptr) {
    return;
  }

  *resp_ptr = cmd_ptr->res().message();
  // last step to update resp_num, early update may casue another therad may
  // TryWriteResp success with resp_ptr not updated
  conn_ptr->resp_num--;
  conn_ptr->TryWriteResp();
}

void PikaClientConn::BatchExecRedisCmd(const std::vector<pink::RedisCmdArgsType>& argvs) {
  resp_num.store(argvs.size());
  metric_info.cmd_process_times.reserve(argvs.size());
  auto cmdtab = g_pika_cmd_table_manager->cmds();
  for (size_t i = 0; i < argvs.size(); ++i) {
    resp_array.push_back(std::make_shared<std::string>());
    uint64_t start = pf.now();
    ExecRedisCmd(argvs[i], resp_array.back());
    uint64_t end = pf.now();
    fstring cmdname = argvs[i][0];
    //size_t cmd_idx = g_pika_cmd_run_time_histogram->m_get_idx(argvs[i][0]);
    //size_t cmd_idx = cmdtab->find_i(cmdname);
    size_t cmd_idx = argvs[i].cmd_idx;
    //TERARK_VERIFY_LT(cmd_idx, cmdtab->end_i());
    if (cmd_idx < cmdtab->end_i()) {
      metric_info.cmd_process_times.push_back({cmd_idx, start, end});
    }
    else {
      fprintf(stderr, "command '%s' is not supported\n", cmdname.c_str());
    }
  }
  TryWriteResp();
}

void PikaClientConn::TryWriteResp() {
  int expected = 0;
  if (resp_num.compare_exchange_strong(expected, -1)) {
    for (auto& resp : resp_array) {
      WriteResp(std::move(*resp));
    }
    if (write_completed_cb_ != nullptr) {
      write_completed_cb_();
      write_completed_cb_ = nullptr;
    }
    resp_array.clear();
    NotifyEpoll(true);
  }
}


void PikaClientConn::ExecRedisCmd(const PikaCmdArgsType& argv, const std::shared_ptr<std::string>& resp_ptr) {
  // get opt
#if 1
  const std::string& opt = argv[0];
  TERARK_VERIFY_S_NE(opt, kClusterPrefix);
#else
  std::string opt = argv[0];
  slash::StringToLower(opt);
  if (opt == kClusterPrefix) {
    if (argv.size() >= 2 ) {
      opt += argv[1];
      slash::StringToLower(opt);
    }
  }
#endif
  std::shared_ptr<Cmd> cmd_ptr = DoCmd(argv, opt, resp_ptr);
  // level == 0 or (cmd error) or (is_read)
  if (g_pika_conf->consensus_level() == 0 || !cmd_ptr->res().ok() || !cmd_ptr->is_write()) {
    *resp_ptr = cmd_ptr->res().message();
    resp_num--;
  }
}

// Initial permission status
void PikaClientConn::AuthStat::Init() {
  // Check auth required
  stat_ = g_pika_conf->userpass() == "" ?
    kLimitAuthed : kNoAuthed;
  if (stat_ == kLimitAuthed
      && g_pika_conf->requirepass() == "") {
    stat_ = kAdminAuthed;
  }
}

// Check permission for current command
bool PikaClientConn::AuthStat::IsAuthed(const std::shared_ptr<Cmd>& cmd_ptr) {
  fstring opt = cmd_ptr->name();
  if (opt == kCmdNameAuth) {
    return true;
  }
  const std::vector<std::string>& blacklist = g_pika_conf->vuser_blacklist();
  switch (stat_) {
    case kNoAuthed:
      return false;
    case kAdminAuthed:
      break;
    case kLimitAuthed:
      if (cmd_ptr->is_admin_require()
        || find(blacklist.begin(), blacklist.end(), opt) != blacklist.end()) {
      return false;
      }
      break;
    default:
      LOG(WARNING) << "Invalid auth stat : " << static_cast<unsigned>(stat_);
      return false;
  }
  return true;
}

// Update permission status
bool PikaClientConn::AuthStat::ChecknUpdate(const std::string& message) {
  // Situations to change auth status
  if (message == "USER") {
    stat_ = kLimitAuthed;
  } else if (message == "ROOT") {
    stat_ = kAdminAuthed;
  } else {
    return false;
  }
  return true;
}

// compare addr in ClientInfo
bool AddrCompare(const ClientInfo& lhs, const ClientInfo& rhs) {
  return rhs.ip_port < lhs.ip_port;
}

bool IdleCompare(const ClientInfo& lhs, const ClientInfo& rhs) {
  return lhs.last_interaction < rhs.last_interaction;
}

