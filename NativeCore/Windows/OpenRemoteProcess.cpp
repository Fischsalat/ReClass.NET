#include <windows.h>

#include "NativeCore.hpp"
#include "CommandParameters.hpp"
#include "Communication.hpp"

RC_Pointer RC_CallConv OpenRemoteProcess(RC_Pointer id, ProcessAccess desiredAccess)
{
	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, "CONOUT$", "w", stdout);
	freopen_s(&Dummy, "CONIN$", "r", stdin);

	if (id == reinterpret_cast <RC_Pointer>(7) && desiredAccess == ProcessAccess::Read)
	{
		printf("Initializiing\n\n");

		SetupSharedMemory();
		SetupCommunicationEvents();
	}

	return reinterpret_cast<RC_Pointer>(7);
}

