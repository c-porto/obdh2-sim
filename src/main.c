#include <stdint.h>

#include <stdio.h>
#include <system/sys_log.h>

int main(void)
{
	uint16_t tmp = UINT16_MAX - 1U;
	sys_log_print_event_from_module(SYS_LOG_INFO, "MAIN", "Teste [%u]",
					tmp);
	sys_log_print_event_from_module(SYS_LOG_WARNING, "MAIN", "Teste [%i]",
					(int16_t)tmp);
	sys_log_print_event_from_module(SYS_LOG_ERROR, "MAIN", "Teste [%i]",
					(int)tmp);
}
