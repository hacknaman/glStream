#include <signal.h>
#include <Windows.h>
#include <dbghelp.h>
#include <process.h>
std::ofstream ofs("VizDebugLog");

#define VIZ_LOG(x) std::cout << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":" << x << std::endl; std::clog << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ":" << x << std::endl;

#ifdef _DEBUG
#ifdef _WIN32
#define TRACE_MAX_STACK_FRAMES 1024
#define TRACE_MAX_FUNCTION_NAME_LENGTH 1024

int printStackTrace()
{
    void *stack[TRACE_MAX_STACK_FRAMES];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD numberOfFrames = CaptureStackBackTrace(0, TRACE_MAX_STACK_FRAMES, stack, NULL);
    SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR));
    symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    DWORD displacement;
    IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
    line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    for (int i = 0; i < numberOfFrames; i++)
    {
        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, NULL, symbol);
        if (SymGetLineFromAddr64(process, address, &displacement, line))
        {
            VIZ_LOG("\tat " << symbol->Name << " in " << line->FileName << ": line: " << line->LineNumber << "address: " << symbol->Address );
        }
        else
        {
            VIZ_LOG("\tSymGetLineFromAddr64 returned error code " << GetLastError());
            VIZ_LOG("\tat " << symbol->Name << " address " << symbol->Address);
        }
    }
    return 0;
}

void programExit() {
    VIZ_LOG("Viz Program is exiting . . .");
}

void programExitSig(int signum) {
    VIZ_LOG("Viz Program is exiting sig " << signum);
    printStackTrace();
}

#endif // _WIN32
#endif // _DEBUG

void initdebug() {
    std::clog.rdbuf(ofs.rdbuf());
#ifdef _DEBUG
    atexit(programExit);
    signal(SIGTERM, programExitSig);
    signal(SIGSEGV, programExitSig);
    signal(SIGINT,  programExitSig);
    signal(SIGILL,  programExitSig);
    signal(SIGABRT, programExitSig);
    signal(SIGFPE,  programExitSig);
#endif // _DEBUG
}
