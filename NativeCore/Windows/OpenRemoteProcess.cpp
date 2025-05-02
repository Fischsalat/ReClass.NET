#include <windows.h>

#include "NativeCore.hpp"
#include "CommandParameters.hpp"
#include "Communication.hpp"

RC_Pointer RC_CallConv OpenRemoteProcess(RC_Pointer id, ProcessAccess desiredAccess)
{
	SetupSharedMemory();
	SetupCommunicationEvents();

	return reinterpret_cast<RC_Pointer>(7);
}
