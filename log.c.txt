/* Diego Aron Poplade - GRR20101352
 * Gustavo Toshi Komura - GRR20102342
 * Prof. Elias P. Duarte Jr.
   Servidor TCP Iterativo    */

#include <stdio.h>
#include <time.h>
#include "log.h"

void log_printf(char* str)
{
		time_t day;
		struct tm* day_tm;

		day = time(NULL);
		day_tm = localtime(&day);

		printf("[%d:%d:%d %d-%d-%d] %s\n", day_tm->tm_hour, day_tm->tm_min, day_tm->tm_sec, day_tm->tm_mday, day_tm->tm_mon + 1, day_tm->tm_year + 1900, str);
}
