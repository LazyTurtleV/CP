#include "serv_func.h"

int main()
{	
	NTSTATUS status;
	FILE *dout_file;
	if (fopen_s(&dout_file, "Processes.txt", "w")) report_err(_T("Warning: opening out file"), NULL);

	_putts(_T("Do you want to get detailed or general info? d - detailed, g - general"));
	INFO_LEVEL_E info_level = getc(stdin) == 'd'? DEVELOPER: USER;


	if (status = print_sys_info(dout_file, info_level))
	{
		if ((unsigned)status > 0x1E7)
			_ftprintf(stderr, _T("Error: failed getting processes' info. Err code %X\n"), status);
		else report_err(_T("Error: getting processes info"), NULL);
	}
	else
		_putts(_T("Successed!"));

	fclose(dout_file);
}