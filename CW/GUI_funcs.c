#include "serv_func.h"

ATOM register_class(PTCHAR class_name_str, HINSTANCE hInst, PWNDCLASSEX win_class, HBRUSH brush)
{
	WNDCLASSEX win_class_tmp = {
		.lpszClassName = class_name_str,
		.hInstance = hInst,
		.hbrBackground = brush,
		.hIcon = LoadIcon(0, IDI_APPLICATION),
		.hCursor = LoadCursor(0, IDC_ARROW),
		.lpfnWndProc = main_win_handler,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.cbSize = sizeof(WNDCLASSEX)
	};

	memcpy_s(win_class, sizeof(WNDCLASSEX), &win_class_tmp, sizeof win_class_tmp);

	return RegisterClassEx(win_class);
}

HWND create_window(HINSTANCE hInst, PTCHAR window_title_str)
{
	WNDCLASSEX win_class;
	if (!register_class(_T("Window"), hInst, &win_class, GetSysColorBrush(COLOR_3DFACE)))
		return NULL;

	HWND hMain_window = CreateWindowEx(
		WS_EX_APPWINDOW,
		win_class.lpszClassName,
		window_title_str,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,
		600,
		NULL,
		NULL,
		hInst,
		NULL
	);

	return hMain_window;
}

HWND create_static_control(HWND hParent)
{
	WNDCLASSEX win_class;
	if (!register_class(_T("TreeContainer"), hParent, &win_class, COLOR_HIGHLIGHT))
		return NULL;

	HWND hControl = CreateWindowEx(
		NULL,
		win_class.lpszClassName,
		_T(""),
		WS_VISIBLE | SS_SIMPLE | WS_CHILD,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		985,
		600,
		hParent,
		NULL,
		NULL,
		NULL
	);
	SetParent(hControl, hParent);

	return hControl;
}

PTCHAR init_app(HWND* hMain_window, HWND* hTree_container, HWND* hTree_view, HINSTANCE hInst)
{
	PTCHAR ret_msg = calloc(1024, sizeof(TCHAR));
	
	if (!(*hMain_window = create_window(hInst, _T("Process&Thread manager"))))
	{
		_tcscpy_s(ret_msg, 1024, _T("create_main_window"));
		return ret_msg;
	}

	if (!(*hTree_container = create_static_control(*hMain_window)))
	{
		_tcscpy_s(ret_msg, 1024, _T("create_static_control"));
		return ret_msg;
	}

	if (!(*hTree_view = create_tree_view(*hTree_container, hInst)))
	{
		_tcscpy_s(ret_msg, 1024, _T("create_list_item"));
		return ret_msg;
	}

	return _T("");
}

//------------------------tree view---------------------------
void create_root_item(HWND hTree_container, HWND hTree_view, PTCHAR node_name_str, ULONG PID)
{
	TV_INSERTSTRUCT sTv_insert = {
	.hParent = NULL,
	.hInsertAfter = TVI_ROOT,
	.item.mask = TVIF_TEXT | TVIF_PARAM,
	.item.pszText = node_name_str,
	.item.lParam = PID
	};

	hTV_parent = SendDlgItemMessage(hTree_container, TREE_VIEW_ID, TVM_INSERTITEM, 0, &sTv_insert);
}

void create_child_item(HWND hParent, HWND hTree_view, PTCHAR item_name_str)
{
	TV_INSERTSTRUCT sTv_insert = {
	.hParent = hTV_parent,
	.hInsertAfter =	TVI_LAST,
	.item.mask = TVIF_TEXT,
	.item.pszText = item_name_str
	};

	SendDlgItemMessage(hParent, TREE_VIEW_ID, TVM_INSERTITEM, 0, &sTv_insert);
}

HWND create_tree_view(HWND hParent, HINSTANCE hInst)
{
	RECT parent_sizes;

	InitCommonControls();
	GetClientRect(hParent, &parent_sizes);

	HWND hControl = CreateWindowEx(
		NULL,
		WC_TREEVIEW,
		_T("Tree view"),
		WS_VISIBLE | WS_CHILD | TVS_HASLINES,
		0,
		0,
		parent_sizes.right,
		parent_sizes.bottom,
		hParent,
		NULL,
		hInst,
		NULL
	);
	SetParent(hControl, hParent);

	return hControl;
}
//-------------------------------------------------------------