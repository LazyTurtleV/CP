#include "serv_func.h"

void report_err(PTCHAR msg, HWND hParent, ULONG err_code)
{
	if(!err_code) err_code = GetLastError();

	TCHAR full_err_msg[1024];
	
	if (err_code <= 0x1E7)
	{
		LPWSTR err_msg = NULL;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |						//dwFlags
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,													//Source
			err_code,												//error code
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),				//language id
			(LPWSTR)&err_msg,										//lp_buffer
			0,														//nSize
			NULL													//arguments
		);

		_sntprintf_s(full_err_msg, 1024, 1024, _T("%s failed, err_code: 0x%X\n%ls"), msg, err_code, err_msg);
	}
	else
		_sntprintf_s(full_err_msg, 1024, 1024, _T("%s failed, err_code: 0x%X"), msg, err_code);

	MessageBox(hParent, full_err_msg, _T("Error"), MB_ICONERROR | MB_OK);
}

LRESULT CALLBACK main_win_handler(HWND hWin, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_NOTIFY:
		if (wParam == TREE_VIEW_ID)
		{
			switch (((LPNMHDR)lParam)->code)
			{
			case NM_RCLICK: {
				HWND hList_view = GetDlgItem(hWin, TREE_VIEW_ID);

				HTREEITEM hClicked_item = TreeView_GetSelection(hList_view);

				TVITEM tv_item = {
					.mask = TVIF_TEXT | TVIF_STATE | TVIF_HANDLE | TVIF_PARAM,
					.pszText = malloc(1024),
					.cchTextMax = 1024,
					.hItem = hClicked_item
				};

				TreeView_GetItem(hList_view, &tv_item);

				//find process from PID
				TCHAR proc_info_str[1024] = RESERVED_PROC_NAME_STR;
				for (PSYSTEM_PROCESS_INFORMATION lpSys_proc_info = pSys_proc_info; lpSys_proc_info->NextEntryOffset != NULL; lpSys_proc_info = (PBYTE)lpSys_proc_info + lpSys_proc_info->NextEntryOffset)
				{
					if (lpSys_proc_info->UniqueProcessId == tv_item.lParam)
					{
						_sntprintf_s(
							proc_info_str,
							1024,
							1024,
							_T("PID: %lu, process name: %s, threads count: %lu \nhandles uses: %lu, session ID: %lu\nPeak virtual mem size: %lu MB\nVirtual mem size: %lu MB\nWorking set size: %lu MB"),
							lpSys_proc_info->UniqueProcessId,
							lpSys_proc_info->ImageName.Buffer,
							lpSys_proc_info->NumberOfThreads,
							lpSys_proc_info->HandleCount,
							lpSys_proc_info->SessionId,
							BYTES_TO_MB(lpSys_proc_info->PeakVirtualSize),
							BYTES_TO_MB(lpSys_proc_info->VirtualSize),
							BYTES_TO_MB(lpSys_proc_info->WorkingSetSize)
						);
					}
				}

				if(_tcscmp(proc_info_str, RESERVED_PROC_NAME_STR))
					MessageBox(NULL, proc_info_str, _T("Process information"), MB_OK);

				break;
			}
			}
		}

		break;
	}

	return DefWindowProc(hWin, msg, wParam, lParam);
}