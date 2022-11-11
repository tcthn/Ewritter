#include <windows.h>
#include <stdio.h>
#include <tchar.h>

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

    if(NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            //handle error
        }
    }
    return bIsWow64;
}

int main( void )
{
	
	STARTUPINFO startInfo = {0};
   	PROCESS_INFORMATION processInfo = {0};
    if(IsWow64()){
	
    printf(TEXT("The process is running under x64.\n"));
    CreateProcess(TEXT("C:\\Edev\\Ewritter\\Update_X64.exe"),NULL,NULL,NULL,FALSE,0,NULL,NULL,&startInfo,&processInfo);
   exit(3);
    }else{
	
        printf(TEXT("The process us running under x86.\n"));
        CreateProcess(TEXT("C:\\Edev\\Ewritter\\Update.exe"),NULL,NULL,NULL,FALSE,0,NULL,NULL,&startInfo,&processInfo);
   	    exit(3);
}
    return 0;
    
    
}
