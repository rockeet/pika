// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_LIST_H_
#define PIKA_LIST_H_

#include "blackwidow/blackwidow.h"

#include "include/pika_command.h"
#include "include/pika_partition.h"

/*
 * list
 */
class LIndexCmd : public Cmd {
 public:
  LIndexCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag), index_(0) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LIndexCmd(*this); }
 private:
  std::string key_;
  int64_t index_;
  void DoInitial() override;
  void Clear() override {
    index_ = 0;
  }
};

class LInsertCmd : public Cmd {
 public:
  LInsertCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag), dir_(blackwidow::After) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LInsertCmd(*this); }
 private:
  std::string key_;
  blackwidow::BeforeOrAfter dir_;
  std::string pivot_;
  std::string value_;
  void DoInitial() override;
};

class LLenCmd : public Cmd {
 public:
  LLenCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LLenCmd(*this); }
 private:
  std::string key_;
  void DoInitial() override;
};

class LPopCmd : public Cmd {
 public:
  LPopCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LPopCmd(*this); }
 private:
  std::string key_;
  void DoInitial() override;
};

class LPushCmd : public Cmd {
 public:
  LPushCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LPushCmd(*this); }
 private:
  std::string key_;
  std::vector<std::string> values_;
  void DoInitial() override;
  void Clear() override {
    values_.clear();
  }
};

class LPushxCmd : public Cmd {
 public:
  LPushxCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LPushxCmd(*this); }
 private:
  std::string key_;
  std::string value_;
  void DoInitial() override;
};

class LRangeCmd : public Cmd {
 public:
  LRangeCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag), left_(0), right_(0) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LRangeCmd(*this); }
 private:
  std::string key_;
  int64_t left_;
  int64_t right_;
  void DoInitial() override;
};

class LRemCmd : public Cmd {
 public:
  LRemCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag), count_(0) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LRemCmd(*this); }
 private:
  std::string key_;
  int64_t count_;
  std::string value_;
  void DoInitial() override;
};

class LSetCmd : public Cmd {
 public:
  LSetCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag), index_(0) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LSetCmd(*this); }
 private:
  std::string key_;
  int64_t index_;
  std::string value_;
  void DoInitial() override;
};

class LTrimCmd : public Cmd {
 public:
  LTrimCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag), start_(0), stop_(0) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new LTrimCmd(*this); }
 private:
  std::string key_;
  int64_t start_;
  int64_t stop_;
  void DoInitial() override;
};

class RPopCmd : public Cmd {
 public:
  RPopCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new RPopCmd(*this); }
 private:
  std::string key_;
  void DoInitial() override;
};

class RPopLPushCmd : public Cmd {
 public:
  RPopLPushCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override{
    std::vector<std::string> res;
    res.push_back(source_);
    return res;
  }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new RPopLPushCmd(*this); }
 private:
  std::string source_;
  std::string receiver_;
  void DoInitial() override;
};

class RPushCmd : public Cmd {
 public:
  RPushCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new RPushCmd(*this); }
 private:
  std::string key_;
  std::vector<std::string> values_;
  void DoInitial() override;
  void Clear() override {
    values_.clear();
  }
};

class RPushxCmd : public Cmd {
 public:
  RPushxCmd(fstring name, int arity, uint16_t flag)
      : Cmd(name, arity, flag) {}
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new RPushxCmd(*this); }
 private:
  std::string key_;
  std::string value_;
  void DoInitial() override;
};
#endif
