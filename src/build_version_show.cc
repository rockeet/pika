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

const char* LocateBuildGitSha(const char* str) {
  for (size_t i = 0; str[i] != '\0'; ++i) {
    if (str[i] == ':') {
      return str + i + 1;
    }
  }
  return "format error!";
}

class BuildVersionsShowPlugin : public AnyPlugin {
public:
  void Update(const json&, const JsonPluginRepo&) {}
  std::string ToString(const json& dump_options, const JsonPluginRepo&) const {
    json js;
    std::string datatime = pika_build_compile_date;
    datatime = datatime + " " + pika_build_compile_time;
    js["build_compile_datetime"] = datatime;
    js["build_git_sha"] = json::object({
      {"pika", LocateBuildGitSha(pika_build_git_sha)},
      {"slash", LocateBuildGitSha(slash_build_git_sha)},
      {"pink", LocateBuildGitSha(pink_build_git_sha)},
      {"blackwidow", LocateBuildGitSha(blackwidow_build_git_sha)},
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