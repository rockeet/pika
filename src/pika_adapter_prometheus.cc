#include "topling/side_plugin_factory.h"
#include "include/pika_conf.h"
#include "pink/include/pika_run_cmd_histogram.h"
#include "db_read_write_histogram.h"

extern PikaConf* g_pika_conf;
extern cmd_run_histogram::PikaCmdRunHistogram* g_pika_run_cmd_histogram;
extern db_rw_histogram::DbReadWriteHistogram* g_db_read_write_histogram;

namespace topling {
using namespace ROCKSDB_NAMESPACE;

class CmdRunHistogramPlugin : public AnyPlugin {
public:
  const char* Name() const override { return "CmdRunHistogramPlugin"; }
  void Update(const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {
    return g_pika_run_cmd_histogram->get_metric();
  }
};

ROCKSDB_REG_DEFAULT_CONS(CmdRunHistogramPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("CmdRunHistogramPlugin");

class CmdDataStatisticsPrometheusPlugin : public AnyPlugin {
public:
  const char* Name() const override { return "CmdDataStatisticsPrometheusPlugin"; }
  void Update(const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {
    bool web = JsonSmartBool(dump_options, "web", false);
    if (web) {
      return g_db_read_write_histogram->get_html();
    } else {
      return g_db_read_write_histogram->get_metric();
    }
  }
};

ROCKSDB_REG_DEFAULT_CONS(CmdDataStatisticsPrometheusPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("CmdDataStatisticsPrometheusPlugin");

} // namespace topling
