#include "topling/side_plugin_factory.h"
#include "include/pika_conf.h"
#include <terark/num_to_str.hpp>

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

using namespace terark;

static std::string html_a_link(fstring git_repo, fstring git_hash) {
  terark::string_appender<> str;
  str|"<a href='https://github.com/"|git_repo|"/commit/"|git_hash|"'>"|git_hash|"</a>";
  return std::move(str);
}

class BuildVersionsShowPlugin : public AnyPlugin {
public:
  const char* Name() const override { return "BuildVersionsShowPlugin"; }
  void Update(const json&, const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {
    json js;
    char date_time_chs[22];
    tm date_time_tm;
    sprintf(date_time_chs, "%s %s", pika_build_compile_date, pika_build_compile_time);
    strptime(date_time_chs, "%b %d %Y %T", &date_time_tm);
    strftime(date_time_chs, 22, "%Y-%m-%d %T", &date_time_tm);
    js["build_compile_datetime"] = date_time_chs;
    auto pika = strchr(pika_build_git_sha, ':') + 1;
    auto pink = strchr(pink_build_git_sha, ':') + 1;
    auto slash = strchr(slash_build_git_sha, ':') + 1;
    auto blackwidow = strchr(blackwidow_build_git_sha, ':') + 1;
    if (JsonSmartBool(dump_options, "html", true)) {
      js["build_git_sha"] = json::object({
        {"pika", html_a_link("rockeet/pika", pika)},
        {"pink", html_a_link("rockeet/pink", pink)},
        {"slash", html_a_link("rockeet/slash", slash)},
        {"blackwidow", html_a_link("rockeet/blackwidow", blackwidow)},
      });
    }
    else {
      js["build_git_sha"] = json::object({
        {"pika", pika},
        {"pink", pink},
        {"slash", slash},
        {"blackwidow", blackwidow},
      });
    }
    return JsonToString(js, dump_options);
  }
};

ROCKSDB_REG_DEFAULT_CONS(BuildVersionsShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("BuildVersionsShowPlugin");


} // namespace topling
