#include "topling/side_plugin_factory.h"
#include "include/pika_conf.h"
#include "pink/include/pika_cmd_histogram_manager.h"

extern PikaConf* g_pika_conf;
extern PikaCmdHistogramManager* g_pika_cmd_histogram_manager;

namespace topling {
using namespace ROCKSDB_NAMESPACE;

class PikaAdapterPrometheusPlugin : public AnyPlugin {
public:
  void Update(const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {
    return g_pika_cmd_histogram_manager->get_metric();
  }
};

ROCKSDB_REG_DEFAULT_CONS(PikaAdapterPrometheusPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("PikaAdapterPrometheusPlugin");


} // namespace topling
