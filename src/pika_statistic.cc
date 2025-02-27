// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#include "include/pika_statistic.h"
#include "slash/include/env.h"
#include "include/pika_command.h"
#include "include/pika_cmd_table_manager.h"

extern PikaCmdTableManager* g_pika_cmd_table_manager;

/* QpsStatistic */

QpsStatistic::QpsStatistic()
    : querynum(0),
      write_querynum(0),
      last_querynum(0),
      last_write_querynum(0),
      last_sec_querynum(0),
      last_sec_write_querynum(0),
      last_time_us(0) {
}

QpsStatistic::QpsStatistic(const QpsStatistic& other) {
  querynum = other.querynum.load();
  write_querynum = other.write_querynum.load();
  last_querynum = other.last_querynum.load();
  last_write_querynum = other.last_write_querynum.load();
  last_sec_querynum = other.last_sec_querynum.load();
  last_sec_write_querynum = other.last_sec_write_querynum.load();
  last_time_us = other.last_time_us.load();
}

QpsStatistic::~QpsStatistic() {
}

void QpsStatistic::IncreaseQueryNum(bool is_write) {
  querynum++;
  if (is_write) {
    write_querynum++;
  }
}

void QpsStatistic::ResetLastSecQuerynum() {
  uint64_t last_query = last_querynum.load();
  uint64_t last_write_query = last_write_querynum.load();
  uint64_t cur_query = querynum.load();
  uint64_t cur_write_query = write_querynum.load();
  uint64_t last_time = last_time_us.load();
  if (cur_write_query < last_write_query) {
    cur_write_query = last_write_query;
  }
  if (cur_query < last_query) {
    cur_query = last_query;
  }
  uint64_t delta_query = cur_query - last_query;
  uint64_t delta_write_query = cur_write_query - last_write_query;
  uint64_t cur_time_us = slash::NowMicros();
  if (cur_time_us <= last_time) {
    cur_time_us = last_time + 1;
  }
  uint64_t delta_time_us = cur_time_us - last_time;
  last_sec_querynum.store(delta_query
       * 1000000 / (delta_time_us));
  last_sec_write_querynum.store(delta_write_query
       * 1000000 / (delta_time_us));
  last_querynum.store(cur_query);
  last_write_querynum.store(cur_write_query);

  last_time_us.store(cur_time_us);
}

/* ServerStatistic */

ServerStatistic::ServerStatistic()
    : accumulative_connections(0) {
//  CmdTable* cmds = new CmdTable();
  const CmdTable* cmds = g_pika_cmd_table_manager->cmds();
  TERARK_VERIFY_EZ(cmds->delcnt());
  exec_count_table.resize(cmds->end_i(), 0);
/*
  for (size_t i = 0, n = cmds->end_i(); i < n; ++i) {
    terark::fstring name = cmds->key(i);
    exec_count_table[i].store(0);
  }
  DestoryCmdTable(cmds);
  delete cmds;
*/
}

ServerStatistic::~ServerStatistic() {
}

/* Statistic */

Statistic::Statistic() {
  pthread_rwlockattr_t table_stat_rw_attr;
  pthread_rwlockattr_init(&table_stat_rw_attr);
  pthread_rwlockattr_setkind_np(&table_stat_rw_attr,
      PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
  pthread_rwlock_init(&table_stat_rw, &table_stat_rw_attr);
  table_stat.resize(kMaxDbNum);
  for (int i = 0; i < kMaxDbNum; ++i) {
    table_stat[i].first = "db" + std::to_string(i);
  }
}

QpsStatistic Statistic::TableStat(const std::string& table_name) {
//  slash::RWLock l(&table_stat_rw, false);
  auto idx = DbIdxFromStr(table_name);
  ROCKSDB_VERIFY_LT(size_t(idx), table_stat.size());
  return table_stat[idx].second;
}

std::unordered_map<std::string, QpsStatistic> Statistic::AllTableStat() {
  //slash::RWLock l(&table_stat_rw, false);
  std::unordered_map<std::string, QpsStatistic> res;
  res.emplace(table_stat[0]);
  return res;
}

void Statistic::UpdateTableQps(const std::string& table_name, bool is_write) {
  auto idx = DbIdxFromStr(table_name);
  ROCKSDB_VERIFY_LT(size_t(idx), table_stat.size());
  table_stat[idx].second.IncreaseQueryNum(is_write);
}

void Statistic::ResetTableLastSecQuerynum() {
  slash::RWLock l(&table_stat_rw, false);
  for (auto& stat : table_stat) {
    stat.second.ResetLastSecQuerynum();
  }
}






