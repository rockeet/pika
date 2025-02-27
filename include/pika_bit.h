// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_BIT_H_
#define PIKA_BIT_H_

#include "blackwidow/blackwidow.h"

#include "include/pika_command.h"
#include "include/pika_partition.h"

/*
 * bitoperation
 */
class BitGetCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  virtual void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new BitGetCmd(*this); }
 private:
  std::string key_;
  int64_t  bit_offset_;
  void Clear() override {
    key_ = "";
    bit_offset_ = -1;
  }
  void DoInitial() override;
};

class BitSetCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  virtual void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new BitSetCmd(*this); }
 private:
  std::string key_;
  int64_t  bit_offset_;
  int64_t  on_;
  void Clear() override {
    key_ = "";
    bit_offset_ = -1;
    on_ = -1;
  }
  void DoInitial() override;
};

class BitCountCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  virtual void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new BitCountCmd(*this); }
 private:
  std::string key_;
  bool  count_all_;
  int64_t  start_offset_;
  int64_t  end_offset_;
  void Clear() override {
    key_ = "";
    count_all_ = false;
    start_offset_ = -1;
    end_offset_ = -1;
  }
  void DoInitial() override;
};

class BitPosCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  virtual void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new BitPosCmd(*this); }
 private:
  std::string key_;
  bool  pos_all_;
  bool  endoffset_set_;
  int64_t bit_val_;
  int64_t  start_offset_;
  int64_t  end_offset_;
  void Clear() override {
    key_ = "";
    pos_all_ = false;
    endoffset_set_ = false;
    bit_val_ = -1;
    start_offset_ = -1;
    end_offset_ = -1;
  }
  void DoInitial() override;
};

class BitOpCmd : public Cmd {
 public:
  using Cmd::Cmd;
  virtual void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new BitOpCmd(*this); }
 private:
  std::string dest_key_;
  std::vector<std::string> src_keys_;
  blackwidow::BitOpType op_;
  void Clear() override {
    dest_key_ = "";
    src_keys_.clear();
    op_ = blackwidow::kBitOpDefault;
  }
  void DoInitial() override;
};
#endif
