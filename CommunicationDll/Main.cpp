#include <Windows.h>
#include <iostream>

#include "Communication.hpp"
#include "Commands.hpp"

void UninjectIfWanted(HMODULE Module, FILE* Dummy)
{
	if (GetAsyncKeyState(VK_F6) & 1)
	{
		DeleteSharedMemory();
		DeleteCommunicationEvents();

		Sleep(200);

		fclose(stdout);

		if (Dummy)
			fclose(Dummy);

		FreeConsole();

		FreeLibraryAndExitThread(Module, 0);
	}
}

void WaitForUninject(HMODULE Module, FILE* Dummy)
{
	while (true)
	{
		UninjectIfWanted(Module, Dummy);

		Sleep(100);
	}
}

void ProcessCommand()
{
	const ParamHeader Header = *reinterpret_cast<ParamHeader*>(SharedMemoryAddress);

	uintptr_t ParamLocationInSharedMem = reinterpret_cast<uintptr_t>(SharedMemoryAddress) + sizeof(ParamHeader);

	switch (Header.Type)
	{
	case ECommandType::InvalidCommand:
		printf("Receiving command type 'InvalidCommand()'\n");
		return;
	case ECommandType::GetRemotePEB:
		printf("Receiving command type 'GetRemotePEB()'\n");
		GetRemotePEB(reinterpret_cast<GetRemotePEB_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::GetRemoteModules:
		printf("Receiving command type 'GetRemoteModules()'\n");
		GetRemoteModules(reinterpret_cast<GetRemoteModules_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::GetRemoteSections:
		printf("Receiving command type 'GetRemoteSections()'\n");
		GetRemoteSections(reinterpret_cast<GetRemoteSections_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::ControlRemoteProcess:
		printf("Receiving command type 'ControlRemoteProcess()'\n");
		ControlRemoteProcess(reinterpret_cast<ControlRemoteProcess_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::ReadRemoteMemory:
		printf("Receiving command type 'ReadRemoteMemory()'\n");
		ReadRemoteMemory(reinterpret_cast<ReadRemoteMemory_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::WriteRemoteMemory:
		printf("Receiving command type 'WriteRemoteMemory()'\n");
		WriteRemoteMemory(reinterpret_cast<WriteRemoteMemory_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::GetCurrentProcessInfo:
		printf("Receiving command type 'GetCurrentProcessInfo()'\n");
		GetCurrentProcessInfo(reinterpret_cast<GetCurrentProcessInfo_Params*>(ParamLocationInSharedMem));
		break;
	default:
		break;
	}

	printf("Finished processing command!\n");
}

DWORD MainThread(HMODULE Module)
{
	/* Code to open a console window */
	AllocConsole();
	FILE* Dummy = nullptr;
	//freopen_s(&Dummy, "CONOUT$", "w", stdout);
	//freopen_s(&Dummy, "CONIN$", "r", stdin);

	// Your code here
	if (!SetupSharedMemory())
	{
		printf("Failed to open shared memory. LastError: 0x%X\n", GetLastError());

		WaitForUninject(Module, Dummy);
	}

	if (!SetupCommunicationEvents())
	{
		printf("Failed to open shared memory. LastError: 0x%X\n", GetLastError());

		// Shared mem is deleted on uninject
		WaitForUninject(Module, Dummy);
	}

	printf("Starting to process stuff\n");

	// Handle Commands
	while (true)
	{
		WaitForNewCommand();

		ProcessCommand();

		SignalCommandProcessed();

		UninjectIfWanted(Module, Dummy);
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, 0);
		break;
	}

	return TRUE;
}