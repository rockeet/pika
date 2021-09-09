#include "topling/side_plugin_factory.h"
#include "include/pika_conf.h"

extern PikaConf* g_pika_conf;

namespace topling {
using namespace ROCKSDB_NAMESPACE;

class PikaConfShowPlugin : public AnyPlugin {
public:
  const char* Name() const override { return "PikaConfShowPlugin"; }
  void Update(const json&, const SidePluginRepo&) {}
  std::string ToString(const json& dump_options, const SidePluginRepo&) const {
    json js;
    js["sideplugin_conf"] = g_pika_conf->sideplugin_conf();
    js["port"] = g_pika_conf->port();
    js["slaveof"] = g_pika_conf->slaveof();
    js["slave_priority"] = g_pika_conf->slave_priority();
    js["write_binlog"] = g_pika_conf->write_binlog();
    js["thread_num"] = g_pika_conf->thread_num();
    js["thread_pool_size"] = g_pika_conf->thread_pool_size();
    js["sync_thread_num"] = g_pika_conf->sync_thread_num();
    js["log_path"] = g_pika_conf->log_path();
    js["db_path"] = g_pika_conf->db_path();
    js["db_sync_path"] = g_pika_conf->db_sync_path();
    js["db_sync_speed"] = g_pika_conf->db_sync_speed();
    js["compact_cron"] = g_pika_conf->compact_cron();
    js["compact_interval"] = g_pika_conf->compact_interval();
    js["write_buffer_size"] = g_pika_conf->write_buffer_size();
    js["arena_block_size"] = g_pika_conf->arena_block_size();
    js["max_write_buffer_size"] = g_pika_conf->max_write_buffer_size();
    js["max_write_buffer_number"] = g_pika_conf->max_write_buffer_number();
    js["max_client_response_size"] = g_pika_conf->max_client_response_size();
    js["timeout"] = g_pika_conf->timeout();
    js["server_id"] = g_pika_conf->server_id();
    js["requirepass"] = g_pika_conf->requirepass();
    js["masterauth"] = g_pika_conf->masterauth();
    js["bgsave_path"] = g_pika_conf->bgsave_path();
    js["expire_dump_days"] = g_pika_conf->expire_dump_days();
    js["bgsave_prefix"] = g_pika_conf->bgsave_prefix();
    js["userpass"] = g_pika_conf->userpass();
    js["suser_blacklist"] = g_pika_conf->suser_blacklist();

    // g_pika_conf->vuser_blacklist() // std::vector<std::string>
    if (g_pika_conf->vuser_blacklist().empty()) {
      js["vuser_blacklist"] = "";
    } else {
      for (size_t i = 0; i < g_pika_conf->vuser_blacklist().size(); ++i) {
        js["vuser_blacklist"].push_back(g_pika_conf->vuser_blacklist()[i]);
      }
    }

    js["classic_mode"] = g_pika_conf->classic_mode();
    js["databases"] = g_pika_conf->databases();
    js["default_slot_num"] = g_pika_conf->default_slot_num();

    // g_pika_conf->table_structs();  // std::vector<TableStruct>
    if (g_pika_conf->table_structs().empty()) {
      js["table_structs"] = "";
    } else {
      for (size_t i = 0; i < g_pika_conf->table_structs().size(); ++i) {
        const TableStruct& temp = g_pika_conf->table_structs()[i];
        js["table_structs"].push_back(json::object({
          {"table_name", temp.table_name},
          {"partition_num", temp.partition_num},
          {"partition_ids", temp.partition_ids},
        }));
      }
    }

    js["default_table"] = g_pika_conf->default_table();
    js["compression"] = g_pika_conf->compression();
    js["target_file_size_base"] = g_pika_conf->target_file_size_base();
    js["max_cache_statistic_keys"] = g_pika_conf->max_cache_statistic_keys();
    js["small_compaction_threshold"] = g_pika_conf->small_compaction_threshold();
    js["max_background_flushes"] = g_pika_conf->max_background_flushes();
    js["max_background_compactions"] = g_pika_conf->max_background_compactions();
    js["max_cache_files"] = g_pika_conf->max_cache_files();
    js["max_bytes_for_level_multiplier"] = g_pika_conf->max_bytes_for_level_multiplier();
    js["block_size"] = g_pika_conf->block_size();
    js["block_cache"] = g_pika_conf->block_cache();
    js["share_block_cache"] = g_pika_conf->share_block_cache();
    js["cache_index_and_filter_blocks"] = g_pika_conf->cache_index_and_filter_blocks();
    js["optimize_filters_for_hits"] = g_pika_conf->optimize_filters_for_hits();
    js["level_compaction_dynamic_level_bytes"] = g_pika_conf->level_compaction_dynamic_level_bytes();
    js["expire_logs_nums"] = g_pika_conf->expire_logs_nums();
    js["expire_logs_days"] = g_pika_conf->expire_logs_days();
    js["conf_path"] = g_pika_conf->conf_path();
    js["slave_read_only"] = g_pika_conf->slave_read_only();
    js["maxclients"] = g_pika_conf->maxclients();
    js["root_connection_num"] = g_pika_conf->root_connection_num();
    js["slowlog_write_errorlog"] = g_pika_conf->slowlog_write_errorlog();
    js["slowlog_slower_than"] = g_pika_conf->slowlog_slower_than();
    js["slowlog_max_len"] = g_pika_conf->slowlog_max_len();
    js["network_interface"] = g_pika_conf->network_interface();
    js["sync_window_size"] = g_pika_conf->sync_window_size();
    js["max_conn_rbuf_size"] = g_pika_conf->max_conn_rbuf_size();
    js["consensus_level"] = g_pika_conf->consensus_level();
    js["replication_num"] = g_pika_conf->replication_num();

    js["daemonize"] = g_pika_conf->daemonize();
    js["pidfile"] = g_pika_conf->pidfile();
    js["binlog_file_size"] = g_pika_conf->binlog_file_size();

    // PikaMeta *
    // It's private and there are no method to get the pointer,
    // if you want show it, you need modify its class modifier to public.
    // js["local_meta"] = g_pika_conf->local_meta();

    return JsonToString(js, dump_options);
  }
};

ROCKSDB_REG_DEFAULT_CONS(PikaConfShowPlugin, AnyPlugin);
ROCKSDB_REG_AnyPluginManip("PikaConfShowPlugin");


} // namespace topling
