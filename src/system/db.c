#include <system/db.h>
#include <system/sys_log.h>
#include <stddef.h>

int create_tm_db(struct db_handle *db, const char *db_file)
{
	if (!db && !db_file)
		return -1;

	db->db_file = db_file;

	int err = sqlite3_open(db_file, &db->handle);

	if (err) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "db",
			"Failed to create/open database!! Error: %s",
			sqlite3_errmsg(db->handle));

		sqlite3_close(db->handle);

		return -1;
	}

	const char *tm_table_st =
		"CREATE TABLE IF NOT EXISTS telemetry (id INTEGER PRIMARY KEY AUTOINCREMENT, subsystem TEXT NOT NULL, field TEXT NOT NULL, value REAL NOT NULL, timestamp TEXT DEFAULT CURRENT_TIMESTAMP)";

	char *errMsg = NULL;

	err = sqlite3_exec(db->handle, tm_table_st, NULL, NULL, &errMsg);

	if (err != SQLITE_OK) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "db",
			"Failed to create telemetry table!");
		sqlite3_free(errMsg);
		return -1;
	}

	sys_log_print_event_from_module(SYS_LOG_INFO, "db",
					"Sucessfully created database [%s]",
					db->db_file);

	return 0;
}

int tm_db_add_entry(struct db_handle *db, const char *subsys,
		    const char *field_name, const double value)
{
	if (!db->handle) {
		sys_log_print_event_from_module(SYS_LOG_ERROR, "db",
						"Database handle is NULL!");
		return -1;
	}

	const char *insert_st =
		"INSERT INTO telemetry (subsystem, field, value) VALUES (?, ?, ?);";

	sqlite3_stmt *stmt = NULL;

	int err = sqlite3_prepare_v2(db->handle, insert_st, -1, &stmt, NULL);

	if (err) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "db",
			"Failed to prepare statement!! Error: %s",
			sqlite3_errmsg(db->handle));

		return -1;
	}

	sqlite3_bind_text(stmt, 1, subsys, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, field_name, -1, SQLITE_STATIC);
	sqlite3_bind_double(stmt, 3, value);

	err = sqlite3_step(stmt);

	if (err != SQLITE_DONE) {
		sys_log_print_event_from_module(
			SYS_LOG_ERROR, "db",
			"Failed to execute statament!! Error: %s",
			sqlite3_errmsg(db->handle));
		sqlite3_finalize(stmt);
		return -1;
	}

	sqlite3_finalize(stmt);

	return 0;
}
