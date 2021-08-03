
#include "topling/side_plugin_factory.h"
#include <terark/io/DataIO.hpp>
#include <terark/io/FileStream.hpp>
#include <terark/io/StreamBuffer.hpp>
#include <terark/util/process.hpp>
#include <terark/num_to_str.hpp>

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

    ProcPipeStream file;
    LittleEndianDataInput<InputBuffer> reader;

    const size_t buf_size = 4096;
    reader.set_bufsize(buf_size);
    char buf[buf_size];

    // 读取并处理内存信息
    file.open("free", "r");
    reader.attach(&file);
    reader.read(buf, buf_size);
    file.close();

    std::stringstream ss;
    ss.str(buf);
    std::string mem[17];
    for (size_t i = 0; i < 17; ++i)
      ss >> mem[i];

    json lmem, jmem, jswap;
    jmem = json::object({
      {"", "Mem"},
      {"total", mem[7]},
      {"used", mem[8]},
      {"free", mem[9]},
      {"shared", mem[10]},
      {"buff/cache", mem[11]},
      {"available", mem[12]},
    });
    jswap = json::object({
      {"", "Swap"},
      {"total", mem[14]},
      {"used", mem[15]},
      {"free", mem[16]},
      {"shared", ""},
      {"buff/cache", ""},
      {"available", ""},
    });
    lmem.push_back(std::move(jmem));
    lmem.push_back(std::move(jswap));

    auto &cols = lmem[0]["<htmltab:col>"];
    cols = json::array({"", "total", "used", "free", "shared", "buff/cache", "available",});
    js["Memory Info"] = std::move(lmem);

    // 读取并处理 CPU 信息
    file.open("env LC_ALL=C lscpu -J", "r");
    reader.attach(&file);
    reader.read(buf, buf_size);
    file.close();

    json temp_js = json::parse(buf);
    std::set<std::string> cpu_basic{"Architecture", "CPU(s)", "Thread(s) per core", "Core(s) per socket", "Socket(s)", "Model name", "Virtualization"};
    std::set<std::string> cpu_numa{"NUMA node(s)", "NUMA node0 CPU(s)", "NUMA node1 CPU(s)"};
    std::set<std::string> cpu_clockspeed{"CPU MHz", "CPU max MHz", "CPU min MHz"};
    std::set<std::string> cpu_cache{"L1d cache", "L1i cache", "L2 cache", "L3 cache"};
    std::set<std::string> cpu_flags{"Flags"};
    for (auto kv: temp_js["lscpu"]) {
      std::string k = kv["field"];
      k.pop_back();
      auto& js_cpu = js["CPU Info"];
      if (cpu_basic.count(k))
        js_cpu["Basic"][k] = kv["data"];
      else if (cpu_numa.count(k))
        js_cpu["NUMA"][k] = kv["data"];
      else if (cpu_clockspeed.count(k))
        js_cpu["Clock Speed"][k] = kv["data"];
      else if (cpu_cache.count(k))
        js_cpu["Cache"][k] = kv["data"];
      else if (cpu_flags.count(k))
        js_cpu[k] = kv["data"];        
    }

    return JsonToString(js, dump_options);
  }
};

ROCKSDB_REG_DEFAULT_CONS(SystemInfoShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("SystemInfoShowPlugin");

} // namespace topling
