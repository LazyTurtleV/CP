#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winternl.h>
#include <tchar.h>
#include <CommCtrl.h>


//---------------------------get system info--------------------------------
#define MB 1048576
#define BYTES_TO_MB(B) (B/MB)
#define RESERVED_PROC_NAME_STR _T("Ashuahshahaa_jxh")

PSYSTEM_PROCESS_INFORMATION pSys_proc_info;

NTSTATUS print_sys_info(FILE *stream_d, HWND hTree_container, HWND hTree_view);

//---------------------------events handling--------------------------------

void report_err(PTCHAR msg, HWND hParent, ULONG err_code);
LRESULT CALLBACK main_win_handler(HWND, UINT, WPARAM, LPARAM);

//-------------------------------- GUI -----------------------------------

HWND create_window(HINSTANCE hInst, PTCHAR window_title_str);
HWND create_static_control(HWND parent);
PTCHAR init_app(HWND* hMain_window, HWND* hTree_container, HWND* hTree_view, HINSTANCE hInst);

HWND create_tree_view(HWND hParent, HINSTANCE hInst);
void create_root_item(HWND hTree_container, HWND hTree_view, PTCHAR node_name, ULONG PID);
void create_child_item(HWND hParent, HWND hTree_view, PTCHAR node_name);

//Tree view global vars ---------------------------------------------------

HTREEITEM hTV_parent;
const int TREE_VIEW_ID;

//--------------------------------------------------------------------------
