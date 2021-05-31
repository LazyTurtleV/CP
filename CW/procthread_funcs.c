#include "serv_func.h"

//some typedefs responsible for ntdll funcs
//--------------------------------------------------------------------------------------

#define INITIAL_BUFFER_SIZE 10000L

#define STATUS_SUCCESS               ((NTSTATUS)0x00000000L)
#define STATUS_INFO_LENGTH_MISMATCH  ((NTSTATUS)0xC0000004L)

typedef NTSTATUS(WINAPI* __NtQuerySystemInformation)
(
	IN       SYSTEM_INFORMATION_CLASS sys_info_class,
	IN OUT   PVOID sys_info_sarrp,
	IN       ULONG sys_info_arr_length,
	OUT OPTIONAL  PULONG ret_length
);

typedef enum _KWAIT_REASON
{
	Executive = 0,
	FreePage = 1,
	PageIn = 2,
	PoolAllocation = 3,
	DelayExecution = 4,
	Suspended = 5,
	UserRequest = 6,
	WrExecutive = 7,
	WrFreePage = 8,
	WrPageIn = 9,
	WrPoolAllocation = 10,
	WrDelayExecution = 11,
	WrSuspended = 12,
	WrUserRequest = 13,
	WrEventPair = 14,
	WrQueue = 15,
	WrLpcReceive = 16,
	WrLpcReply = 17,
	WrVirtualMemory = 18,
	WrPageOut = 19,
	WrRendezvous = 20,
	Spare2 = 21,
	Spare3 = 22,
	Spare4 = 23,
	Spare5 = 24,
	WrCalloutStack = 25,
	WrKernel = 26,
	WrResource = 27,
	WrPushLock = 28,
	WrMutex = 29,
	WrQuantumEnd = 30,
	WrDispatchInt = 31,
	WrPreempted = 32,
	WrYieldExecution = 33,
	WrFastMutex = 34,
	WrGuardedMutex = 35,
	WrRundown = 36,
	MaximumWaitReason = 37
} KWAIT_REASON;

const TCHAR* const _KWAIT_REASON_S[] =
{
	[Executive] = _T("Executive"),
	[FreePage] = _T("FreePage"),
	[PageIn] = _T("PageIn"),
	[PoolAllocation] = _T("PoolAllocation"),
	[DelayExecution] = _T("DelayExecution"),
	[Suspended] = _T("Suspended"),
	[UserRequest] = _T("UserRequest"),
	[WrExecutive] = _T("WrExecutive"),
	[WrFreePage] = _T("WrFreePage"),
	[WrPageIn] = _T("WrPageIn"),
	[WrPoolAllocation] = _T("WrPoolAllocation"),
	[WrDelayExecution] = _T("WrDelayExecution"),
	[WrSuspended] = _T("WrSuspended"),
	[WrUserRequest] = _T("WrUserRequest"),
	[WrEventPair] = _T("WrEventPair"),
	[WrQueue] = _T("WrQueue"),
	[WrLpcReceive] = _T("WrLpcReceive"),
	[WrLpcReply] = _T("WrLpcReply"),
	[WrVirtualMemory] = _T("WrVirtualMemory"),
	[WrPageOut] = _T("WrPageOut"),
	[WrRendezvous] = _T("WrRendezvous"),
	[Spare2] = _T("Spare2"),
	[Spare3] = _T("Spare3"),
	[Spare4] = _T("Spare4"),
	[Spare5] = _T("Spare5"),
	[WrCalloutStack] = _T("WrCalloutStack"),
	[WrKernel] = _T("WrKernel"),
	[WrResource] = _T("WrResource"),
	[WrPushLock] = _T("WrPushLock"),
	[WrMutex] = _T("WrMutex"),
	[WrQuantumEnd] = _T("WrQuantumEnd"),
	[WrDispatchInt] = _T("WrDispatchInt"),
	[WrPreempted] = _T("WrPreempted"),
	[WrYieldExecution] = _T("WrYieldExecution"),
	[WrFastMutex] = _T("WrFastMutex"),
	[WrGuardedMutex] = _T("WrGuardedMutex"),
	[WrRundown] = _T("WrRundown"),
	[MaximumWaitReason] = _T("MaximumWaitReason")
};

typedef struct _SYSTEM_THREAD_INFO_DETAILED_S
{
	LARGE_INTEGER 	KernelTime;
	LARGE_INTEGER 	UserTime;
	LARGE_INTEGER 	CreateTime;
	ULONG 	WaitTime;
	PVOID 	StartAddress;
	CLIENT_ID 	ClientId;
	KPRIORITY 	Priority;
	LONG 	BasePriority;
	ULONG 	ContextSwitches;
	ULONG 	ThreadState;
	KWAIT_REASON WaitReason;
} _SYSTEM_THREAD_INFO_DETAILED_S, *PSYSTEM_THREAD_INFO_DETAILED_S;

//--------------------------------------------------------------------------------------

void print_proc_info(PSYSTEM_PROCESS_INFORMATION pProc_info, HWND hTree_container, HWND hTree_view);
void print_thread_info(PSYSTEM_THREAD_INFO_DETAILED_S pThread_info, HWND hTree_container, HWND hTree_view);

NTSTATUS print_sys_info(FILE *stream_d, HWND hTree_container, HWND hTree_view)
{
	__NtQuerySystemInformation __nt_query_sys_info = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQuerySystemInformation");
	
	pSys_proc_info = malloc(INITIAL_BUFFER_SIZE);
	
	NTSTATUS status;
	size_t buffer_size = INITIAL_BUFFER_SIZE;
	while (TRUE)
	{
		status = __nt_query_sys_info(SystemProcessInformation, pSys_proc_info, buffer_size, NULL);

		if (!status) break;
		else if ((unsigned)status <= 0x1E7)
		{
			return status;
		}
		else if (status != STATUS_INFO_LENGTH_MISMATCH)
		{
			return status;
		}
		buffer_size *= 2;
		pSys_proc_info = realloc(pSys_proc_info, buffer_size);
	}

	//save initial position 
	PSYSTEM_PROCESS_INFORMATION pInitial_position = pSys_proc_info;

	for (; pSys_proc_info->NextEntryOffset != NULL; pSys_proc_info = pSys_proc_info->NextEntryOffset + (PBYTE)pSys_proc_info)
	{
		print_proc_info(pSys_proc_info, hTree_container, hTree_view);

		PSYSTEM_THREAD_INFO_DETAILED_S sys_thread_info = pSys_proc_info + 1;
		for (int count = 0; sys_thread_info < pSys_proc_info->NextEntryOffset + (PBYTE)pSys_proc_info && count < pSys_proc_info->NumberOfThreads; ++sys_thread_info, ++count)
		{
			print_thread_info(sys_thread_info, hTree_container, hTree_view);
		}
	}

	pSys_proc_info = pInitial_position;

	return status;
}

void print_proc_info(PSYSTEM_PROCESS_INFORMATION pProc_info, HWND hTree_container, HWND hTree_view)
{
	PTCHAR proc_info_str = _T("null"); 
	if (pProc_info->ImageName.Length)
	{
		proc_info_str = malloc(pProc_info->ImageName.Length*sizeof(TCHAR));
		_tcscpy_s(proc_info_str, pProc_info->ImageName.Length, pProc_info->ImageName.Buffer);
	}
	

	create_root_item(hTree_container, hTree_view, proc_info_str, pProc_info->UniqueProcessId);
}

void print_thread_info(PSYSTEM_THREAD_INFO_DETAILED_S pThread_info, HWND hTree_container, HWND hTree_view)
{
	PTCHAR thread_label_str = malloc(1024*sizeof(TCHAR));
	_sntprintf_s(thread_label_str, 1024, 1024, _T("TID: %u, thread state: %2u, wait reason: %s"), pThread_info->ClientId.UniqueThread, pThread_info->ThreadState, _KWAIT_REASON_S[pThread_info->WaitReason]);
	create_child_item(hTree_container, hTree_view, thread_label_str);
}

//------------------------------------------------------------------------------------------------------------------------