
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
extern bool JsonSmartBool(const json& js, const char* subname, bool Default);

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

    js["CPU info"]["CPU basic info"]["Architecture"] = cpu_map["Architecture"];
    js["CPU info"]["CPU basic info"]["CPU(s)"] = cpu_map["CPU(s)"];
    js["CPU info"]["CPU basic info"]["Thread(s) per core"] = cpu_map["Thread(s) per core"];
    js["CPU info"]["CPU basic info"]["Core(s) per socket"] = cpu_map["Core(s) per socket"];
    js["CPU info"]["CPU basic info"]["Socket(s)"] = cpu_map["Socket(s)"];
    js["CPU info"]["CPU basic info"]["Model name"] = cpu_map["Model name"];
    js["CPU info"]["CPU basic info"]["Virtualization"] = cpu_map["Virtualization"];

    js["CPU info"]["NUMA node(s)"]["total"] = cpu_map["NUMA node(s)"];
    js["CPU info"]["NUMA node(s)"]["node0"] = cpu_map["NUMA node0 CPU(s)"];
    js["CPU info"]["NUMA node(s)"]["node1"] = cpu_map["NUMA node1 CPU(s)"];
  
    js["CPU info"]["CPU Clock Speed"]["MHz"] = cpu_map["CPU MHz"];
    js["CPU info"]["CPU Clock Speed"]["max MHz"] = cpu_map["CPU max MHz"];
    js["CPU info"]["CPU Clock Speed"]["min MHz"] = cpu_map["CPU min MHz"];

    js["CPU info"]["cache"]["L1d"] = cpu_map["L1d cache"];
    js["CPU info"]["cache"]["L1i"] = cpu_map["L1i cache"];
    js["CPU info"]["cache"]["L2"] = cpu_map["L2 cache"];
    js["CPU info"]["cache"]["L3"] = cpu_map["L3 cache"];

    js["CPU info"]["Flags"] = cpu_map["Flags"];

    // if (dump_options["html"] == "0") {
    //   for (size_t i = 0; i < 6; ++i)
    //     js["Memory info"]["Mem"][mem[i]] = mem[i + 7];
    //   for (size_t i = 0; i < 3; ++i)
    //     js["Memory info"]["Swap"][mem[i]] = mem[i + 14];
    //   return JsonToString(js, dump_options);
    // }

    // if (dump_options["html"] == "1")
    json lmem, jmem;
    jmem[" "] = "Mem";
    jmem["total"] = mem[7];
    jmem["used"] = mem[8];
    jmem["free"] = mem[9];
    jmem["shared"] = mem[10];
    jmem["buff/cache"] = mem[11];
    jmem["available"] = mem[12];
    lmem.push_back(std::move(jmem));

    jmem[" "] = "Swap";
    jmem["total"] = mem[14];
    jmem["used"] = mem[15];
    jmem["free"] = mem[16];
    jmem["shared"] = "";
    jmem["buff/cache"] = "";
    jmem["available"] = "";
    lmem.push_back(std::move(jmem));

    auto &cols = lmem[0]["<htmltab:col>"];
    cols = json::array({
        " ",
        "total",
        "used",
        "free",
        "shared",
        "buff/cache",
        "available",
    });
    js["Memory Info"] = std::move(lmem);

    return JsonToString(js, dump_options);
  }
};

ROCKSDB_REG_DEFAULT_CONS(SystemInfoShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("SystemInfoShowPlugin");

} // namespace topling
