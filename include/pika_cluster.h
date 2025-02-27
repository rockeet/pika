// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_CLUSTER_H_
#define PIKA_CLUSTER_H_

#include "include/pika_command.h"


Status ParseSlotGroup(const std::string& slot_group,
    std::set<uint32_t>* slots);

class PkClusterInfoCmd : public Cmd {
 public:
  enum InfoSection {
    kInfoErr = 0x0,
    kInfoSlot,
    kInfoTable
  };
  enum InfoRange {
    kSingle = 0x0,
    kAll,
    kRange
  };
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PkClusterInfoCmd(*this); }

 private:
  InfoSection info_section_ = kInfoErr;
  InfoRange info_range_ = kAll;

  std::string table_name_;
  std::set<uint32_t> slots_;

  void DoInitial() override;
  void Clear() override {
    info_section_ = kInfoErr;
    info_range_ = kAll;
    table_name_.clear();
    slots_.clear();
  }
  const static std::string kSlotSection;
  const static std::string kTableSection;
  void ClusterInfoTableAll(std::string* info);
  void ClusterInfoTable(std::string* info);
  void ClusterInfoSlotRange(const std::string& table_name, const std::set<uint32_t>& slots,
      std::string* info);
  void ClusterInfoSlotAll(std::string* info);
  Status GetSlotInfo(const std::string& table_name, uint32_t partition_id, std::string* info);
  bool ParseInfoSlotSubCmd();
  bool ParseInfoTableSubCmd();
};

class SlotParentCmd : public Cmd {
 public:
  using Cmd::Cmd;

 protected:
  std::set<uint32_t> slots_;
  std::set<PartitionInfo> p_infos_;
  void DoInitial() override;
  void Clear() override {
    slots_.clear();
    p_infos_.clear();
    table_name_.clear();
  }
};

class PkClusterAddSlotsCmd : public SlotParentCmd {
 public:
  using SlotParentCmd::SlotParentCmd;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PkClusterAddSlotsCmd(*this); }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
 private:
  void DoInitial() override;
  Status AddSlotsSanityCheck();
};

class PkClusterDelSlotsCmd : public SlotParentCmd {
 public:
  using SlotParentCmd::SlotParentCmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PkClusterDelSlotsCmd(*this); }
 private:
  void DoInitial() override;
  Status RemoveSlotsSanityCheck();
};

class PkClusterSlotsSlaveofCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PkClusterSlotsSlaveofCmd(*this); }
 private:
  std::string ip_;
  int32_t port_ = 0;
  bool force_sync_ = false;
  bool is_noone_ = false;
  std::set<uint32_t> slots_;
  void DoInitial() override;
  void Clear() override {
    ip_.clear();
    port_ = 0;
    slots_.clear();
    force_sync_ = false;
    is_noone_ = false;
    table_name_.clear();
  }
};


class PkClusterAddTableCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PkClusterAddTableCmd(*this); }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
 private:
  uint64_t slot_num_ = 0;
  void DoInitial() override;
  Status AddTableSanityCheck();
  void Clear() override {
    slot_num_ = 0;
    table_name_.clear();
  }
};

class PkClusterDelTableCmd : public PkClusterDelSlotsCmd {
 public:
  using PkClusterDelSlotsCmd::PkClusterDelSlotsCmd;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PkClusterDelTableCmd(*this); }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
 private:
  void DoInitial() override;
  Status DelTableSanityCheck(const std::string& table_name);
};

#endif  // PIKA_CLUSTER_H_
