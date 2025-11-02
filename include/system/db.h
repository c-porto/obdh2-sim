#ifndef DB_H_
#define DB_H_

#include <sqlite3.h>

#define STRINGZ(x) #x

struct db_handle {
	sqlite3 *handle;
	const char *db_file;
};

int create_tm_db(struct db_handle *db, const char *db_file);

int tm_db_add_entry(struct db_handle *db, const char *subsys, const char *field_name, const double value);

#endif
