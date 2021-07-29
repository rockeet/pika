
#include "topling/side_plugin_factory.h"
#include <terark/io/DataIO.hpp>
#include <terark/io/FileStream.hpp>
#include <terark/io/StreamBuffer.hpp>
#include <terark/util/process.hpp>
#include <terark/num_to_str.hpp>
#include <iostream>

namespace topling {
using namespace ROCKSDB_NAMESPACE;
using namespace terark;

extern std::string trim_space(std::string s);

std::string html_style = R"(
  <style>
    .sys_info {
      border: 1px solid rgb(196, 196, 196);
      border-radius: 5px;
      padding: 10px;
      text-align: center;
    }
  </style>
)";

class SystemInfoShowPlugin : public AnyPlugin {
public:
  void Update(const json &, const SidePluginRepo &) {}
  std::string ToString(const json &dump_options, const SidePluginRepo &) const {
    json js;

    ProcPipeStream m_file;
    LittleEndianDataInput<InputBuffer> m_reader;

    const size_t buf_size = 2048;
    m_reader.set_bufsize(buf_size);
    char buf[buf_size];
    std::stringstream ss;

    m_file.open("free", "r");
    m_reader.attach(&m_file);
    m_reader.read(buf, buf_size);
    m_file.close();

    ss.str(buf);
    std::string mem[17];
    for (size_t i = 0; i < 17; ++i)
      ss >> mem[i];   
    for (size_t i = 0; i < 6; ++i)
      js["Memory"]["Mem"][mem[i]] = mem[i + 7];
    for (size_t i = 0; i < 3; ++i)
      js["Memory"]["Swap"][mem[i]] = mem[i + 14];

    m_file.open("env LC_ALL=C lscpu", "r");
    m_reader.attach(&m_file);
    m_reader.read(buf, buf_size);
    m_file.close();

    ss.str(buf);
    std::string cpu_info;
    std::map<std::string, std::string> cpu_map;
    for(size_t i = 0; i < 26 && getline(ss, cpu_info); ++i) {
      size_t colon_pos = cpu_info.find(':');
      cpu_map[cpu_info.substr(0, colon_pos)] = cpu_info.substr(colon_pos + 1);
    }

    // return JsonToString(js, dump_options);

    ////////////////
    
    string_appender<std::string> str;
    str.reserve(5*1024);

    str|"<table>";
    str|"<caption>" "Memory Info" "</caption>";
    str|"<tr>" "<th class=\"sys_info\">" ""     "</th>";
    str|"<th class=\"sys_info\">" "total"       "</th>";
    str|"<th class=\"sys_info\">" "used"        "</th>";
    str|"<th class=\"sys_info\">" "free"        "</th>";
    str|"<th class=\"sys_info\">" "shared"      "</th>";
    str|"<th class=\"sys_info\">" "buff/cache"  "</th>";
    str|"<th class=\"sys_info\">" "available"   "</th>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Mem"  "</th>";
    str|"<td class=\"sys_info\">"|mem[ 7]|"</td>";
    str|"<td class=\"sys_info\">"|mem[ 8]|"</td>";
    str|"<td class=\"sys_info\">"|mem[ 9]|"</td>";
    str|"<td class=\"sys_info\">"|mem[10]|"</td>";
    str|"<td class=\"sys_info\">"|mem[11]|"</td>";
    str|"<td class=\"sys_info\">"|mem[12]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Swap" "</th>";
    str|"<td class=\"sys_info\">"|mem[14]|"</td>";
    str|"<td class=\"sys_info\">"|mem[15]|"</td>";
    str|"<td class=\"sys_info\">"|mem[16]|"</td>";
    str|"<td class=\"sys_info\">" "" "</td>";
    str|"<td class=\"sys_info\">" "" "</td>";
    str|"<td class=\"sys_info\">" "" "</td>" "</tr>";
    str|"</table>";

    str|"<table>";
    str|"<caption>" "CPU Info" "</caption>";
    str|"<tr>" "<th class=\"sys_info\">" "Architecture" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Architecture"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "CPU(s)" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["CPU(s)"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Thread(s) per core" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Thread(s) per core"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Core(s) per socket" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Core(s) per socket"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Socket(s)" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Socket(s)"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "NUMA node(s)" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["NUMA node(s)"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "CPU family" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["CPU family"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Model" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Model"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Model name" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Model name"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Stepping" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Stepping"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "CPU MHz" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["CPU MHz"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "CPU max MHz" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["CPU max MHz"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "CPU min MHz" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["CPU min MHz"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "BogoMIPS" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["BogoMIPS"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Virtualization" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Virtualization"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "L1d cache" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["L1d cache"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "L1i cache" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["L1i cache"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "L2 cache" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["L2 cache"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "L3 cache" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["L3 cache"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "NUMA node0 CPU(s)" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["NUMA node0 CPU(s)"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "NUMA node1 CPU(s)" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["NUMA node1 CPU(s)"]|"</td>" "</tr>";
    str|"<tr>" "<th class=\"sys_info\">" "Flags" "</th>";
    str|"<td class=\"sys_info\">"|cpu_map["Flags"]|"</td>" "</tr>";
    str|"</table>";




    return std::move(str);
  }
};

ROCKSDB_REG_DEFAULT_CONS(SystemInfoShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("SystemInfoShowPlugin");

} // namespace topling
