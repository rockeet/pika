// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#include "include/pika_command.h"

#include "include/pika_kv.h"
#include "include/pika_bit.h"
#include "include/pika_set.h"
#include "include/pika_geo.h"
#include "include/pika_list.h"
#include "include/pika_zset.h"
#include "include/pika_hash.h"
#include "include/pika_admin.h"
#include "include/pika_pubsub.h"
#include "include/pika_hyperloglog.h"
#include "include/pika_slot.h"
#include "include/pika_cluster.h"
#include "include/pika_server.h"
#include "include/pika_rm.h"
#include "include/pika_cmd_table_manager.h"
#include "third/blackwidow/src/scope_record_lock.h"
#include <terark/util/autofree.hpp>
#include <terark/util/function.hpp>
#include <terark/util/profiling.hpp>
#include "pink/include/pika_cmd_time_histogram.h"

using time_histogram::PikaCmdRunTimeHistogram;

extern PikaServer* g_pika_server;
extern PikaReplicaManager* g_pika_rm;
extern PikaCmdTableManager* g_pika_cmd_table_manager;
extern PikaCmdRunTimeHistogram* g_pika_cmd_run_time_histogram;

static size_t CmdTable_get_idx(fstring name) {
  const CmdTable* cmdtab = g_pika_cmd_table_manager->cmds();
  size_t idx = cmdtab->find_i(name);
  return idx;
}

static terark::fstring CmdTable_get_name(size_t idx) {
  const CmdTable* cmdtab = g_pika_cmd_table_manager->cmds();
  TERARK_VERIFY_LT(idx, cmdtab->end_i());
  return cmdtab->key(idx);
}

void InitCmdTable(CmdTable* cmd_table) {
  auto add = [cmd_table](Cmd* cmd) {
    auto ib = cmd_table->insert_i(cmd->name(), cmd);
    TERARK_VERIFY_S(ib.second, "cmdname: %s", cmd->name());
    cmd->set_cmd_idx(ib.first);
  };

  //Admin
  add(new SlaveofCmd(kCmdNameSlaveof, -3, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new DbSlaveofCmd(kCmdNameDbSlaveof, -2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new AuthCmd(kCmdNameAuth, 2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new BgsaveCmd(kCmdNameBgsave, -1, kCmdFlagsRead | kCmdFlagsAdmin | kCmdFlagsSuspend));
  add(new CompactCmd(kCmdNameCompact, -1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PurgelogstoCmd(kCmdNamePurgelogsto, -2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PingCmd(kCmdNamePing, 1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SelectCmd(kCmdNameSelect, 2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new FlushallCmd(kCmdNameFlushall, 1, kCmdFlagsWrite | kCmdFlagsSuspend | kCmdFlagsAdmin));
  add(new FlushdbCmd(kCmdNameFlushdb, -1, kCmdFlagsWrite | kCmdFlagsSuspend | kCmdFlagsAdmin));
  add(new ClientCmd(kCmdNameClient, -2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new ShutdownCmd(kCmdNameShutdown, 1, kCmdFlagsRead | kCmdFlagsLocal | kCmdFlagsAdmin));
  add(new InfoCmd(kCmdNameInfo, -1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new ConfigCmd(kCmdNameConfig, -2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new MonitorCmd(kCmdNameMonitor, -1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new DbsizeCmd(kCmdNameDbsize, 1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new TimeCmd(kCmdNameTime, 1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new DelbackupCmd(kCmdNameDelbackup, 1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new EchoCmd(kCmdNameEcho, 2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new ScandbCmd(kCmdNameScandb, -1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlowlogCmd(kCmdNameSlowlog, -2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PaddingCmd(kCmdNamePadding, 2, kCmdFlagsWrite | kCmdFlagsAdmin));
  add(new PKPatternMatchDelCmd(kCmdNamePKPatternMatchDel, 3, kCmdFlagsWrite | kCmdFlagsAdmin));
  add(new DummyCmd(kCmdDummy, 0, kCmdFlagsWrite | kCmdFlagsSinglePartition));

#if 0
  // Slots related
  add(new SlotsInfoCmd(kCmdNameSlotsInfo, -1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsHashKeyCmd(kCmdNameSlotsHashKey, -2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtSlotAsyncCmd(kCmdNameSlotsMgrtSlotAsync, 8, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtTagSlotAsyncCmd(kCmdNameSlotsMgrtTagSlotAsync, 8, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsDelCmd(kCmdNameSlotsDel, -2, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsScanCmd(kCmdNameSlotsScan, -3, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtExecWrapperCmd(kCmdNameSlotsMgrtExecWrapper, -3, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtAsyncStatusCmd(kCmdNameSlotsMgrtAsyncStatus, 1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtAsyncCancelCmd(kCmdNameSlotsMgrtAsyncCancel, 1, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtSlotCmd(kCmdNameSlotsMgrtSlot, 5, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtTagSlotCmd(kCmdNameSlotsMgrtTagSlot, 5, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtOneCmd(kCmdNameSlotsMgrtOne, 5, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new SlotsMgrtTagOneCmd(kCmdNameSlotsMgrtTagOne, 5, kCmdFlagsRead | kCmdFlagsAdmin));

  // Cluster related
  add(new PkClusterInfoCmd(kCmdNamePkClusterInfo, -3, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PkClusterAddSlotsCmd(kCmdNamePkClusterAddSlots, -3, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PkClusterDelSlotsCmd(kCmdNamePkClusterDelSlots, -3, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PkClusterSlotsSlaveofCmd(kCmdNamePkClusterSlotsSlaveof, -5, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PkClusterAddTableCmd(kCmdNamePkClusterAddTable, 4, kCmdFlagsRead | kCmdFlagsAdmin));
  add(new PkClusterDelTableCmd(kCmdNamePkClusterDelTable, 3, kCmdFlagsRead | kCmdFlagsAdmin));
#ifdef TCMALLOC_EXTENSION
  add(new TcmallocCmd(kCmdNameTcmalloc, -2, kCmdFlagsRead | kCmdFlagsAdmin));
#endif
#endif

  //Kv
  add(new SetCmd(kCmdNameSet, -3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new GetCmd(kCmdNameGet, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new DelCmd(kCmdNameDel, -2, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new IncrCmd(kCmdNameIncr, 2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new IncrbyCmd(kCmdNameIncrby, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new IncrbyfloatCmd(kCmdNameIncrbyfloat, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new DecrCmd(kCmdNameDecr, 2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new DecrbyCmd(kCmdNameDecrby, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new GetsetCmd(kCmdNameGetset, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new AppendCmd(kCmdNameAppend, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new MgetCmd(kCmdNameMget, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new KeysCmd(kCmdNameKeys, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new SetnxCmd(kCmdNameSetnx, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new SetexCmd(kCmdNameSetex, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new PsetexCmd(kCmdNamePsetex, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new DelvxCmd(kCmdNameDelvx, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new MsetCmd(kCmdNameMset, -3, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new MsetnxCmd(kCmdNameMsetnx, -3, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new GetrangeCmd(kCmdNameGetrange, 4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new SetrangeCmd(kCmdNameSetrange, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new StrlenCmd(kCmdNameStrlen, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new ExistsCmd(kCmdNameExists, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new ExpireCmd(kCmdNameExpire, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new PexpireCmd(kCmdNamePexpire, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new ExpireatCmd(kCmdNameExpireat, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new PexpireatCmd(kCmdNamePexpireat, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new TtlCmd(kCmdNameTtl, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new PttlCmd(kCmdNamePttl, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new PersistCmd(kCmdNamePersist, 2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new TypeCmd(kCmdNameType, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new ScanCmd(kCmdNameScan, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new ScanxCmd(kCmdNameScanx, -3, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsKv));
  add(new PKSetexAtCmd(kCmdNamePKSetexAt, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new PKScanRangeCmd(kCmdNamePKScanRange, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));
  add(new PKRScanRangeCmd(kCmdNamePKRScanRange, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsKv));

  //Hash
  add(new HDelCmd(kCmdNameHDel, -3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HSetCmd(kCmdNameHSet, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HGetCmd(kCmdNameHGet, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HGetallCmd(kCmdNameHGetall, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HExistsCmd(kCmdNameHExists, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HIncrbyCmd(kCmdNameHIncrby, 4, kCmdFlagsWrite |kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HIncrbyfloatCmd(kCmdNameHIncrbyfloat, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HKeysCmd(kCmdNameHKeys, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HLenCmd(kCmdNameHLen, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HMgetCmd(kCmdNameHMget, -3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HMsetCmd(kCmdNameHMset, -4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HSetnxCmd(kCmdNameHSetnx, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HStrlenCmd(kCmdNameHStrlen, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HValsCmd(kCmdNameHVals, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HScanCmd(kCmdNameHScan, -3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new HScanxCmd(kCmdNameHScanx, -3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new PKHScanRangeCmd(kCmdNamePKHScanRange, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));
  add(new PKHRScanRangeCmd(kCmdNamePKHRScanRange, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsHash));

  //List
  add(new LIndexCmd(kCmdNameLIndex, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LInsertCmd(kCmdNameLInsert, 5, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LLenCmd(kCmdNameLLen, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LPopCmd(kCmdNameLPop, 2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LPushCmd(kCmdNameLPush, -3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LPushxCmd(kCmdNameLPushx, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LRangeCmd(kCmdNameLRange, 4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LRemCmd(kCmdNameLRem, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LSetCmd(kCmdNameLSet, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new LTrimCmd(kCmdNameLTrim, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new RPopCmd(kCmdNameRPop, 2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new RPopLPushCmd(kCmdNameRPopLPush, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new RPushCmd(kCmdNameRPush, -3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));
  add(new RPushxCmd(kCmdNameRPushx, 3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsList));

  //Zset
  add(new ZAddCmd(kCmdNameZAdd, -4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZCardCmd(kCmdNameZCard, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZScanCmd(kCmdNameZScan, -3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZIncrbyCmd(kCmdNameZIncrby, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRangeCmd(kCmdNameZRange, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRevrangeCmd(kCmdNameZRevrange, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRangebyscoreCmd(kCmdNameZRangebyscore, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRevrangebyscoreCmd(kCmdNameZRevrangebyscore, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZCountCmd(kCmdNameZCount, 4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRemCmd(kCmdNameZRem, -3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZUnionstoreCmd(kCmdNameZUnionstore, -4, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsZset));
  add(new ZInterstoreCmd(kCmdNameZInterstore, -4, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsZset));
  add(new ZRankCmd(kCmdNameZRank, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRevrankCmd(kCmdNameZRevrank, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZScoreCmd(kCmdNameZScore, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRangebylexCmd(kCmdNameZRangebylex, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRevrangebylexCmd(kCmdNameZRevrangebylex, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZLexcountCmd(kCmdNameZLexcount, 4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRemrangebyrankCmd(kCmdNameZRemrangebyrank, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRemrangebyscoreCmd(kCmdNameZRemrangebyscore, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZRemrangebylexCmd(kCmdNameZRemrangebylex, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZPopmaxCmd(kCmdNameZPopmax, -2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));
  add(new ZPopminCmd(kCmdNameZPopmin, -2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsZset));

  //Set
  add(new SAddCmd(kCmdNameSAdd, -3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsSet));
  add(new SPopCmd(kCmdNameSPop, 2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsSet));
  add(new SCardCmd(kCmdNameSCard, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsSet));
  add(new SMembersCmd(kCmdNameSMembers, 2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsSet));
  add(new SScanCmd(kCmdNameSScan, -3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsSet));
  add(new SRemCmd(kCmdNameSRem, -3, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsSet));
  add(new SUnionCmd(kCmdNameSUnion, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsSet));
  add(new SUnionstoreCmd(kCmdNameSUnionstore, -3, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsSet));
  add(new SInterCmd(kCmdNameSInter, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsSet));
  add(new SInterstoreCmd(kCmdNameSInterstore, -3, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsSet));
  add(new SIsmemberCmd(kCmdNameSIsmember, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsSet));
  add(new SDiffCmd(kCmdNameSDiff, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsSet));
  add(new SDiffstoreCmd(kCmdNameSDiffstore, -3, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsSet));
  add(new SMoveCmd(kCmdNameSMove, 4, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsSet));
  add(new SRandmemberCmd(kCmdNameSRandmember, -2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsSet));

  //BitMap
  add(new BitSetCmd(kCmdNameBitSet, 4, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsBit));
  add(new BitGetCmd(kCmdNameBitGet, 3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsBit));
  add(new BitCountCmd(kCmdNameBitCount, -2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsBit));
  add(new BitPosCmd(kCmdNameBitPos, -3, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsBit));
  add(new BitOpCmd(kCmdNameBitOp, -3, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsBit));

  //HyperLogLog
  add(new PfAddCmd(kCmdNamePfAdd, -2, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsHyperLogLog));
  add(new PfCountCmd(kCmdNamePfCount, -2, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsHyperLogLog));
  add(new PfMergeCmd(kCmdNamePfMerge, -3, kCmdFlagsWrite | kCmdFlagsMultiPartition | kCmdFlagsHyperLogLog));

  //GEO
  add(new GeoAddCmd(kCmdNameGeoAdd, -5, kCmdFlagsWrite | kCmdFlagsSinglePartition | kCmdFlagsGeo));
  add(new GeoPosCmd(kCmdNameGeoPos, -2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsGeo));
  add(new GeoDistCmd(kCmdNameGeoDist, -4, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsGeo));
  add(new GeoHashCmd(kCmdNameGeoHash, -2, kCmdFlagsRead | kCmdFlagsSinglePartition | kCmdFlagsGeo));
  add(new GeoRadiusCmd(kCmdNameGeoRadius, -6, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsGeo));
  add(new GeoRadiusByMemberCmd(kCmdNameGeoRadiusByMember, -5, kCmdFlagsRead | kCmdFlagsMultiPartition | kCmdFlagsGeo));

  //PubSub
  add(new PublishCmd(kCmdNamePublish, 3, kCmdFlagsRead | kCmdFlagsPubSub));
  add(new SubscribeCmd(kCmdNameSubscribe, -2, kCmdFlagsRead | kCmdFlagsPubSub));
  add(new UnSubscribeCmd(kCmdNameUnSubscribe, -1, kCmdFlagsRead | kCmdFlagsPubSub));
  add(new PSubscribeCmd(kCmdNamePSubscribe, -2, kCmdFlagsRead | kCmdFlagsPubSub));
  add(new PUnSubscribeCmd(kCmdNamePUnSubscribe, -1, kCmdFlagsRead | kCmdFlagsPubSub));
  add(new PubSubCmd(kCmdNamePubSub, -2, kCmdFlagsRead | kCmdFlagsPubSub));

  // total_key_size() with align = 1960, fit to 2032 = 8*(255-1), ok to use uint8_t for LinkTp
  fprintf(stderr, "%s: cmdtab->total_key_size() = %zd\n", __func__, cmd_table->total_key_size());
}

void Init_g_pika_cmd_run_time_histogram() {
  g_pika_cmd_run_time_histogram->m_get_idx  = &CmdTable_get_idx;
  g_pika_cmd_run_time_histogram->m_get_name = &CmdTable_get_name;
  const CmdTable* cmdtab = g_pika_cmd_table_manager->cmds();
  TERARK_VERIFY_EQ(cmdtab->end_i(), PikaCmdRunTimeHistogram::HistogramNum);
  for (size_t i = 0, n = cmdtab->end_i(); i < n; ++i) {
    g_pika_cmd_run_time_histogram->AddHistogram(cmdtab->key(i));
  }
}

Cmd* GetCmdFromTable(const fstring& opt, const CmdTable& cmd_table) {
  size_t idx = cmd_table.find_i(opt);
  if (cmd_table.end_i() != idx) {
    return cmd_table.val(idx);
  }
  return NULL;
}

void DestoryCmdTable(CmdTable* cmd_table) {
  TERARK_VERIFY_EZ(cmd_table->delcnt());
  for (size_t i = 0, n = cmd_table->end_i(); i < n; ++i) {
    delete cmd_table->val(i);
  }
  cmd_table->clear();
}

Cmd::Cmd(fstring name, int arity, uint16_t flag)
    : stage_(kNone), arity_(arity), flag_(flag), do_duration_(0)
{
  special_cmd_ = SpecialCmd::kGeneral;
  TERARK_VERIFY_LT(name.size(), sizeof(name_));
  memcpy(name_, name.p, name.n);
  name_len_ = uint8_t(name.n);
}
Cmd::~Cmd() = default;

void Cmd::Initial(const PikaCmdArgsType& argv,
                  const std::string& table_name) {
  argv_ = argv;
  table_name_ = table_name;
  res_.clear(); // Clear res content
  Clear();      // Clear cmd, Derived class can has own implement
  DoInitial();
};

std::vector<std::string> Cmd::current_key() const {
  std::vector<std::string> res;
  res.push_back("");
  return res;
}

void Cmd::Execute() {
  switch (special_cmd_) {
  case SpecialCmd::kGeneral:
    if (is_single_partition() ||
        (g_pika_conf->classic_mode() && g_pika_conf->consensus_level() == 0)) {
      ProcessSinglePartitionCmd();
    } else if (is_multi_partition()) {
      ProcessMultiPartitionCmd();
    } else {
      ProcessDoNotSpecifyPartitionCmd();
    }
    break;
  case SpecialCmd::kFlushDB:  ProcessFlushDBCmd();  break;
  case SpecialCmd::kFlushAll: ProcessFlushAllCmd(); break;
  case SpecialCmd::kInfo:
  case SpecialCmd::kConfig:
    ProcessDoNotSpecifyPartitionCmd();
    break;
  default: TERARK_DIE("bad special_cmd_ = %d", int(special_cmd_));
  }
}

void Cmd::ProcessFlushDBCmd() {
  std::shared_ptr<Table> table = g_pika_server->GetTable(table_name_);
  if (!table) {
    res_.SetRes(CmdRes::kInvalidTable);
  } else {
    if (table->IsKeyScaning()) {
      res_.SetRes(CmdRes::kErrOther, "The keyscan operation is executing, Try again later");
    } else {
      slash::RWLock l_prw(&table->partitions_rw_, true);
      slash::RWLock s_prw(&g_pika_rm->partitions_rw_, true);
      for (const auto& partition_item : table->partitions_) {
        std::shared_ptr<Partition> partition = partition_item.second;
        PartitionInfo p_info(partition->GetTableName(), partition->GetPartitionId());
        if (g_pika_rm->sync_master_partitions_.find(p_info)
            == g_pika_rm->sync_master_partitions_.end()) {
          res_.SetRes(CmdRes::kErrOther, "Partition not found");
          return;
        }
        ProcessCommand(partition, g_pika_rm->sync_master_partitions_[p_info]);
      }
      res_.SetRes(CmdRes::kOk);
    }
  }
}

void Cmd::ProcessFlushAllCmd() {
  slash::RWLock l_trw(&g_pika_server->tables_rw_, true);
  for (const auto& table_item : g_pika_server->tables_) {
    if (!table_item.second) {
      continue;
    }
    if (table_item.second->IsKeyScaning()) {
      res_.SetRes(CmdRes::kErrOther, "The keyscan operation is executing, Try again later");
      return;
    }
  }

  for (const auto& table_item : g_pika_server->tables_) {
    if (!table_item.second) {
      continue;
    }
    slash::RWLock l_prw(&table_item.second->partitions_rw_, true);
    slash::RWLock s_prw(&g_pika_rm->partitions_rw_, true);
    for (const auto& partition_item : table_item.second->partitions_) {
      std::shared_ptr<Partition> partition = partition_item.second;
      PartitionInfo p_info(partition->GetTableName(), partition->GetPartitionId());
      if (g_pika_rm->sync_master_partitions_.find(p_info)
          == g_pika_rm->sync_master_partitions_.end()) {
        res_.SetRes(CmdRes::kErrOther, "Partition not found");
        return;
      }
      ProcessCommand(partition, g_pika_rm->sync_master_partitions_[p_info]);
    }
  }
  res_.SetRes(CmdRes::kOk);
}

void Cmd::ProcessSinglePartitionCmd() {
  const std::shared_ptr<Partition>* partitionpp = nullptr;
  if (g_pika_conf->classic_mode()) {
    // in classic mode a table has only one partition
    partitionpp = &g_pika_server->GetPartitionByDbName(table_name_);
  } else {
    std::vector<std::string> cur_key = current_key();
    if (cur_key.empty()) {
      res_.SetRes(CmdRes::kErrOther, "Internal Error");
      return;
    }
    // in sharding mode we select partition by key
    partitionpp = &g_pika_server->GetTablePartitionByKey(table_name_, cur_key.front());
  }

  const auto& partition = *partitionpp;
  if (!partition) {
    res_.SetRes(CmdRes::kErrOther, "Partition not found");
    return;
  }

  std::shared_ptr<SyncMasterPartition>& sync_partition =
    g_pika_rm->GetSyncMasterPartitionByName(
        PartitionInfo(partition->GetTableName(), partition->GetPartitionId()));
  if (!sync_partition) {
    res_.SetRes(CmdRes::kErrOther, "Partition not found");
    return;
  }
  ProcessCommand(partition, sync_partition);
}

void Cmd::ProcessCommand(const std::shared_ptr<Partition>& partition,
    const std::shared_ptr<SyncMasterPartition>& sync_partition,
    const HintKeys& hint_keys) {
  switch (stage_) {
  default: TERARK_DIE("bad stage_ = %d", stage_);
  case kNone:
    InternalProcessCommand(partition, sync_partition, hint_keys);
    break;
  case kBinlogStage:  DoBinlog(sync_partition)       ; break;
  case kExecuteStage: DoCommand(partition, hint_keys); break;
  }
}

void Cmd::InternalProcessCommand(const std::shared_ptr<Partition>& partition,
    const std::shared_ptr<SyncMasterPartition>& sync_partition, const HintKeys& hint_keys) {
  std::vector<std::string> cur_keys;
  const std::vector<std::string>* p_keys = nullptr;
  if (is_write()) {
    if (!hint_keys.empty() && is_multi_partition() &&
     !g_pika_conf->classic_mode() && g_pika_conf->consensus_level() == 0) {
      p_keys = &hint_keys.keys;
    } else {
      cur_keys = current_key();
      p_keys = &cur_keys;
    }
  }
  size_t num = p_keys ? p_keys->size() : 0;
  terark::AutoFree<rocksdb::Slice> keys_ref(num);
  for (size_t i = 0; i < num; ++i) keys_ref.p[i] = (*p_keys)[i];
  blackwidow::MultiScopeRecordLock lock(partition->LockMgr(), keys_ref.p, num);

  if (g_pika_conf->slowlog_slower_than() >= 0) {
    auto start_tp = terark::qtime::now();
    DoCommand(partition, hint_keys);
    do_duration_ += start_tp.us(terark::qtime::now());
  }
  else {
    DoCommand(partition, hint_keys);
  }
  DoBinlog(sync_partition);
}

void Cmd::DoCommand(const std::shared_ptr<Partition>& partition, const HintKeys& hint_keys) {
  if (!is_suspend()) {
    partition->DbRWLockReader();
  }

  if (!hint_keys.empty() && is_multi_partition() &&
     !g_pika_conf->classic_mode() && g_pika_conf->consensus_level() == 0) {
    Split(partition, hint_keys);
  } else {
    Do(partition);
  }

  if (!is_suspend()) {
    partition->DbRWUnLock();
  }
}

void Cmd::DoBinlog(const std::shared_ptr<SyncMasterPartition>& partition) {
  if (res().ok() && is_write() && g_pika_conf->write_binlog()) {
    std::shared_ptr<pink::PinkConn> conn_ptr = GetConn();
    std::shared_ptr<std::string> resp_ptr = GetResp();
    // Consider that dummy cmd appended by system, both conn and resp are null.
    if ((!conn_ptr || !resp_ptr) && (name_ != kCmdDummy)) {
      if (!conn_ptr) {
        LOG(WARNING) << partition->SyncPartitionInfo().ToString() << " conn empty.";
      }
      if (!resp_ptr) {
        LOG(WARNING) << partition->SyncPartitionInfo().ToString() << " resp empty.";
      }
      res().SetRes(CmdRes::kErrOther);
      return;
    }

    Status s = partition->ConsensusProposeLog(shared_from_this(),
        std::dynamic_pointer_cast<PikaClientConn>(conn_ptr), resp_ptr);
    if (!s.ok()) {
      LOG(WARNING) << partition->SyncPartitionInfo().ToString()
      << " Writing binlog failed, maybe no space left on device " << s.ToString();
      res().SetRes(CmdRes::kErrOther, s.ToString());
      return;
    }
  }
}

void Cmd::ProcessMultiPartitionCmd() {
  std::shared_ptr<Partition> partition;
  std::vector<std::string> cur_key = current_key();
  if (cur_key.empty()) {
    res_.SetRes(CmdRes::kErrOther, "Internal Error");
    return;
  }

  int hint = 0;
  std::unordered_map<uint32_t, ProcessArg> process_map;
  // split cur_key into partitions
  std::shared_ptr<Table> table = g_pika_server->GetTable(table_name_);
  if (!table) {
    res_.SetRes(CmdRes::kErrOther, "Table not found");
  }
  for (auto& key : cur_key) {
    // in sharding mode we select partition by key
    uint32_t partition_id =  g_pika_cmd_table_manager->DistributeKey(key, table->PartitionNum());
    std::unordered_map<uint32_t, ProcessArg>::iterator iter = process_map.find(partition_id);
    if (iter == process_map.end()) {
      std::shared_ptr<Partition> partition =  table->GetPartitionById(partition_id);
      if (!partition) {
        res_.SetRes(CmdRes::kErrOther, "Partition not found");
        return;
      }
      std::shared_ptr<SyncMasterPartition> sync_partition =
      g_pika_rm->GetSyncMasterPartitionByName(
          PartitionInfo(partition->GetTableName(), partition->GetPartitionId()));
      if (!sync_partition) {
        res_.SetRes(CmdRes::kErrOther, "Partition not found");
        return;
      }
      HintKeys hint_keys;
      hint_keys.Push(key, hint);
      process_map[partition_id] = ProcessArg(partition, sync_partition, hint_keys);
    } else {
      iter->second.hint_keys.Push(key, hint);
    }
    hint++;
  }
  for (auto& iter : process_map) {
    ProcessArg& arg = iter.second;
    ProcessCommand(arg.partition, arg.sync_partition, arg.hint_keys);
    if (!res_.ok()) {
      return;
    }
  }
  Merge();
}

void Cmd::ProcessDoNotSpecifyPartitionCmd() {
  Do();
}

bool Cmd::is_write() const {
  return ((flag_ & kCmdFlagsMaskRW) == kCmdFlagsWrite);
}
bool Cmd::is_local() const {
  return ((flag_ & kCmdFlagsMaskLocal) == kCmdFlagsLocal);
}
// Others need to be suspended when a suspend command run
bool Cmd::is_suspend() const {
  return ((flag_ & kCmdFlagsMaskSuspend) == kCmdFlagsSuspend);
}
// Must with admin auth
bool Cmd::is_admin_require() const {
  return ((flag_ & kCmdFlagsMaskAdminRequire) == kCmdFlagsAdminRequire);
}
bool Cmd::is_single_partition() const {
  return ((flag_ & kCmdFlagsMaskPartition) == kCmdFlagsSinglePartition);
}
bool Cmd::is_multi_partition() const {
  return ((flag_ & kCmdFlagsMaskPartition) == kCmdFlagsMultiPartition);
}

bool Cmd::is_classic_mode() const {
  return g_pika_conf->classic_mode();
}

bool Cmd::HashtagIsConsistent(const std::string& lhs, const std::string& rhs) const {
  if (is_classic_mode() == false) {
    if (GetHashkey(lhs) != GetHashkey(rhs)) {
      return false;
    }
  }
  return true;
}

const PikaCmdArgsType& Cmd::argv() const {
  return argv_;
}

std::string Cmd::ToBinlog(uint32_t exec_time,
                          uint32_t term_id,
                          uint64_t logic_id,
                          uint32_t filenum,
                          uint64_t offset) {
  dstring content;
  content.reserve(RAW_ARGS_LEN);
  RedisAppendLen(content, argv_.size(), "*");

  for (const auto& v : argv_) {
    RedisAppendLen(content, v.size(), "$");
    RedisAppendContent(content, v);
  }

  return PikaBinlogTransverter::BinlogEncode(BinlogType::TypeFirst,
                                             exec_time,
                                             term_id,
                                             logic_id,
                                             filenum,
                                             offset,
                                             content,
                                             {});
}

bool Cmd::CheckArg(int num) const {
  if ((arity_ > 0 && num != arity_)
    || (arity_ < 0 && num < -arity_)) {
    return false;
  }
  return true;
}

void Cmd::LogCommand() const {
  std::string command;
  for (const auto& item : argv_) {
    command.append(" ");
    command.append(item);
  }
  LOG(INFO) << "command:" << command;
}

void Cmd::SetConn(const std::shared_ptr<pink::PinkConn>& conn) {
  conn_ = conn;
}

std::shared_ptr<pink::PinkConn> Cmd::GetConn() {
  return conn_.lock();
}

void Cmd::SetResp(const std::shared_ptr<std::string>& resp) {
  resp_ = resp;
}

std::shared_ptr<std::string> Cmd::GetResp() {
  return resp_.lock();
}

void Cmd::SetStage(CmdStage stage) {
  stage_ = stage;
}
