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

void report_err(PTCHAR msg, FILE* stream_d)
{
	DWORD err_code = GetLastError();

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

	if(stream_d) _ftprintf(stream_d, _T("%s failed, err_code: 0x%X\n%ls"), msg, err_code, err_msg);
	_ftprintf(stderr, _T("%s failed, err_code: 0x%X\n%ls"), msg, err_code, err_msg);
}

NTSTATUS print_sys_info(FILE* stream_d, INFO_LEVEL_E info_level)
{
	__NtQuerySystemInformation __nt_query_sys_info = GetProcAddress(GetModuleHandle(_T("ntdll.dll")), "NtQuerySystemInformation");
	
	PSYSTEM_PROCESS_INFORMATION proc_info_ptr = malloc(INITIAL_BUFFER_SIZE);
	NTSTATUS status;

	size_t buffer_size = INITIAL_BUFFER_SIZE;
	while (TRUE)
	{
		status = __nt_query_sys_info(SystemProcessInformation, proc_info_ptr, buffer_size, NULL);

		if (!status) break;
		else if ((unsigned)status <= 0x1E7)
		{
			return status;
		}
		else if (status != STATUS_INFO_LENGTH_MISMATCH)
		{
			_ftprintf(stderr, _T("Error: failed getting processes' info. Err code %X"), status);
			return status;
		}
		buffer_size *= 2;
		proc_info_ptr = realloc(proc_info_ptr, buffer_size);
	}

	for (; proc_info_ptr->NextEntryOffset != NULL; proc_info_ptr = proc_info_ptr->NextEntryOffset + (PBYTE)proc_info_ptr)
	{
		_fputts(_T("================================================================================================\n"), stream_d);
		
		switch (info_level)
		{
			case USER: 
			{
				_ftprintf(
					stream_d,
					_T("PID: %lu, process name: %s, threads count: %ld\n"),
					proc_info_ptr->UniqueProcessId,
					proc_info_ptr->ImageName.Buffer,
					proc_info_ptr->NumberOfThreads
				);
				_fputts(_T("-----------------------------------------------------------------------------------------------\n"), stream_d);
				PSYSTEM_THREAD_INFO_DETAILED_S sys_thread_info_ptr = proc_info_ptr + 1;
				for (int count = 0; sys_thread_info_ptr < proc_info_ptr->NextEntryOffset + (PBYTE)proc_info_ptr && count < proc_info_ptr->NumberOfThreads; ++sys_thread_info_ptr, ++count)
				{
					_ftprintf(
						stream_d,
						_T("TID: %8lu, thread state: %2ld, base priotiry: %2ld, dynamic priority: %2ld\n"),
						sys_thread_info_ptr->ClientId.UniqueThread,
						sys_thread_info_ptr->ThreadState,
						sys_thread_info_ptr->BasePriority,
						sys_thread_info_ptr->Priority
					);
				}
				break;
			}
			case DEVELOPER: 
			{
				_ftprintf(
					stream_d,
					_T("PID: %lu, process name: %s, threads count: %lu, handles uses: %lu, session ID: %lu\nPeak virtual mem allocated: %llu B, Virtual mem allocated: %llu B\n"),
					proc_info_ptr->UniqueProcessId,
					proc_info_ptr->ImageName.Buffer,
					proc_info_ptr->NumberOfThreads,
					proc_info_ptr->HandleCount,
					proc_info_ptr->SessionId,
					proc_info_ptr->PeakVirtualSize,
					proc_info_ptr->VirtualSize
				);
				PSYSTEM_THREAD_INFO_DETAILED_S sys_thread_info = proc_info_ptr + 1;
				for (int count = 0; sys_thread_info < proc_info_ptr->NextEntryOffset + (PBYTE)proc_info_ptr && count < proc_info_ptr->NumberOfThreads; ++sys_thread_info, ++count)
				{
					_fputts(_T("-----------------------------------------------------------------------------------------------\n"), stream_d);
					_ftprintf(
						stream_d,
						_T("TID: %8lu, thread state: %2d, base priotiry: %2u, dynamic priority: %2u\nWait reason: %s\nWait time: %12lu, kernel time: %12lld, user time: %8lld\n"),
						sys_thread_info->ClientId.UniqueThread,
						sys_thread_info->ThreadState,
						sys_thread_info->BasePriority,
						sys_thread_info->Priority,
						_KWAIT_REASON_S[sys_thread_info->WaitReason],
						sys_thread_info->WaitTime,
						sys_thread_info->KernelTime.QuadPart,
						sys_thread_info->UserTime.QuadPart
					);
				}

				break;
			}
		}
	}

	return status;
}