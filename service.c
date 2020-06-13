
#pragma comment(lib, "advapi32.lib")

#include <windows.h>
#include <stdio.h>


#if defined(_WIN32) || defined(_WIN64)
	#define SERVICE_WINDOWS
#elif defined(__linux__)  // && !__ANDROID__
	#define SERVICE_LINUX
#endif


#define MAX_SERVICE_COUNT 0x10


struct WinService {
	int start;
	int type;
	char *DisplayName;
	char *ServiceName;
};

struct ServiceContext{
	HANDLE hservice; //handle to the service
	HANDLE StopEvent;
	void (*OnTimer)(void*);
	void (*OnStart)(void*);
	void (*OnResumeService)(void*);
	void (*OnStopService)(void*);
	void (*OnPauseService)(void*);
	void (*OnInterrogateService)(void*);
	void (*OnShutdownService)(void*);
	void (*OnParamChange)(void*);
	void (*OnNetBindAdd)(void*);
	void (*OnNetBindRemove)(void*);
	void (*OnNetBindEnable)(void*);
	void (*OnNetBindDisable)(void*);
	void (*OnDeviceEvent)(void*);
	void (*OnHardwareProfileChange)(void*);
	void (*OnPowerEvent)(void*);
	void (*OnSessionChange)(void*);
	void (*OnTimeChange)(void*);
	void (*OnTriggerEvent)(void*);
	void (*OnUserModeReboot)(void*);
	void (*OnPreShutdown)(void*);
};

enum SvcCtrlCodeEnum
{
	SvcCtrlCodeStart,
	SvcCtrlCodeStop,
	SvcCtrlCodeContinue,
	SvcCtrlCodeTimer,
};

#define SERVICE_CONTROL_TIMER 128


/*
	case SIGABRT:
	case SIGALRM:
	case SIGBUS:
	case SIGCHLD:
	case SIGCONT:
	case SIGKILL:
	case SIGPOLL:
	case SIGPWR:
	case SIGQUIT:
	case SIGSTOP:
	case SIGTSTP:
	case SIGTERM:
*/


void CALLBACK TimerCallback(PVOID param, BOOLEAN not_used)
{
	SERVICE_STATUS status;
	struct ServiceContext * ctx = param;	

	//ctx->OnTimer(0);
	ControlService(ctx->hservice, SERVICE_CONTROL_TIMER, &status);
}


int service_host_main()
{
	HANDLE timer_queue;
	HANDLE timer;
	int time_in_ms = 0;
	int time_to_elapse = 0;
	int result;

	timer_queue = CreateTimerQueue();
	
	result = CreateTimerQueueTimer(&timer, timer_queue, TimerCallback, &time_to_elapse, time_in_ms, time_in_ms, 0);  
	if(!result){
		printf("Error: CreateTimerQueueTimer failed\n");
		return 0;
	}

	//Once the service is stopped, call DeleteTimerQueueTimer
	return 1;
}

int service_handler(int code)
{
	switch(code)
	{
		/*
		case SvcCtrlCodeStart:
		case SvcCtrlCodeStop:
		case SvcCtrlCodeContinue:
		case SvcCtrlCodeTimer:
		*/
	}
	return 0;
}

void get_events_from_lib(struct ServiceContext* ctx)
{
	char *libpath = 0;
	void *proc = 0;

	//get library path from config
	
	proc = GetProcAddress(hlib, "OnStart");
	if(!proc){
		//error: failed to get proc address	
	}
	proc = GetProcAddress(hlib, "OnStop");
	proc = GetProcAddress(hlib, "OnPause");
	proc = GetProcAddress(hlib, "OnResume");
	proc = GetProcAddress(hlib, "OnTimer");

}

DWORD WINAPI ServiceCtrlHandlerEx(DWORD ControlCode, DWORD EventType, LPVOID EventData, LPVOID Context)
{
	struct ServiceContext *ctx = Context;

	switch(ControlCode){

	case SERVICE_CONTROL_STOP:
		//SERVICE_STOP_PENDING
		ctx->OnStopService(0);
		SetEvent(ctx->StopEvent);
	//SERVICE_STOPPED
		break;

	case SERVICE_CONTROL_PAUSE:
		//SERVICE_PAUSE_PENDING
		ctx->OnPauseService(0);
	//SERVICE_PAUSED
		break;

	case SERVICE_CONTROL_CONTINUE:
		//SERVICE_CONTINUE_PENDING
		ctx->OnResumeService(0);
	//SERVICE_RUNNING
		break;

	case SERVICE_CONTROL_INTERROGATE:
		ctx->OnInterrogateService(0);
		break;

	case SERVICE_CONTROL_SHUTDOWN: 
		ctx->OnShutdownService(0);
		break;

	case SERVICE_CONTROL_PARAMCHANGE:
		ctx->OnParamChange(0);
		break;

	case SERVICE_CONTROL_NETBINDADD:
		ctx->OnNetBindAdd(0);
		break;

	case SERVICE_CONTROL_NETBINDREMOVE:
		ctx->OnNetBindRemove(0);
		break;

	case SERVICE_CONTROL_NETBINDENABLE:
		ctx->OnNetBindEnable(0);
		break;

	case SERVICE_CONTROL_NETBINDDISABLE:
		ctx->OnNetBindDisable(0);
		break;

	case SERVICE_CONTROL_DEVICEEVENT:
		ctx->OnDeviceEvent(0);
		break;

	case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
		ctx->OnHardwareProfileChange(0);
		break;

	case SERVICE_CONTROL_POWEREVENT:
		ctx->OnPowerEvent(0);
		break;

	case SERVICE_CONTROL_SESSIONCHANGE:
		ctx->OnSessionChange(0);
		break;

	case SERVICE_CONTROL_PRESHUTDOWN:
		ctx->OnPreShutdown(0);
		break;

	case SERVICE_CONTROL_TIMECHANGE:
		ctx->OnTimeChange(0);
		break;

	case SERVICE_CONTROL_TRIGGEREVENT:
		ctx->OnTriggerEvent(0);
		break;

	/*
	case SERVICE_CONTROL_USER_LOGOFF: 
	case SERVICE_CONTROL_LOWRESOURCES:
	case SERVICE_CONTROL_SYSTEMLOWRESOURCES:
	*/

	}

	return NO_ERROR;
}

void ServiceName_ServiceMain(DWORD argc, LPTSTR argv[])
{
	struct ServiceContext ctx;
	SERVICE_STATUS service_status;

	/*
	ctx.OnResumeService		= OnResumeService;
	ctx.OnStopService		= OnStopService;
	ctx.OnPauseService		= OnPauseService;
	ctx.OnInterrogateService	= OnInterrogateService;
	ctx.OnShutdownService		= OnShutdownService;
	ctx.OnParamChange		= OnParamChange;
	ctx.OnNetBindAdd		= OnNetBindAdd;
	ctx.OnNetBindRemove		= OnNetBindRemove;
	ctx.OnNetBindEnable		= OnNetBindEnable;
	ctx.OnNetBindDisable		= OnNetBindDisable;
	ctx.OnDeviceEvent		= OnDeviceEvent;
	ctx.OnHardwareProfileChange 	= OnHardwareProfileChange;
	ctx.OnPowerEvent		= OnPowerEvent;
	ctx.OnSessionChange		= OnSessionChange;
	ctx.OnTimeChange		= OnTimeChange;
	ctx.OnTriggerChange		= OnTriggerChange;
	ctx.OnUserModeReboot		= OnUserModeReboot;
	ctx.OnPreShutdown		= OnPreShutdown;
	*/

	service_status.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	//service_status.dwCurrentState = 

	ctx.StopEvent = CreateEventEx(0, 0, CREATE_EVENT_MANUAL_RESET, 0);
	if(!ctx.StopEvent){
		printf("CreateEventEx failed\n");
		return;
	}

	RegisterServiceCtrlHandlerEx("ServiceName", ServiceCtrlHandlerEx, &ctx);

	WaitForSingleObject(ctx.StopEvent, INFINITE);
}

struct ServiceDatabase {
	SC_HANDLE schSCManager;
};

void getValueFromConfig()
{}


void InstallService()
{
	SC_HANDLE sch_scmanager;
	SC_HANDLE sch_service;

	sch_scmanager = OpenSCManager(0, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
	if(!sch_scmanager){
		printf("OpenSCManager failed\n");
		return;
	}
	
	sch_service = CreateService(
		sch_scmanager,
		"ServiceName",
		"DisplayName",
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS, //or SERVICE_USER_SHARE_PROCESS
		SERVICE_DEMAND_START, //or SERVICE_AUTO_START
		SERVICE_ERROR_NORMAL,
		"BinaryPathName",
		"LoadOrderGroup",
		0, //TagId,
		0, //Dependencies,
		0, //ServiceStartName,
		0  //Password
	);

	if(!sch_service){
		printf("CreateService failed\n");
	return;
	}
	printf("Service installed successfully\n");

	CloseServiceHandle(sch_service);
	CloseServiceHandle(sch_scmanager);
}

void registerService()
{
	int idx = 0;

	SERVICE_TABLE_ENTRY DispatchTable[MAX_SERVICE_COUNT];

	if(idx == MAX_SERVICE_COUNT){
		//error: too many services 
		return;
	}

	DispatchTable[idx].lpServiceName = "ServiceName";
	DispatchTable[idx].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceName_ServiceMain;
	idx++;
	DispatchTable[idx].lpServiceName = 0;
	DispatchTable[idx].lpServiceProc = 0;
}

// for each service in a service host, load the shared library (the service) and call the 
int servicehost_load_service()
{
#if defined(SERVICE_WINDOWS)
	HMODULE hlib;

	//DLL Security: Before loading any service, should call SetCurrentDirectory to guard against missing Dll attacks
	//Can set it back after the library has been loaded.

	//get libary filename from config
	
	hlib = LoadLibraryExA("filename", 0, 0);
	if(!hlib){
		//error: failed to load service	
	}

	get_events_from_lib(0);
	
#endif

	//dlopen();
	//dlsym();

	return 0;
}

int main(int argc, char *argv[])
{
	if(!memcmp(argv[1], "install", 7)){
		InstallService();
		return; 
	}

	 
	//StartServiceCtrlDispatcher(DispatchTable);
}
