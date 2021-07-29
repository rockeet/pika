
#include "topling/side_plugin_factory.h"
#include <terark/io/DataIO.hpp>
#include <terark/io/FileStream.hpp>
#include <terark/io/StreamBuffer.hpp>
#include <terark/util/process.hpp>

namespace topling {
using namespace ROCKSDB_NAMESPACE;
using namespace terark;

extern std::string trim_space(std::string s);

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
    std::string cpu;
    for(size_t i = 0; i < 26 && getline(ss, cpu); ++i) {
      size_t colon_pos = cpu.find(':');
      js["CPU"][cpu.substr(0, colon_pos)] = cpu.substr(colon_pos + 1);
    }

    return JsonToString(js, dump_options);
  }
};

ROCKSDB_REG_DEFAULT_CONS(SystemInfoShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("SystemInfoShowPlugin");

} // namespace topling
