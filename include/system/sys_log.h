#ifndef SYS_LOG_H_
#define SYS_LOG_H_

enum sys_log_level {
	SYS_LOG_ERROR,
	SYS_LOG_WARNING,
	SYS_LOG_INFO,
};

int sys_log_set_log_file(const char *filename);

int sys_log_print_event_from_module(int level, const char *module, const char *format, ...);

int sys_log_print_msg(const char *format, ...);

#endif
