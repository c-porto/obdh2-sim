#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <strings.h>
#include <system/sys_log.h>
#include <time.h>
#include <pthread.h>

static const char *log_level_strings[] = { "E", "W", "I" };

static const char *log_file = "stdout";

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

int sys_log_set_log_file(const char *filename)
{
	log_file = filename;
	return 0;
}

int sys_log_print_event_from_module(int level, const char *module,
				    const char *format, ...)
{
	bool is_stdout = false;
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	pthread_mutex_lock(&log_mutex);

	FILE *f = NULL;

	if (strcasecmp("stdout", log_file) == 0) {
		f = stdout;
		is_stdout = true;
	} else {
		f = fopen(log_file, "a");
	}

	if (!f) {
		perror("sys_log: fopen");
		return -1;
	}

	fprintf(f, "%lu.%03lu %s %s: ", ts.tv_sec, ts.tv_nsec / 1000U,
		log_level_strings[level], module);

	va_list args;
	va_start(args, format);
	vfprintf(f, format, args);
	va_end(args);

	fprintf(f, "\n");
	fflush(f);

	if (!is_stdout)
		fclose(f);

	pthread_mutex_unlock(&log_mutex);

	return 0;
}
