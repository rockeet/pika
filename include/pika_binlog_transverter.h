// Copyright (c) 2018-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef PIKA_BINLOG_TRANSVERTER_H_
#define PIKA_BINLOG_TRANSVERTER_H_

#include <vector>
#include <stdint.h>
#include <iostream>
#include <glog/logging.h>


/*
 * ***********************************************Type First Binlog Item Format***********************************************
 * | <Type> | <Create Time> | <Term Id> | <Binlog Logic Id> | <File Num> | <Offset> | <Content Length> |      <Content>     |
 *  2 Bytes      4 Bytes      4 Bytes       8 Bytes            4 Bytes     8 Bytes        4 Bytes       content length Bytes
 *
 */
#define BINLOG_ENCODE_LEN 34

enum BinlogType {
  TypeFirst = 1,
};


const int BINLOG_ITEM_HEADER_SIZE = 34;
const int PADDING_BINLOG_PROTOCOL_SIZE = 22;
const int SPACE_STROE_PARAMETER_LENGTH = 5;

class BinlogItem {
  public:
    BinlogItem() :
        exec_time_(0),
        term_id_(0),
        logic_id_(0),
        filenum_(0),
        offset_(0),
        content_("") {}

    friend class PikaBinlogTransverter;

    uint32_t exec_time()   const;
    uint32_t term_id()   const;
    uint64_t logic_id()    const;
    uint32_t filenum()     const;
    uint64_t offset()      const;
    std::string content()  const;
    std::string ToString() const;

    void set_exec_time(uint32_t exec_time);
    void set_term_id(uint32_t term_id);
    void set_logic_id(uint64_t logic_id);
    void set_filenum(uint32_t filenum);
    void set_offset(uint64_t offset);

  private:
    uint32_t exec_time_;
    uint32_t term_id_;
    uint64_t logic_id_;
    uint32_t filenum_;
    uint64_t offset_;
    std::string content_;
    std::vector<std::string> extends_;
};

class PikaBinlogTransverter{
  public:
    PikaBinlogTransverter() {}
    static std::string BinlogEncode(BinlogType type,
                                    uint32_t exec_time,
                                    uint32_t term_id,
                                    uint64_t logic_id,
                                    uint32_t filenum,
                                    uint64_t offset,
                                    const std::string& content,
                                    const std::vector<std::string>& extends);

    static bool BinlogDecode(BinlogType type,
                             const std::string& binlog,
                             BinlogItem* binlog_item);

    static std::string ConstructPaddingBinlog(BinlogType type, uint32_t size);

    static bool BinlogItemWithoutContentDecode(BinlogType type,
                                               const std::string& binlog,
                                               BinlogItem* binlog_item);
};

#endif
