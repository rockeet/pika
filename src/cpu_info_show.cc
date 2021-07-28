#include "topling/side_plugin_factory.h"
#include <fstream>

namespace topling {
using namespace ROCKSDB_NAMESPACE;

std::string trim_space(std::string s) {
  s.erase(0, s.find_first_not_of("\t "));
  s.erase(s.find_last_not_of("\t ") + 1);
  return s;
}

class CpuInfoShowPlugin : public AnyPlugin {
public:
  void Update(const json &, const SidePluginRepo &) {}
  std::string ToString(const json &dump_options, const SidePluginRepo &) const {
    json js;

    std::ifstream in_file;
    in_file.open("/proc/cpuinfo");
    std::string item;
    int processor_n = -1;
    for (size_t i = 0; getline(in_file, item); ++i) {
      if (i % 27 == 0) {
        ++processor_n;
        continue;
      } else if (item.empty()) {
        continue;
      }
      size_t colon_pos = item.find(':');
      js["processor " + std::to_string(processor_n)]
        [trim_space(item.substr(0, colon_pos))] =
            trim_space(item.substr(colon_pos + 1, item.size() - 1));
    }

    return JsonToString(js, dump_options);
  }
};

ROCKSDB_REG_DEFAULT_CONS(CpuInfoShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("CpuInfoShowPlugin");

} // namespace topling