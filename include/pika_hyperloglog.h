// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_HYPERLOGLOG_H_
#define PIKA_HYPERLOGLOG_H_

#include "include/pika_command.h"
#include "include/pika_partition.h"

/*
 * hyperloglog
 */
class PfAddCmd : public Cmd {
 public:
  PfAddCmd(const fstring name, int arity, uint16_t flag)
      : Cmd(name,  arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PfAddCmd(*this); }
 private:
  std::string key_;
  std::vector<std::string> values_;
  void DoInitial() override;
  void Clear() override {
    values_.clear();
  }
};

class PfCountCmd : public Cmd {
 public:
  PfCountCmd(const fstring name, int arity, uint16_t flag)
      : Cmd(name,  arity, flag) {}
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PfCountCmd(*this); }
 private:
  std::vector<std::string> keys_;
  void DoInitial() override;
  void Clear() override {
    keys_.clear();
  }
};

class PfMergeCmd : public Cmd {
 public:
  PfMergeCmd(const fstring name, int arity, uint16_t flag)
      : Cmd(name,  arity, flag) {}
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new PfMergeCmd(*this); }
 private:
  std::vector<std::string> keys_;
  void DoInitial() override;
  void Clear() override {
    keys_.clear();
  }
};

#endif
