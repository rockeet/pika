#include "topling/side_plugin_factory.h"
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
  void Update(const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {

    json js;

    char date_time_chs[22];
    tm date_time_tm;
    sprintf(date_time_chs, "%s %s", pika_build_compile_date, pika_build_compile_time);
    strptime(date_time_chs, "%b %d %Y %T", &date_time_tm);
    strftime(date_time_chs, 22, "%Y-%m-%d %T", &date_time_tm);
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

ROCKSDB_REG_DEFAULT_CONS(BuildVersionsShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("BuildVersionsShowPlugin");


} // namespace topling
