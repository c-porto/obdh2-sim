#include <pthread.h>

#include <stdint.h>
#include <system/db.h>
#include <system/sys_log.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int read_cpu_times(unsigned long long *idle, unsigned long long *total)
{
	FILE *f = fopen("/proc/stat", "r");
	if (!f)
		return -1;

	unsigned long long user, nice, system, idle_v, iowait, irq, softirq,
		steal;
	int ret = fscanf(f, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
			 &user, &nice, &system, &idle_v, &iowait, &irq,
			 &softirq, &steal);
	fclose(f);

	if (ret < 4)
		return -1;

	*idle = idle_v + iowait;
	*total = user + nice + system + idle_v + iowait + irq + softirq + steal;
	return 0;
}

static double get_cpu_usage(void)
{
	unsigned long long idle1, total1;
	unsigned long long idle2, total2;

	if (read_cpu_times(&idle1, &total1) < 0)
		return -1.0;

	usleep(100000);

	if (read_cpu_times(&idle2, &total2) < 0)
		return -1.0;

	unsigned long long idle_delta = idle2 - idle1;
	unsigned long long total_delta = total2 - total1;

	if (total_delta == 0)
		return -1.0;

	return 100.0 * (1.0 - ((double)idle_delta / (double)total_delta));
}

struct mem_info {
	uint64_t total_kb;
	uint64_t free_kb;
	uint64_t available_kb;
};

static int read_meminfo(struct mem_info *m)
{
	FILE *f = fopen("/proc/meminfo", "r");

	if (!f)
		return -1;

	char line[1024U];
	memset(m, 0, sizeof(*m));

	while (fgets(line, sizeof(line), f)) {
		if (sscanf(line, "MemTotal: %llu kB", &m->total_kb) == 1)
			continue;
		if (sscanf(line, "MemFree: %llu kB", &m->free_kb) == 1)
			continue;
		if (sscanf(line, "MemAvailable: %llu kB", &m->available_kb) ==
		    1)
			continue;
	}

	fclose(f);

	return 0;
}

void *read_sys_info_thread(void *arg)
{
	(void)arg;

	struct timespec next = { 0 };
	struct mem_info info = { 0 };

	struct db_handle db = { 0 };
	const char *db_file = "/var/local/sys_info.sqlite3";

	if (create_tm_db(&db, db_file) < 0)
		sys_log_print_event_from_module(SYS_LOG_ERROR, "sys_info",
						"Failed to create DB!");

	clock_gettime(CLOCK_MONOTONIC, &next);

	for (;;) {
		next.tv_sec += 60;

		int ret = read_meminfo(&info);

		if (ret == 0) {
			tm_db_add_entry(&db, "sys_info", "mem_avail_kb",
					(double)info.available_kb);
			tm_db_add_entry(&db, "sys_info", "mem_free_kb",
					(double)info.free_kb);
			tm_db_add_entry(&db, "sys_info", "mem_total_kb",
					(double)info.total_kb);
		} else {
			sys_log_print_event_from_module(
				SYS_LOG_ERROR, "sys_info",
				"Failed to open /proc/meminfo");
		}

		double cpu_usage = get_cpu_usage();

		if (cpu_usage > 0)
			tm_db_add_entry(&db, "sys_info", "cpu_usage", cpu_usage);

		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
	};

	return NULL;
}
