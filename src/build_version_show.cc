#include "utilities/json/json_plugin_factory.h"
#include "include/pika_conf.h"

// Date on which the code was compiled:
extern const char* pika_build_compile_date;
extern const char* pika_build_compile_time;

// this variable tells us about the git revision
extern const char* pika_build_git_sha;
extern const char* slash_build_git_sha;
extern const char* pink_build_git_sha;
extern const char* blackwidow_build_git_sha;


namespace topling {
using namespace ROCKSDB_NAMESPACE;


class BuildVersionsShowPlugin : public AnyPlugin {
public:
  void Update(const json&, const JsonPluginRepo&) {}
  std::string ToString(const json& dump_options, const JsonPluginRepo&) const {

    json js;

    char date_time_chs[20];
    tm date_time_tm;
    sprintf(date_time_chs, "%s %s", pika_build_compile_date, pika_build_compile_time);
    strptime(date_time_chs, "%b %d %Y %T", &date_time_tm);
    strftime(date_time_chs, 20, "%Y-%m-%d %T", &date_time_tm);
    js["build_compile_datetime"] = date_time_chs;

    js["build_git_sha"] = json::object({
      {"pika", strchr(pika_build_git_sha, ':') + 1},
      {"slash", strchr(slash_build_git_sha, ':') + 1},
      {"pink", strchr(pink_build_git_sha, ':') + 1},
      {"blackwidow", strchr(blackwidow_build_git_sha, ':') + 1},
    });

    return JsonToString(js, dump_options);
  }
};

static std::shared_ptr<AnyPlugin> JS_NewBuildVersionsShowPlugin(const json&,
                                                 const JsonPluginRepo&) {
  return std::make_shared<BuildVersionsShowPlugin>();
}
ROCKSDB_FACTORY_REG("BuildVersionsShowPlugin", JS_NewBuildVersionsShowPlugin);

struct BuildVersionsShowPlugin_Manip : PluginManipFunc<AnyPlugin> {
  void Update(AnyPlugin *,
              const json &js,
              const JsonPluginRepo &repo) const final {}
  std::string ToString(const AnyPlugin &reader,
                        const json &dump_options,
                        const JsonPluginRepo &repo) const final {
    if (auto rd = dynamic_cast<const BuildVersionsShowPlugin*>(&reader)) {
      return rd->ToString(dump_options, repo);
    }

    THROW_InvalidArgument("Unknow BuildVersionsShowPlugin");
  }
};
ROCKSDB_REG_PluginManip("BuildVersionsShowPlugin", BuildVersionsShowPlugin_Manip);


} // namespace topling