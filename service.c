

#define MAX_SERVICE_COUNT 0x10


static void OnResumeService(LPVOID)
{}

static void OnStopService(LPVOID)
{}

static void OnPauseService(LPVOID)
{}

static void OnInterrogateService(LPVOID)
{}

static void OnShutdownService(LPVOID)
{}

static void OnParamChange(LPVOID)
{}

static void OnNetBindAdd(LPVOID)
{}

static void OnNetBindRemove(LPVOID)
{}

static void OnNetBindEnable(LPVOID)
{}

static void OnNetBindDisable(LPVOID)
{}

static void OnDeviceEvent(LPVOID)
{}

static void OnHardwareProfileChange(LPVOID)
{}

static void OnPowerEvent(LPVOID)
{}

static void OnSessionChange(LPVOID)
{}

static void OnTimeChange(LPVOID)
{}

static void OnTriggerEvent(LPVOID)
{}

static void OnUserModeReboot(LPVOID)
{}

static void OnPreShutdown(LPVOID)
{}


struct WinService {
	int start;
	int type;
	char *DisplayName;
	char *ServiceName;
};

struct ServiceContext{
	HANDLE StopEvent;
	void (*OnTimer)(void*);
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


int service_host_main()
{
	HANDLE timer_queue;
	HANDLE timer;
	int time_in_ms;
	int result;

	timer_queue = CreateTimerQueue();
	
	result = CreateTimerQueueTimer(&timer, timer_queue, callback, time_in_ms, time_in_ms, 0);  
	if(!result){
		printf("Error: CreateTimerQueueTimer failed\n");
		return 0;
	}

	//Once the service is stopped, call DeleteTimerQueueTimer
}

void CALLBACK TimerCallback(PVOID param, BOOLEAN not_used)
{
	SERVICE_STATUS status;
	struct ServiceContext * ctx = param;	

	//ctx->OnTimer(0);
	ControlService(hservice, SERVICE_CONTROL_TIMER, &status);
}

int service_handler(int code)
{
	switch(code)
	{
		case SvcCtrlCodeStart:
		case SvcCtrlCodeStop:
		case SvcCtrlCodeContinue:
		case SvcCtrlCodeTimer:
	}
}


DWORD WINAPI ServiceCtrlHandlerEx(DWORD ControlCode, DWORD EventType, LPVOID EventData, LPVOID Context)
{
	struct ServiceContext *ctx = Context;

	switch(ControlCode){

	case SERVICE_CONTROL_STOP:
		//SERVICE_STOP_PENDING
		ctx->OnStopService();
		SetEvent(ctx->StopEvent);
	//SERVICE_STOPPED
		break;

	case SERVICE_CONTROL_PAUSE:
		//SERVICE_PAUSE_PENDING
		ctx->OnPauseService();
	//SERVICE_PAUSED
		break;

	case SERVICE_CONTROL_CONTINUE:
		//SERVICE_CONTINUE_PENDING
		ctx->OnResumeService();
	//SERVICE_RUNNING
		break;

	case SERVICE_CONTROL_INTERRORGATE:
		ctx->OnInterrogateService();
		break;

	case SERVICE_CONTROL_SHUTDOWN: 
		ctx->OnShutdownService();
		break;

	case SERVICE_CONTROL_PARAMCHANGE:
		ctx->OnParamChange();
		break;

	case SERVICE_CONTROL_NETBINDADD:
		ctx->OnNetBindAdd();
		break;

	case SERVICE_CONTROL_NETBINDREMOVE:
		ctx->OnNetBindRemove();
		break;

	case SERVICE_CONTROL_NETBINDENABLE:
		ctx->OnNetBindEnable();
		break;

	case SERVICE_CONTROL_NETBINDDISABLE:
		ctx->OnNetBindDisable();
		break;

	case SERVICE_CONTROL_DEVICEEVENT:
		ctx->OnDeviceEvent();
		break;

	case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
		ctx->OnHardwareProfileChange();
		break;

	case SERVICE_CONTROL_POWEREVENT:
		ctx->OnPowerEvent();
		break;

	case SERVICE_CONTROL_SESSIONCHANGE:
		ctx->OnSessionChange();
		break;

	case SERVICE_CONTROL_TIMECHANGE:
		ctx->OnTimeChange();
		break;

	case SERVICE_CONTROL_TRIGGEREVENT:
		ctx->OnTriggerEvent();
		break;

	case SERVICE_CONTROL_USERMODEREBOOT:
		ctx->OnUserModeReboot();
		break;

	case SERVICE_CONTROL_PRESHUTDOWN:
		ctx->OnPreShutdown();
		break;

	}
}

void ServiceName_ServiceMain(DWORD argc, LPTSTR argv[])
{
	struct ServiceContext ctx;
	SERVICE_STATUS service_status;

	ctx.OnResumService		= OnResumeService;
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
}

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
		schandle,
		"ServiceName",
		"DisplayName",
		SERVICE_ALL_ACCESS,
		ServiceType,
		StartType,
		ErrorControl,
		BinaryPathName,
		LoadOrderGroup,
		TagId,
		Dependencies,
		ServiceStartName,
		Password
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
	SERVICE_TABLE_ENTRY DispatchTable[MAX_SERVICE_COUNT];

	if(idx == MAX_SERVICE_COUNT){
		//error: too many services 
		return;
	}

	DispatchTable[idx].lpServiceName = "ServiceName";
	DispatchTable[idx].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain_ServiceMain;
	idx++;
	DispatchTable[idx].lpServiceName = 0;
	DispatchTable[idx].lpServiceProc = 0;
}

// for each service in a service host, load the shared library (the service) and call the 
int servicehost_load_service()
{
#if defined(WINDOWS)
	HMODULE hlib;
	void *proc;

	hlib = LoadLibraryExA("filename", 0, 0);
	if(!hlib){
		//error: failed to load service	
	}
	proc = GetProcAddress(hlib, "procname");
	if(!proc){
		//error: failed to get proc address	
	}
#endif

	//dlopen();
	//dlsym();
}

int main(int argc, char *argv[])
{
	if(!memcmp(argv[1], "install", 7)){
		InstallService();
		return; 
	}

	 
	StartServiceCtrlDispatcher(DispatchTable);
}
