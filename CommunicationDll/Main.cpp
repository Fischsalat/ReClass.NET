#include <Windows.h>
#include <iostream>

#include "Communication.hpp"
#include "Commands.hpp"

void UninjectIfWanted(HMODULE Module, FILE* Dummy)
{
	if (GetAsyncKeyState(VK_F6) & 1)
	{
		Sleep(200);

		fclose(stdout);
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

	uintptr_t ParamLocationInSharedMem = reinterpret_cast<uintptr_t>(SharedMemoryAddress) + 0x10;

	switch (Header.Type)
	{
	case ECommandType::InvalidCommand:
		return;
	case ECommandType::GetRemotePEB:
		GetRemotePEB(reinterpret_cast<GetRemotePEB_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::GetRemoteModules:
		GetRemoteModules(reinterpret_cast<GetRemoteModules_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::GetRemoteSections:
		GetRemoteSections(reinterpret_cast<GetRemoteSections_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::ControlRemoteProcess:
		ControlRemoteProcess(reinterpret_cast<ControlRemoteProcess_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::ReadRemoteMemory:
		ReadRemoteMemory(reinterpret_cast<ReadRemoteMemory_Params*>(ParamLocationInSharedMem));
		break;
	case ECommandType::WriteRemoteMemory:
		WriteRemoteMemory(reinterpret_cast<WriteRemoteMemory_Params*>(ParamLocationInSharedMem));
		break;
	default:
		break;
	}
}

DWORD MainThread(HMODULE Module)
{
	/* Code to open a console window */
	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, "CONOUT$", "w", stdout);
	freopen_s(&Dummy, "CONIN$", "r", stdin);

	// Your code here
	if (!SetupSharedMemory())
	{
		printf("Failed to open shared memory. LastError: 0x%X\n", GetLastError());

		WaitForUninject(Module, Dummy);
	}

	if (!SetupCommunicationEvents())
	{
		printf("Failed to open shared memory. LastError: 0x%X\n", GetLastError());
		DeleteSharedMemory();

		WaitForUninject(Module, Dummy);
	}

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