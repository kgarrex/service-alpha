
#pragma comment(lib, "advapi32.lib")

#include <windows.h>
#include <stdio.h>

#include "jsmn\jsmn.h"


#if defined(_WIN32) || defined(_WIN64)
	#define WINDOWS_SERVICE 
	#define SERVICEHOST_EXPORT __declspec(dllexport)
#elif defined(__linux__)  // && !__ANDROID__
	#define LINUX_SERVICE 
	#define SERVICE_HOST
#endif


#define MAX_SERVICE_COUNT 0x10
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))


/**
 ** Typedefs
**/
typedef jsmn_parser svchost_json_document_t;

struct WinService {
	int start;
	int type;
	char *DisplayName;
	char *ServiceName;
};

struct ServiceContext{

	int svc_idx;

#if defined(WINDOWS_SERVICE)
	HANDLE hlib;
	HANDLE hservice; //handle to the service
	HANDLE StopEvent;
	SERVICE_STATUS svc_status;
	SERVICE_STATUS_HANDLE svc_status_handle;
#elif defined(LINUX_SERVICE)
	void *lib;
#endif

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


/**
** Global Variables
*/
SC_HANDLE sch_scmanager;
SERVICE_TABLE_ENTRY dispatch_table[MAX_SERVICE_COUNT];


svchost_json_document_t json_doc;
jsmntok_t tokens[10];



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


const char *proc_names[] =
{
	"OnStart",
	"OnStop",
	"OnPause",
	"OnResume",
	"OnTimer"
};

struct get_proc_params
{
	const char *procname;
};



void *get_proc_address(void *context, const void *params)
{
	struct get_proc_params *p = params;
	void *proc = 0;

	struct ServiceContext *ctx = context;
#if defined(WINDOWS_SERVICE)
	proc = GetProcAddress(ctx->hlib, p->procname);
#elif defined(LINUX_SERVICE)
	proc = dlsym(ctx->lib, p->procname);
#endif

	if(!proc){} //error

	return proc;
}

void get_events_from_lib(struct ServiceContext* ctx)
{
	char *libpath = 0;
	void *proc = 0;
	struct get_proc_params params;

	
	for(int i = 0; i < ARRAY_SIZE(proc_names); i++)
	{
		params.procname = proc_names[i];
		get_proc_address(ctx, &params);
	}
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

void report_service_status(struct ServiceContext *ctx, int cur_state, int exit_code, int wait_hint)
{
	static DWORD dwCheckPoint = 1;

	ctx->svc_status.dwCurrentState = cur_state;
	ctx->svc_status.dwWin32ExitCode = exit_code;
	ctx->svc_status.dwWaitHint = wait_hint;

	if(cur_state == SERVICE_START_PENDING)
		ctx->svc_status.dwControlsAccepted = 0;
	else ctx->svc_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if((cur_state == SERVICE_RUNNING) || (cur_state == SERVICE_STOPPED))
		ctx->svc_status.dwCheckPoint = 0;
	else ctx->svc_status.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus(ctx->svc_status_handle, &ctx->svc_status);
}


void ServiceName_ServiceMain(DWORD argc, LPTSTR argv[])
{
	SERVICE_STATUS_HANDLE svc_status_handle;
	struct ServiceContext ctx;
	SERVICE_STATUS service_status;


	service_status.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	//service_status.dwCurrentState = 

	ctx.StopEvent = CreateEventEx(0, 0, CREATE_EVENT_MANUAL_RESET, 0);
	if(!ctx.StopEvent){
		printf("CreateEventEx failed\n");
		return;
	}

	ctx.svc_status_handle = RegisterServiceCtrlHandlerEx("ServiceName", ServiceCtrlHandlerEx, &ctx);

	WaitForSingleObject(ctx.StopEvent, INFINITE);
}


struct ServiceDatabase {
	SC_HANDLE schSCManager;
};

void getValueFromConfig()
{}


void InstallService()
{
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
	//CloseServiceHandle(sch_scmanager);
}


void registerService(struct ServiceContext *ctx, const char *svc_name)
{
	static int idx = 0; 

	if(idx == MAX_SERVICE_COUNT){
		//error: too many services 
		return;
	}

	dispatch_table[idx].lpServiceName = svc_name; 
	dispatch_table[idx].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceName_ServiceMain;
	idx++;
	dispatch_table[idx].lpServiceName = 0;
	dispatch_table[idx].lpServiceProc = 0;
}

// for each service in a service host, load the shared library (the service) and call the 
int load_service()
{
#if defined(WINDOWS_SERVICE)
	HMODULE hlib;

	//DLL Security: Before loading any service, should call SetCurrentDirectory to guard against missing Dll attacks
	//Can set it back after the library has been loaded.

	//get libary filename from config
	
	hlib = LoadLibraryExA("filename", 0, 0);
	if(!hlib){
		//error: failed to load service	
	}

	get_events_from_lib(0);
	
#elif defined(LINUX_SERVICE)
	void *lib;

	lib = dlopen("filename", 0);

#endif
	return 0;
}

const char *json_string = "{\"hello\":\"world\"}";




/**
 * @brief Get the file name of the current executing
 *        svchost binary 
**/
int svchost_filename(char buf[], int bufsize)
{
#if defined(WINDOWS_SERVICE)
	DWORD length;
	char *n;
	char fullpath[256];

	length = GetModuleFileName(0, fullpath, 256);

	n = fullpath+length-1;
	for(int i = length; i; i--){
		n--;	
		if(*n == '\\'){ n++; break; }
	}
	length = length - (int)(n-fullpath);
	memcpy(buf, n, length);
	buf[length] = '\0';
	return length;
#elif defined(LINUX_SERVICE)
#endif
}


/**
** @brief Get the path of the current executing svchost binary
**
** @return the length of the path string in bytes
**/

int svchost_path(char buf[], int bufsize)
{
#if defined(WINDOWS_SERVICE)
	DWORD length;
	char *n;
	char fullpath[256];

	length = GetModuleFileName(0, fullpath, 256);
	if(!length){
		printf("GetModuleFileName failed\n");	
		return;
	}

	n = fullpath+length-1;

	for(int i = length; i; i--){
		n--;
		if(*n == '\\') break;	
	}
	length = (int)(n - fullpath);
	memcpy(buf, n, length);
	buf[length] = '\0';
	return length;
#elif defined(LINUX_SERVICE)
#endif
}


void svchost_change_file_ext(char filename[], int bufsize, const char *ext)
{
	
}


//get the binary and look for the config file here
void svchost_load_config_file()
{
	int length;
	char filename_buf[16];
	char path_buf[256];

	length = svchost_filename(filename_buf, 16);
	printf("file(%d): %s\n", length, filename_buf);

	length = svchost_path(path_buf, 256);
	printf("path (%d): %s\n", length, path_buf);

	
	//CreateFileA(
	//json_doc 	
}

//26 letters in the alphabet, max number of options


struct cmd_opt {
	char *tag;
	char *label;
	int (*proc)();
}


struct cmd_subcmd {
	char *tag; //label
	struct cmd_opt opt[26];
};

int process_main(int subcmdc, struct cmd_subcmd, int argc, char *argv[])
{
		
}


/**
** @param
**/

SERVICEHOST_EXPORT void svchost_process_main(int argc, char *argv[])
{

	if(argc > 1){
		if(!memcmp(argv[1], "install", 7)){
			InstallService();
			return; 
		}
	}

	struct ServiceContext context;

	//loop through each service in the config
	//registerService(&context, svc_name);
	
	//jsmn_init(&parser);


	//jsmn_parse(&parser, json_string, strlen(json_string), tokens, 10);

	char n[8];

	//n = json_string + tokens[0].start;
	//printf("Here %s\n", 2, n);



	svchost_load_config_file();	

	 

	//StartServiceCtrlDispatcher(dispatch_table);

}
