// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_ZSET_H_
#define PIKA_ZSET_H_

#include "blackwidow/blackwidow.h"

#include "include/pika_command.h"
#include "include/pika_partition.h"

/*
 * zset
 */
class ZAddCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZAddCmd(*this); }
 private:
  std::string key_;
  std::vector<blackwidow::ScoreMember> score_members;
  void DoInitial() override;
};

class ZCardCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZCardCmd(*this); }
 private:
  std::string key_;
  void DoInitial() override;
};

class ZScanCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZScanCmd(*this); }
 private:
  std::string key_, pattern_ = "*";
  int64_t cursor_, count_ = 10;
  void DoInitial() override;
  void Clear() override {
    pattern_ = "*";
    count_ = 10;
  }
};

class ZIncrbyCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZIncrbyCmd(*this); }
 private:
  std::string key_, member_;
  double by_;
  void DoInitial() override;
};

class ZsetRangeParentCmd : public Cmd {
 public:
  using Cmd::Cmd;
 protected:
  std::string key_;
  int64_t start_ = 0, stop_ = 0;
  bool is_ws_ = false;
  void DoInitial() override;
  void Clear() override {
    is_ws_ = false;
  }
};

class ZRangeCmd : public ZsetRangeParentCmd {
 public:
  using ZsetRangeParentCmd::ZsetRangeParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRangeCmd(*this); }
 private:
  void DoInitial() override;
};

class ZRevrangeCmd : public ZsetRangeParentCmd {
 public:
  using ZsetRangeParentCmd::ZsetRangeParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRevrangeCmd(*this); }
 private:
  void DoInitial() override;
};

class ZsetRangebyscoreParentCmd : public Cmd {
 public:
  using Cmd::Cmd;
 protected:
  std::string key_;
  double min_score_, max_score_;
  bool left_close_ = true, right_close_ = true, with_scores_ = false;
  int64_t offset_ = 0, count_ = -1;
  void DoInitial() override;
  void Clear() override {
    left_close_ = right_close_ = true;
    with_scores_ = false;
    offset_ = 0;
    count_ = -1;
  }
};

class ZRangebyscoreCmd : public ZsetRangebyscoreParentCmd {
 public:
  using ZsetRangebyscoreParentCmd::ZsetRangebyscoreParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRangebyscoreCmd(*this); }
 private:
  void DoInitial() override;
};

class ZRevrangebyscoreCmd : public ZsetRangebyscoreParentCmd {
 public:
  using ZsetRangebyscoreParentCmd::ZsetRangebyscoreParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRevrangebyscoreCmd(*this); }
 private:
  void DoInitial() override;
};

class ZCountCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZCountCmd(*this); }
 private:
  std::string key_;
  double min_score_, max_score_;
  bool left_close_ = true, right_close_ = true;
  void DoInitial() override;
  void Clear() override {
    left_close_ = true;
    right_close_ = true;
  }
};

class ZRemCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRemCmd(*this); }
 private:
  std::string key_;
  std::vector<std::string> members_;
  void DoInitial() override;
};

class ZsetUIstoreParentCmd : public Cmd {
 public:
  using Cmd::Cmd;
 protected:
  std::string dest_key_;
  int64_t num_keys_;
  blackwidow::AGGREGATE aggregate_ = blackwidow::SUM;
  std::vector<std::string> keys_;
  std::vector<double> weights_;
  void DoInitial() override;
  void Clear() override {
    aggregate_ = blackwidow::SUM;
  }
};

class ZUnionstoreCmd : public ZsetUIstoreParentCmd {
 public:
  using ZsetUIstoreParentCmd::ZsetUIstoreParentCmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZUnionstoreCmd(*this); }
 private:
  void DoInitial() override;
};

class ZInterstoreCmd : public ZsetUIstoreParentCmd {
 public:
  using ZsetUIstoreParentCmd::ZsetUIstoreParentCmd;
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZInterstoreCmd(*this); }
 private:
  void DoInitial() override;
};

class ZsetRankParentCmd : public Cmd {
 public:
  using Cmd::Cmd;
 protected:
  std::string key_, member_;
  void DoInitial() override;
};

class ZRankCmd : public ZsetRankParentCmd {
 public:
  using ZsetRankParentCmd::ZsetRankParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRankCmd(*this); }
 private:
  void DoInitial() override;
};

class ZRevrankCmd : public ZsetRankParentCmd {
 public:
  using ZsetRankParentCmd::ZsetRankParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRevrankCmd(*this); }
 private:
  void DoInitial() override;
};

class ZScoreCmd : public ZsetRankParentCmd {
 public:
  using ZsetRankParentCmd::ZsetRankParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZScoreCmd(*this); }
 private:
  std::string key_, member_;
  void DoInitial() override;
};


class ZsetRangebylexParentCmd : public Cmd {
 public:
  using Cmd::Cmd;
 protected:
  std::string key_, min_member_, max_member_;
  bool left_close_ = true, right_close_ = true;
  int64_t offset_ = 0, count_ = -1;
  void DoInitial() override;
  void Clear() override {
    left_close_ = right_close_ = true;
    offset_ = 0;
    count_ = -1;
  }
};

class ZRangebylexCmd : public ZsetRangebylexParentCmd {
 public:
  using ZsetRangebylexParentCmd::ZsetRangebylexParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRangebylexCmd(*this); }
 private: 
  void DoInitial() override;
};

class ZRevrangebylexCmd : public ZsetRangebylexParentCmd {
 public:
  using ZsetRangebylexParentCmd::ZsetRangebylexParentCmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRevrangebylexCmd(*this); }
 private: 
  void DoInitial() override;
};

class ZLexcountCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZLexcountCmd(*this); }
 private:
  std::string key_, min_member_, max_member_;
  bool left_close_ = true, right_close_ = true;
  void DoInitial() override;
  void Clear() override {
    left_close_ = right_close_ = true;
  }
};

class ZRemrangebyrankCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRemrangebyrankCmd(*this); }
 private:
  std::string key_;
  int64_t start_rank_, stop_rank_;
  void DoInitial() override;
};

class ZRemrangebyscoreCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRemrangebyscoreCmd(*this); }
 private:
  std::string key_;
  double min_score_, max_score_;
  bool left_close_ = true, right_close_ = true;
  void DoInitial() override;
  void Clear() override {
    left_close_ =  right_close_ = true;
  }
};

class ZRemrangebylexCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { return {key_}; }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZRemrangebylexCmd(*this); }
 private:
  std::string key_;
  std::string min_member_, max_member_;
  bool left_close_ = true, right_close_ = true;
  void DoInitial() override;
  void Clear() override {
    left_close_ =  right_close_ = true;
  }
};

class ZPopmaxCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { 
    std::vector<std::string> res;
    res.emplace_back(key_);
    return res; 
  }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZPopmaxCmd(*this); }
 private:
  void DoInitial() override;
  std::string key_;
  int64_t count_;
};

class ZPopminCmd : public Cmd {
 public:
  using Cmd::Cmd;
  std::vector<std::string> current_key() const override { 
    std::vector<std::string> res;
    res.push_back(key_);
    return res; 
  }
  void Do(const std::shared_ptr<Partition>& partition = nullptr) override;
  void Split(const std::shared_ptr<Partition>&, const HintKeys&) override {}
  void Merge() override {}
  Cmd* Clone() override { return new ZPopminCmd(*this); }
 private:
  void DoInitial() override;
  std::string key_;
  int64_t count_;
};

#endif
