#include <fstream>
#include "topling/side_plugin_factory.h"

namespace topling {
using namespace ROCKSDB_NAMESPACE;

extern std::string trim_space(std::string s);

class MemoryInfoShowPlugin : public AnyPlugin {
public:
  void Update(const json &, const SidePluginRepo &) {}
  std::string ToString(const json &dump_options, const SidePluginRepo &) const {
    json js;

    std::ifstream in_file;
    in_file.open("/proc/meminfo");
    std::string item;
    for (size_t i = 0; getline(in_file, item); ++i) {
      size_t colon_pos = item.find(':');
      js[trim_space(item.substr(0, colon_pos))] = trim_space(item.substr(colon_pos + 1, item.size() - 1));
    }
    in_file.close();

    return JsonToString(js, dump_options);
  }
};

ROCKSDB_REG_DEFAULT_CONS(MemoryInfoShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("MemoryInfoShowPlugin");

} // namespace topling
