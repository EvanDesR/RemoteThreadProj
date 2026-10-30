#include <Windows.h>
#include <stdio.h>


int Error(const char* msg)
{
	printf("Err:(%s)   %u", msg, GetLastError());
	return 1;
}

int main(int argc, const char* argv[])
{
	if (argc < 3)
	{
		printf("useage remotethread <pid> <dllpath> \n");
	}
	int pid = atoi(argv[1]);


	HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, FALSE, pid);
	if (!hProcess)
	{
		return Error("Error opening process \n");
	}
	void* buffer = VirtualAllocEx(hProcess, nullptr, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); //Allocate memory in target process if that process has PROCESS_VM_OPERATION
	if (!buffer)
	{
		return Error("Failed to Allocate memory");
	}

	if(!WriteProcessMemory(hProcess, buffer, argv[2], strlen(argv[2]),nullptr)) //DLL path is written to the virtual memory in the target process
	{
		return Error("WriteProcessMemory failed");
	}

	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, 
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"kernel32") //Get memory address of loadlibraryA, which is in kernel32.dll
			, "LoadLibraryA"),
			buffer,0,nullptr); //Get Address of LoadLibrary functi
	if (!hThread)
	{
		return Error("hThread failed. Failed to create thread!");
	}
	return 1;

}