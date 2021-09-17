#include "topling/side_plugin_factory.h"
#include "include/pika_conf.h"
#include "pink/include/pika_cmd_time_histogram.h"
#include "pika_data_length_histogram.h"

extern PikaConf* g_pika_conf;
extern time_histogram::PikaCmdRunTimeHistogram* g_pika_cmd_run_time_histogram;
extern length_histogram::CmdDataLengthHistogram* g_pika_cmd_data_length_histogram;

namespace topling {
using namespace ROCKSDB_NAMESPACE;

class CmdRunTimeHistogramPlugin : public AnyPlugin {
public:
  const char* Name() const override { return "CmdRunTimeHistogramPlugin"; }
  void Update(const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {
    return g_pika_cmd_run_time_histogram->GetTimeMetric();
  }
};

ROCKSDB_REG_DEFAULT_CONS(CmdRunTimeHistogramPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("CmdRunTimeHistogramPlugin");

class CmdDataLengthHistogramPlugin : public AnyPlugin {
public:
  const char* Name() const override { return "CmdDataLengthHistogramPlugin"; }
  void Update(const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {
    bool metric = JsonSmartBool(dump_options, "metric", true);
    if (metric) {
      return g_pika_cmd_data_length_histogram->GetLengthMetric();
    } else {
      return g_pika_cmd_data_length_histogram->GetLengthHtml();
    }
  }
};

ROCKSDB_REG_DEFAULT_CONS(CmdDataLengthHistogramPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("CmdDataLengthHistogramPlugin");

} // namespace topling
