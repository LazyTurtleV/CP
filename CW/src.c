#include "serv_func.h"

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev_inst, PTCHAR lpCmd_line, int nCmd_show)
{		
	HWND hMain_window, hTree_container, hTree_view;

	PTCHAR ret_msg = init_app(&hMain_window, &hTree_container, &hTree_view, hInst);
	if (_tcsicmp(ret_msg, _T("")))
	{
		report_err(ret_msg, NULL, NULL);
		return 1;
	}	

	GetDlgItem(hTree_view, TREE_VIEW_ID);

	ShowWindow(hMain_window, SW_NORMAL);
	UpdateWindow(hMain_window);

	NTSTATUS status;
	if (status = print_sys_info(stderr, hTree_container, hTree_view))
	{
		report_err(_T("print_sys_info"), hMain_window, status);
		return status;
	}

	MSG msg = { 0 };
	int iGetOk = 0;
	while ((iGetOk = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (iGetOk == -1) return 3;		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}