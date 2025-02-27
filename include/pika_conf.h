// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_CONF_H_
#define PIKA_CONF_H_

#include <map>
#include <set>
#include <unordered_set>

#include "slash/include/base_conf.h"
#include "slash/include/slash_mutex.h"
#include "slash/include/slash_string.h"

#include "include/pika_define.h"
#include "include/pika_meta.h"

#define kBinlogReadWinDefaultSize 9000
#define kBinlogReadWinMaxSize 90000

typedef slash::RWLock RWLock;

// global class, class members well initialized
class PikaConf : public slash::BaseConf {
 public:
  PikaConf(const std::string& path);
  ~PikaConf();

  // Getter
  const std::string& sideplugin_conf()              { return sideplugin_conf_; }
  int port()                                        { return port_; }
  std::string slaveof()                             { RWLock l(&rwlock_, false); return slaveof_;}
  int slave_priority()                              { return slave_priority_; }
  bool write_binlog()                               { return write_binlog_; }
  int thread_num()                                  { return thread_num_; }
  int thread_pool_size()                            { return thread_pool_size_; }
  int sync_thread_num()                             { return sync_thread_num_; }
  std::string log_path()                            { return log_path_; }
  std::string db_path()                             { return db_path_; }
  std::string db_sync_path()                        { return db_sync_path_; }
  int db_sync_speed()                               { return db_sync_speed_; }
  std::string compact_cron()                        { RWLock l(&rwlock_, false); return compact_cron_; }
  std::string compact_interval()                    { RWLock l(&rwlock_, false); return compact_interval_; }
  int64_t write_buffer_size()                       { return write_buffer_size_; }
  int64_t arena_block_size()                        { return arena_block_size_; }
  int64_t max_write_buffer_size()                   { return max_write_buffer_size_; }
  int max_write_buffer_number()                     { return max_write_buffer_num_; }
  int64_t max_client_response_size()                { return max_client_response_size_;}
  int timeout()                                     { return timeout_; }
  std::string server_id()                           { RWLock l(&rwlock_, false); return server_id_; }
  std::string requirepass()                         { RWLock l(&rwlock_, false); return requirepass_; }
  std::string masterauth()                          { RWLock l(&rwlock_, false); return masterauth_; }
  std::string bgsave_path()                         { RWLock l(&rwlock_, false); return bgsave_path_; }
  int expire_dump_days()                            { return expire_dump_days_; }
  std::string bgsave_prefix()                       { RWLock l(&rwlock_, false); return bgsave_prefix_; }
  std::string userpass()                            { RWLock l(&rwlock_, false); return userpass_; }
  const std::string suser_blacklist()               { RWLock l(&rwlock_, false); return slash::StringConcat(user_blacklist_, COMMA); }
  const std::vector<std::string>& vuser_blacklist() { RWLock l(&rwlock_, false); return user_blacklist_;}
//bool classic_mode()                               { return classic_mode_;}
  bool classic_mode()                               { return true; }
  int databases()                                   { return databases_;}
  int default_slot_num()                            { return default_slot_num_;}
  const std::vector<TableStruct>& table_structs()   { RWLock l(&rwlock_, false); return table_structs_; }
  std::string default_table()                       { RWLock l(&rwlock_, false); return default_table_;}
  std::string compression()                         { RWLock l(&rwlock_, false); return compression_; }
  int target_file_size_base()                       { RWLock l(&rwlock_, false); return target_file_size_base_; }
  int max_cache_statistic_keys()                    { RWLock l(&rwlock_, false); return max_cache_statistic_keys_;}
  int small_compaction_threshold()                  { RWLock l(&rwlock_, false); return small_compaction_threshold_;}
  int max_background_flushes()                      { RWLock l(&rwlock_, false); return max_background_flushes_; }
  int max_background_compactions()                  { RWLock l(&rwlock_, false); return max_background_compactions_; }
  int max_cache_files()                             { RWLock l(&rwlock_, false); return max_cache_files_; }
  int max_bytes_for_level_multiplier()              { RWLock l(&rwlock_, false); return max_bytes_for_level_multiplier_; }
  int64_t block_size()                              { RWLock l(&rwlock_, false); return block_size_; }
  int64_t block_cache()                             { RWLock l(&rwlock_, false); return block_cache_; }
  bool share_block_cache()                          { RWLock l(&rwlock_, false); return share_block_cache_; }
  bool cache_index_and_filter_blocks()              { RWLock l(&rwlock_, false); return cache_index_and_filter_blocks_; }
  bool optimize_filters_for_hits()                  { RWLock l(&rwlock_, false); return optimize_filters_for_hits_; }
  bool level_compaction_dynamic_level_bytes()       { RWLock l(&rwlock_, false); return level_compaction_dynamic_level_bytes_; }
  int expire_logs_nums()                            { return expire_logs_nums_; }
  int expire_logs_days()                            { return expire_logs_days_; }
  std::string conf_path()                           { return conf_path_; }
  bool slave_read_only()                            { return slave_read_only_; }
  int maxclients()                                  { return maxclients_; }
  int root_connection_num()                         { return root_connection_num_; }
  bool slowlog_write_errorlog()                     { return slowlog_write_errorlog_; }
  int slowlog_slower_than()                         { return slowlog_log_slower_than_; }
  int slowlog_max_len()                             { return slowlog_max_len_; }
  std::string network_interface()                   { return network_interface_; }
  int sync_window_size()                            { return sync_window_size_; }
  int max_conn_rbuf_size()                          { return max_conn_rbuf_size_; }
//int consensus_level()                             { return consensus_level_; }
  int consensus_level()                             { return 0; }
  int replication_num()                             { return replication_num_; }

  // Immutable config items, we don't use lock.
  bool daemonize()                                  { return daemonize_; }
  std::string pidfile()                             { return pidfile_; }
  int binlog_file_size()                            { return binlog_file_size_; }
  PikaMeta* local_meta()                            { return local_meta_; }

  // Setter
  void SetJsonFile(const std::string& fname) {
    sideplugin_conf_ = fname;
  }
  void SetPort(const int value) {
    port_ = value;
  }
  void SetThreadNum(const int value) {
    thread_num_ = value;
  }
  void SetTimeout(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("timeout", std::to_string(value));
    timeout_ = value;
  }
  void SetThreadPoolSize(const int value) {
    thread_pool_size_ = value;
  }
  void SetSlaveof(const std::string& value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("slaveof", value);
    slaveof_ = value;
  }
  void SetSlavePriority(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("slave-priority", std::to_string(value));
    slave_priority_ = value;
  }
  void SetWriteBinlog(const std::string& value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("write-binlog", value);
    write_binlog_ = (value == "yes") ? true : false;
  }
  void SetMaxCacheStatisticKeys(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("max-cache-statistic-keys", std::to_string(value));
    max_cache_statistic_keys_ = value;
  }
  void SetSmallCompactionThreshold(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("small-compaction-threshold", std::to_string(value));
    small_compaction_threshold_ = value;
  }
  void SetMaxClientResponseSize(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("max-client-response-size", std::to_string(value));
    max_client_response_size_ = value;
  }
  void SetBgsavePath(const std::string &value) {
    RWLock l(&rwlock_, true);
    bgsave_path_ = value;
    if (value[value.length() - 1] != '/') {
      bgsave_path_ += "/";
    }
  }
  void SetExpireDumpDays(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("dump-expire", std::to_string(value));
    expire_dump_days_ = value;
  }
  void SetBgsavePrefix(const std::string &value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("dump-prefix", value);
    bgsave_prefix_ = value;
  }
  void SetRequirePass(const std::string &value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("requirepass", value);
    requirepass_ = value;
  }
  void SetMasterAuth(const std::string &value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("masterauth", value);
    masterauth_ = value;
  }
  void SetUserPass(const std::string &value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("userpass", value);
    userpass_ = value;
  }
  void SetUserBlackList(const std::string &value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("userblacklist", value);
    slash::StringSplit(value, COMMA, user_blacklist_);
    for (auto& item : user_blacklist_) {
      slash::StringToLower(item);
    }
  }
  void SetExpireLogsNums(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("expire-logs-nums", std::to_string(value));
    expire_logs_nums_ = value;
  }
  void SetExpireLogsDays(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("expire-logs-days", std::to_string(value));
    expire_logs_days_ = value;
  }
  void SetMaxConnection(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("maxclients", std::to_string(value));
    maxclients_ = value;
  }
  void SetRootConnectionNum(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("root-connection-num", std::to_string(value));
    root_connection_num_ = value;
  }
  void SetSlowlogWriteErrorlog(const bool value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("slowlog-write-errorlog", value == true ? "yes" : "no");
    slowlog_write_errorlog_ = value;
  }
  void SetSlowlogSlowerThan(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("slowlog-log-slower-than", std::to_string(value));
    slowlog_log_slower_than_ = value;
  }
  void SetSlowlogMaxLen(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("slowlog-max-len", std::to_string(value));
    slowlog_max_len_ = value;
  }
  void SetDbSyncSpeed(const int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("db-sync-speed", std::to_string(value));
    db_sync_speed_ = value;
  }
  void SetCompactCron(const std::string &value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("compact-cron", value);
    compact_cron_ = value;
  }
  void SetCompactInterval(const std::string &value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("compact-interval", value);
    compact_interval_ = value;
  }
  void SetSyncWindowSize(int value) {
    TryPushDiffCommands("sync-window-size", std::to_string(value));
    sync_window_size_ = value;
  }
  void SetMaxConnRbufSize(int value) {
    TryPushDiffCommands("max-conn-rbuf-size", std::to_string(value));
    max_conn_rbuf_size_ = value;
  }
  void SetMaxCacheFiles(int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("max-cache-files", std::to_string(value));
    max_cache_files_ = value;
  }
  void SetMaxBackgroudCompactions(int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("max-background-compactions", std::to_string(value));
    max_background_compactions_ = value;
  }
  void SetWriteBufferSize(int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("write-buffer-size", std::to_string(value));
    write_buffer_size_ = value;
  }
  void SetMaxWriteBufferNumber(int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("max-write-buffer-number", std::to_string(value));
    max_write_buffer_num_ = value;
  }
  void SetArenaBlockSize(int value) {
    RWLock l(&rwlock_, true);
    TryPushDiffCommands("arena-block-size", std::to_string(value));
    arena_block_size_ = value;
  }

  Status TablePartitionsSanityCheck(const std::string& table_name,
                                    const std::set<uint32_t>& partition_ids,
                                    bool is_add);
  Status AddTablePartitions(const std::string& table_name,
                            const std::set<uint32_t>& partition_ids);
  Status RemoveTablePartitions(const std::string& table_name,
                               const std::set<uint32_t>& partition_ids);
  Status AddTable(const std::string &table_name, uint32_t slot_num);
  Status AddTableSanityCheck(const std::string &table_name);
  Status DelTable(const std::string &table_name);
  Status DelTableSanityCheck(const std::string &table_name);

  int Load();
  int ConfigRewrite();

 private:
  Status InternalGetTargetTable(const std::string& table_name,
                                uint32_t* const target);

  std::string sideplugin_conf_;
  int port_;
  std::string slaveof_;
  int slave_priority_;
  int thread_num_;
  int thread_pool_size_;
  int sync_thread_num_;
  std::string log_path_;
  std::string db_path_;
  std::string db_sync_path_;
  int expire_dump_days_;
  int db_sync_speed_;
  std::string compact_cron_;
  std::string compact_interval_;
  int64_t write_buffer_size_;
  int64_t arena_block_size_;
  int64_t max_write_buffer_size_;
  int max_write_buffer_num_;
  int64_t max_client_response_size_;
  bool daemonize_;
  int timeout_;
  std::string server_id_;
  std::string requirepass_;
  std::string masterauth_;
  std::string userpass_;
  std::vector<std::string> user_blacklist_;
  bool classic_mode_;
  int databases_;
  int default_slot_num_;
  std::vector<TableStruct> table_structs_;
  std::string default_table_;
  std::string bgsave_path_;
  std::string bgsave_prefix_;
  std::string pidfile_;

  std::string compression_;
  int maxclients_;
  int root_connection_num_;
  bool slowlog_write_errorlog_;
  int slowlog_log_slower_than_;
  int slowlog_max_len_;
  int expire_logs_days_;
  int expire_logs_nums_;
  bool slave_read_only_;
  std::string conf_path_;
  int max_cache_statistic_keys_;
  int small_compaction_threshold_;
  int max_background_flushes_;
  int max_background_compactions_;
  int max_cache_files_;
  int max_bytes_for_level_multiplier_;
  int64_t block_size_;
  int64_t block_cache_;
  bool share_block_cache_;
  bool cache_index_and_filter_blocks_;
  bool optimize_filters_for_hits_;
  bool level_compaction_dynamic_level_bytes_;
  int sync_window_size_;
  int max_conn_rbuf_size_;
  int consensus_level_;
  int replication_num_;

  std::string network_interface_;

  // diff commands between cached commands and config file commands
  std::map<std::string, std::string> diff_commands_;
  void TryPushDiffCommands(const std::string& command, const std::string& value);

  //
  // Critical configure items
  //
  bool write_binlog_;
  int target_file_size_base_;
  int binlog_file_size_;

  PikaMeta* local_meta_;

  pthread_rwlock_t rwlock_;
};

#endif
