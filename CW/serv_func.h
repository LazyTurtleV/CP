#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winternl.h>
#include <tchar.h>

typedef enum INFO_LEVEL_E
{
	USER,
	DEVELOPER
} INFO_LEVEL_E;

void report_err(PTCHAR msg, FILE *stream_d);
NTSTATUS print_sys_info(FILE *stream_d, INFO_LEVEL_E info_level);
