// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_COMMAND_H_
#define PIKA_COMMAND_H_

#include <unordered_map>

#include "pink/include/redis_conn.h"
#include "pink/include/pink_conn.h"
#include "slash/include/slash_string.h"

#include "include/pika_partition.h"
#include <terark/hash_strmap.hpp>

using terark::fstring;

class SyncMasterPartition;
class SyncSlavePartition;

//Constant for command name
//Admin
const fstring kCmdNameSlaveof = "slaveof";
const fstring kCmdNameDbSlaveof = "dbslaveof";
const fstring kCmdNameAuth = "auth";
const fstring kCmdNameBgsave = "bgsave";
const fstring kCmdNameCompact = "compact";
const fstring kCmdNamePurgelogsto = "purgelogsto";
const fstring kCmdNamePing = "ping";
const fstring kCmdNameSelect = "select";
const fstring kCmdNameFlushall = "flushall";
const fstring kCmdNameFlushdb = "flushdb";
const fstring kCmdNameClient = "client";
const fstring kCmdNameShutdown = "shutdown";
const fstring kCmdNameInfo = "info";
const fstring kCmdNameConfig = "config";
const fstring kCmdNameMonitor = "monitor";
const fstring kCmdNameDbsize = "dbsize";
const fstring kCmdNameTime = "time";
const fstring kCmdNameDelbackup = "delbackup";
const fstring kCmdNameEcho = "echo";
const fstring kCmdNameScandb = "scandb";
const fstring kCmdNameSlowlog = "slowlog";
const fstring kCmdNamePadding = "padding";
#ifdef TCMALLOC_EXTENSION
const fstring kCmdNameTcmalloc = "tcmalloc";
#endif
const fstring kCmdNamePKPatternMatchDel = "pkpatternmatchdel";
const std::string kCmdDummy = "dummy";

//Kv
const fstring kCmdNameSet = "set";
const fstring kCmdNameGet = "get";
const fstring kCmdNameDel = "del";
const fstring kCmdNameIncr = "incr";
const fstring kCmdNameIncrby = "incrby";
const fstring kCmdNameIncrbyfloat = "incrbyfloat";
const fstring kCmdNameDecr = "decr";
const fstring kCmdNameDecrby = "decrby";
const fstring kCmdNameGetset = "getset";
const fstring kCmdNameAppend = "append";
const fstring kCmdNameMget = "mget";
const fstring kCmdNameKeys = "keys";
const fstring kCmdNameSetnx = "setnx";
const fstring kCmdNameSetex = "setex";
const fstring kCmdNamePsetex = "psetex";
const fstring kCmdNameDelvx = "delvx";
const fstring kCmdNameMset = "mset";
const fstring kCmdNameMsetnx = "msetnx";
const fstring kCmdNameGetrange = "getrange";
const fstring kCmdNameSetrange = "setrange";
const fstring kCmdNameStrlen = "strlen";
const fstring kCmdNameExists = "exists";
const fstring kCmdNameExpire = "expire";
const fstring kCmdNamePexpire = "pexpire";
const fstring kCmdNameExpireat = "expireat";
const fstring kCmdNamePexpireat = "pexpireat";
const fstring kCmdNameTtl = "ttl";
const fstring kCmdNamePttl = "pttl";
const fstring kCmdNamePersist = "persist";
const fstring kCmdNameType = "type";
const fstring kCmdNameScan = "scan";
const fstring kCmdNameScanx = "scanx";
const fstring kCmdNamePKSetexAt = "pksetexat";
const fstring kCmdNamePKScanRange = "pkscanrange";
const fstring kCmdNamePKRScanRange = "pkrscanrange";

//Hash
const fstring kCmdNameHDel = "hdel";
const fstring kCmdNameHSet = "hset";
const fstring kCmdNameHGet = "hget";
const fstring kCmdNameHGetall = "hgetall";
const fstring kCmdNameHExists = "hexists";
const fstring kCmdNameHIncrby = "hincrby";
const fstring kCmdNameHIncrbyfloat = "hincrbyfloat";
const fstring kCmdNameHKeys = "hkeys";
const fstring kCmdNameHLen = "hlen";
const fstring kCmdNameHMget = "hmget";
const fstring kCmdNameHMset = "hmset";
const fstring kCmdNameHSetnx = "hsetnx";
const fstring kCmdNameHStrlen = "hstrlen";
const fstring kCmdNameHVals = "hvals";
const fstring kCmdNameHScan = "hscan";
const fstring kCmdNameHScanx = "hscanx";
const fstring kCmdNamePKHScanRange = "pkhscanrange";
const fstring kCmdNamePKHRScanRange = "pkhrscanrange";

//List
const fstring kCmdNameLIndex = "lindex";
const fstring kCmdNameLInsert = "linsert";
const fstring kCmdNameLLen = "llen";
const fstring kCmdNameLPop = "lpop";
const fstring kCmdNameLPush = "lpush";
const fstring kCmdNameLPushx = "lpushx";
const fstring kCmdNameLRange = "lrange";
const fstring kCmdNameLRem = "lrem";
const fstring kCmdNameLSet = "lset";
const fstring kCmdNameLTrim = "ltrim";
const fstring kCmdNameRPop = "rpop";
const fstring kCmdNameRPopLPush = "rpoplpush";
const fstring kCmdNameRPush = "rpush";
const fstring kCmdNameRPushx = "rpushx";

//BitMap
const fstring kCmdNameBitSet = "setbit";
const fstring kCmdNameBitGet = "getbit";
const fstring kCmdNameBitPos = "bitpos";
const fstring kCmdNameBitOp = "bitop";
const fstring kCmdNameBitCount = "bitcount";

//Zset
const fstring kCmdNameZAdd = "zadd";
const fstring kCmdNameZCard = "zcard";
const fstring kCmdNameZScan = "zscan";
const fstring kCmdNameZIncrby = "zincrby";
const fstring kCmdNameZRange = "zrange";
const fstring kCmdNameZRangebyscore = "zrangebyscore";
const fstring kCmdNameZCount = "zcount";
const fstring kCmdNameZRem = "zrem";
const fstring kCmdNameZUnionstore = "zunionstore";
const fstring kCmdNameZInterstore = "zinterstore";
const fstring kCmdNameZRank = "zrank";
const fstring kCmdNameZRevrank = "zrevrank";
const fstring kCmdNameZScore = "zscore";
const fstring kCmdNameZRevrange = "zrevrange";
const fstring kCmdNameZRevrangebyscore = "zrevrangebyscore";
const fstring kCmdNameZRangebylex = "zrangebylex";
const fstring kCmdNameZRevrangebylex = "zrevrangebylex";
const fstring kCmdNameZLexcount = "zlexcount";
const fstring kCmdNameZRemrangebyrank = "zremrangebyrank";
const fstring kCmdNameZRemrangebylex = "zremrangebylex";
const fstring kCmdNameZRemrangebyscore = "zremrangebyscore";
const fstring kCmdNameZPopmax = "zpopmax";
const fstring kCmdNameZPopmin = "zpopmin";

//Set
const fstring kCmdNameSAdd = "sadd";
const fstring kCmdNameSPop = "spop";
const fstring kCmdNameSCard = "scard";
const fstring kCmdNameSMembers = "smembers";
const fstring kCmdNameSScan = "sscan";
const fstring kCmdNameSRem = "srem";
const fstring kCmdNameSUnion = "sunion";
const fstring kCmdNameSUnionstore = "sunionstore";
const fstring kCmdNameSInter = "sinter";
const fstring kCmdNameSInterstore = "sinterstore";
const fstring kCmdNameSIsmember = "sismember";
const fstring kCmdNameSDiff = "sdiff";
const fstring kCmdNameSDiffstore = "sdiffstore";
const fstring kCmdNameSMove = "smove";
const fstring kCmdNameSRandmember = "srandmember";

//HyperLogLog
const fstring kCmdNamePfAdd = "pfadd";
const fstring kCmdNamePfCount = "pfcount";
const fstring kCmdNamePfMerge = "pfmerge";

//GEO
const fstring kCmdNameGeoAdd = "geoadd";
const fstring kCmdNameGeoPos = "geopos";
const fstring kCmdNameGeoDist = "geodist";
const fstring kCmdNameGeoHash = "geohash";
const fstring kCmdNameGeoRadius = "georadius";
const fstring kCmdNameGeoRadiusByMember = "georadiusbymember";

//Pub/Sub
const fstring kCmdNamePublish = "publish";
const fstring kCmdNameSubscribe = "subscribe";
const fstring kCmdNameUnSubscribe = "unsubscribe";
const fstring kCmdNamePubSub = "pubsub";
const fstring kCmdNamePSubscribe = "psubscribe";
const fstring kCmdNamePUnSubscribe = "punsubscribe";

//Codis Slots
const fstring kCmdNameSlotsInfo = "slotsinfo";
const fstring kCmdNameSlotsHashKey = "slotshashkey";
const fstring kCmdNameSlotsMgrtTagSlotAsync = "slotsmgrttagslot-async";
const fstring kCmdNameSlotsMgrtSlotAsync = "slotsmgrtslot-async";
const fstring kCmdNameSlotsDel = "slotsdel";
const fstring kCmdNameSlotsScan = "slotsscan";
const fstring kCmdNameSlotsMgrtExecWrapper = "slotsmgrt-exec-wrapper";
const fstring kCmdNameSlotsMgrtAsyncStatus = "slotsmgrt-async-status";
const fstring kCmdNameSlotsMgrtAsyncCancel = "slotsmgrt-async-cancel";
const fstring kCmdNameSlotsMgrtSlot = "slotsmgrtslot";
const fstring kCmdNameSlotsMgrtTagSlot = "slotsmgrttagslot";
const fstring kCmdNameSlotsMgrtOne = "slotsmgrtone";
const fstring kCmdNameSlotsMgrtTagOne = "slotsmgrttagone";


//Cluster
const fstring kCmdNamePkClusterInfo = "pkclusterinfo";
const fstring kCmdNamePkClusterAddSlots = "pkclusteraddslots";
const fstring kCmdNamePkClusterDelSlots = "pkclusterdelslots";
const fstring kCmdNamePkClusterSlotsSlaveof = "pkclusterslotsslaveof";
const fstring kCmdNamePkClusterAddTable = "pkclusteraddtable";
const fstring kCmdNamePkClusterDelTable = "pkclusterdeltable";

const std::string kClusterPrefix = "pkcluster";
typedef pink::RedisCmdArgsType PikaCmdArgsType;
static const int RAW_ARGS_LEN = 1024 * 1024; 

enum CmdFlagsMask {
  kCmdFlagsMaskRW            = 1,
  kCmdFlagsMaskType          = 30,
  kCmdFlagsMaskLocal         = 32,
  kCmdFlagsMaskSuspend       = 64,
  kCmdFlagsMaskPrior         = 128,
  kCmdFlagsMaskAdminRequire  = 256,
  kCmdFlagsMaskPartition     = 1536
};

enum CmdFlags {
  kCmdFlagsRead                  = 0, //default rw
  kCmdFlagsWrite                 = 1,
  kCmdFlagsAdmin                 = 0, //default type
  kCmdFlagsKv                    = 2,
  kCmdFlagsHash                  = 4,
  kCmdFlagsList                  = 6,
  kCmdFlagsSet                   = 8,
  kCmdFlagsZset                  = 10,
  kCmdFlagsBit                   = 12,
  kCmdFlagsHyperLogLog           = 14,
  kCmdFlagsGeo                   = 16,
  kCmdFlagsPubSub                = 18,
  kCmdFlagsNoLocal               = 0, //default nolocal
  kCmdFlagsLocal                 = 32,
  kCmdFlagsNoSuspend             = 0, //default nosuspend
  kCmdFlagsSuspend               = 64,
  kCmdFlagsNoPrior               = 0, //default noprior
  kCmdFlagsPrior                 = 128,
  kCmdFlagsNoAdminRequire        = 0, //default no need admin
  kCmdFlagsAdminRequire          = 256,
  kCmdFlagsDoNotSpecifyPartition = 0, //default do not specify partition
  kCmdFlagsSinglePartition       = 512,
  kCmdFlagsMultiPartition        = 1024
};


void inline RedisAppendContent(dstring& str, const rocksdb::Slice& value);
void inline RedisAppendLen(dstring& str, int64_t ori, const rocksdb::Slice &prefix);

const char kNewLine[] = "\r\n";

class CmdRes {
public:
  enum CmdRet {
    kNone = 0,
    kOk,
    kPong,
    kSyntaxErr,
    kInvalidInt,
    kInvalidBitInt,
    kInvalidBitOffsetInt,
    kInvalidBitPosArgument,
    kWrongBitOpNotNum,
    kInvalidFloat,
    kOverFlow,
    kNotFound,
    kOutOfRange,
    kInvalidPwd,
    kNoneBgsave,
    kPurgeExist,
    kInvalidParameter,
    kWrongNum,
    kInvalidIndex,
    kInvalidDbType,
    kInvalidTable,
    kInconsistentHashTag,
    kErrOther
  };

  CmdRes():ret_(kNone) {}

  bool none() const {
    return ret_ == kNone && message_.empty();
  }
  bool ok() const {
    return ret_ == kOk || ret_ == kNone;
  }
  void clear() {
    message_.clear();
    ret_ = kNone;
  }
  const dstring& raw_message() const {
    return message_;
  }
  dstring message() const {
    dstring result;
    switch (ret_) {
    case kNone:
      return message_;
    case kOk:
      return "+OK\r\n";
    case kPong:
      return "+PONG\r\n";
    case kSyntaxErr:
      return "-ERR syntax error\r\n";
    case kInvalidInt:
      return "-ERR value is not an integer or out of range\r\n";
    case kInvalidBitInt:
      return "-ERR bit is not an integer or out of range\r\n";
    case kInvalidBitOffsetInt:
      return "-ERR bit offset is not an integer or out of range\r\n";
    case kWrongBitOpNotNum:
      return "-ERR BITOP NOT must be called with a single source key.\r\n";

    case kInvalidBitPosArgument:
      return "-ERR The bit argument must be 1 or 0.\r\n";
    case kInvalidFloat:
      return "-ERR value is not a valid float\r\n";
    case kOverFlow:
      return "-ERR increment or decrement would overflow\r\n";
    case kNotFound:
      return "-ERR no such key\r\n";
    case kOutOfRange:
      return "-ERR index out of range\r\n";
    case kInvalidPwd:
      return "-ERR invalid password\r\n";
    case kNoneBgsave:
      return "-ERR No BGSave Works now\r\n";
    case kPurgeExist:
      return "-ERR binlog already in purging...\r\n";
    case kInvalidParameter:
      return "-ERR Invalid Argument\r\n";
    case kWrongNum:
      result = "-ERR wrong number of arguments for '";
      result.append(message_);
      result.append("' command\r\n");
      break;
    case kInvalidIndex:
      result = "-ERR invalid DB index for '";
      result.append(message_);
      result.append("'\r\n");
      break;
    case kInvalidDbType:
      result = "-ERR invalid DB for '";
      result.append(message_);
      result.append("'\r\n");
      break;
    case kInconsistentHashTag:
      return "-ERR parameters hashtag is inconsistent\r\n";
    case kInvalidTable:
      result = "-ERR invalid Table for '";
      result.append(message_);
      result.append("'\r\n");
      break;
    case kErrOther:
      result = "-ERR ";
      result.append(message_);
      result.append(kNewLine);
      break;
    default:
      break;
    }
    return result;
  }

  // Inline functions for Create Redis protocol
  void AppendStringLen(int64_t ori) {
    RedisAppendLen(message_, ori, "$");
  }
  void AppendArrayLen(int64_t ori) {
    RedisAppendLen(message_, ori, "*");
  }
  void AppendInteger(int64_t ori) {
    RedisAppendLen(message_, ori, ":");
  }
  void AppendContent(const rocksdb::Slice& value) {
    RedisAppendContent(message_, value);
  }
  void AppendString(const rocksdb::Slice& value) {
    AppendStringLen(value.size());
    AppendContent(value);
  }
  void AppendString(std::string&& value) {
    if (message_.empty() && value.size() + 12 < value.capacity()) {
      char buf[16]; // insert prefix inplace, more cache friendly
      buf[0] = '$';
      int len = slash::ll2string(buf+1, sizeof(buf), value.size());
      buf[len+1] = '\r';
      buf[len+2] = '\n';
      value.insert(0, buf, len+3);
      value.append("\r\n", 2);
      message_ = std::move(value);
    } else {
      AppendStringLen(value.size());
      AppendContent(value);
    }
  }
  void AppendStringRaw(const Slice& value) {
    message_.append(value.data(), value.size());
  }
  void SetRes(CmdRet _ret) {
    ret_ = _ret;
  }
  void SetRes(CmdRet _ret, std::string&& content) {
    ret_ = _ret;
    if (!content.empty()) {
      message_ = std::move(content);
    }
  }
  void SetRes(CmdRet _ret, const fstring content) {
    ret_ = _ret;
    if (!content.empty()) {
      message_.assign(content.p, content.n);
    }
  }
  void SetRes(CmdRet _ret, const char* content) {
    ret_ = _ret;
    if (content && *content) {
      message_.assign(content);
    }
  }

private:
  dstring message_;
  CmdRet ret_;
};

class Cmd: public std::enable_shared_from_this<Cmd> {
 public:
  enum CmdStage : unsigned char {
    kNone,
    kBinlogStage,
    kExecuteStage
  };
  struct HintKeys {
    void Push(const std::string& key, int hint) {
      keys.push_back(key);
      hints.push_back(hint);
    }
    bool empty() const {
      return keys.empty() && hints.empty();
    }
    std::vector<std::string> keys;
    std::vector<int> hints;
  };
  struct ProcessArg {
    ProcessArg() {}
    ProcessArg(const std::shared_ptr<Partition>& _partition,
        const std::shared_ptr<SyncMasterPartition>& _sync_partition,
        HintKeys _hint_keys) : partition(_partition),
        sync_partition(_sync_partition), hint_keys(_hint_keys) {}
    std::shared_ptr<Partition> partition;
    std::shared_ptr<SyncMasterPartition> sync_partition;
    HintKeys hint_keys;
  };
  Cmd(fstring name, int arity, uint16_t flag);
  virtual ~Cmd();

  virtual std::vector<std::string> current_key() const;
  virtual void Execute();
  virtual void ProcessFlushDBCmd();
  virtual void ProcessFlushAllCmd();
  virtual void ProcessSinglePartitionCmd();
  virtual void ProcessMultiPartitionCmd();
  virtual void ProcessDoNotSpecifyPartitionCmd();
  virtual void Do(const std::shared_ptr<Partition>& = nullptr) = 0;
  virtual Cmd* Clone() = 0;
  // used for execute multikey command into different slots
  virtual void Split(const std::shared_ptr<Partition>&, const HintKeys& hint_keys) = 0;
  virtual void Merge() = 0;

  void Initial(const PikaCmdArgsType& argv,
               const std::string& table_name);

  bool is_write()            const;
  bool is_local()            const;
  bool is_suspend()          const;
  bool is_admin_require()    const;
  bool is_single_partition() const;
  bool is_multi_partition()  const;
  bool is_classic_mode()     const;
  bool HashtagIsConsistent(const std::string& lhs, const std::string& rhs) const;
  uint64_t GetDoDuration() const { return do_duration_; };

  const fstring name() const { return fstring(name_, name_len_); }
  CmdRes& res() { return res_; }
  const std::string& table_name() const { return table_name_; }
  BinlogOffset binlog_offset() const;
  const PikaCmdArgsType& argv() const;
  virtual std::string ToBinlog(uint32_t exec_time,
                               uint32_t term_id,
                               uint64_t logic_id,
                               uint32_t filenum,
                               uint64_t offset);

  void SetConn(const std::shared_ptr<pink::PinkConn>& conn);
  std::shared_ptr<pink::PinkConn> GetConn();

  void SetResp(const std::shared_ptr<std::string>& resp);
  std::shared_ptr<std::string> GetResp();

  void SetStage(CmdStage stage);
  size_t cmd_idx() const { return cmd_idx_; }
  void set_cmd_idx(size_t idx) { cmd_idx_ = uint16_t(idx); }

 protected:
  // enable copy, used default copy
  //Cmd(const Cmd&);
  void ProcessCommand(const std::shared_ptr<Partition>&,
      const std::shared_ptr<SyncMasterPartition>& sync_partition, const HintKeys& hint_key = HintKeys());
  void InternalProcessCommand(const std::shared_ptr<Partition>&,
      const std::shared_ptr<SyncMasterPartition>& sync_partition, const HintKeys& hint_key);
  void DoCommand(const std::shared_ptr<Partition>&, const HintKeys& hint_key);
  void DoBinlog(const std::shared_ptr<SyncMasterPartition>&);
  bool CheckArg(int num) const;
  void LogCommand() const;

  char name_[31]; uint8_t name_len_;
  //char padding_;
  CmdStage stage_;
  int16_t arity_;
  uint16_t flag_;
  uint16_t cmd_idx_ = UINT16_MAX;

  CmdRes res_;
  PikaCmdArgsType argv_;
  std::string table_name_;

  std::weak_ptr<pink::PinkConn> conn_;
  std::weak_ptr<std::string> resp_;
  uint64_t do_duration_;

 private:
  virtual void DoInitial() = 0;
  virtual void Clear() {}

  Cmd& operator=(const Cmd&) = delete;
};

using CmdTableBase = terark::hash_strmap<Cmd*
        , terark::fstring_func::IF_SP_ALIGN(hash_align, hash)
        , terark::fstring_func::IF_SP_ALIGN(equal_align, equal)
        , terark::ValueOut, terark::FastCopy, uint8_t, size_t
        >;
class CmdTable : public CmdTableBase {
  using CmdTableBase::erase;
  using CmdTableBase::find;
  using CmdTableBase::insert;
  using CmdTableBase::operator[];
public:
  using CmdTableBase::CmdTableBase;
};

// Method for Cmd Table
void InitCmdTable(CmdTable* cmd_table);
Cmd* GetCmdFromTable(const std::string& opt, const CmdTable& cmd_table);
void DestoryCmdTable(CmdTable* cmd_table);

void RedisAppendContent(dstring& str, const rocksdb::Slice& value) {
  str.append(value.data(), value.size());
  str.append("\r\n", 2);
}

void RedisAppendLen(dstring& str, int64_t ori, const rocksdb::Slice& prefix) {
  char buf[32];
  slash::ll2string(buf, 32, static_cast<long long>(ori));
  str.append(prefix.data(), prefix.size());
  str.append(buf);
  str.append("\r\n", 2);
}

void TryAliasChange(std::vector<std::string>* argv);

#endif
