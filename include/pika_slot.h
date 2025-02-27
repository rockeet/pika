// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_SLOT_H_
#define PIKA_SLOT_H_

#include "include/pika_command.h"

class SlotsInfoCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsInfoCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsHashKeyCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsHashKeyCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsMgrtSlotAsyncCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtSlotAsyncCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsMgrtTagSlotAsyncCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtTagSlotAsyncCmd(*this); }
 private:
  void DoInitial() override;
  std::string dest_ip_;
  int64_t dest_port_ = 0;
  int64_t slot_num_ = -1;
  void Clear() override {
    dest_ip_.clear();
    dest_port_ = 0;
    slot_num_ = -1;
  }
};

class SlotsScanCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsScanCmd(*this); }
 private:
  int64_t cursor_;
  uint32_t slotnum_;
  std::string pattern_ = "*";
  int64_t count_ = 10;
  void DoInitial() override;
  void Clear() override {
    pattern_ = "*";
    count_ = 10;
  }
};

class SlotsDelCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsDelCmd(*this); }
 private:
  std::vector<uint32_t> slots_;
  void DoInitial() override;
  void Clear() override {
    slots_.clear();
  }
};

class SlotsMgrtExecWrapperCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtExecWrapperCmd(*this); }
 private:
  std::string key_;
  void DoInitial() override;
  void Clear() override {
    key_.clear();
  }
};

class SlotsMgrtAsyncStatusCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtAsyncStatusCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsMgrtAsyncCancelCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtAsyncCancelCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsMgrtSlotCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtSlotCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsMgrtTagSlotCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtTagSlotCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsMgrtOneCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtOneCmd(*this); }
 private:
  void DoInitial() override;
};

class SlotsMgrtTagOneCmd : public Cmd {
 public:
  using Cmd::Cmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new SlotsMgrtTagOneCmd(*this); }
 private:
  void DoInitial() override;
};

#endif  // PIKA_SLOT_H_
