#include <windows.h>

#include "NativeCore.hpp"

void RC_CallConv CloseRemoteProcess(RC_Pointer handle)
{
	if (handle == nullptr || reinterpret_cast<uintptr_t>(handle) == 7)
	{
		return;
	}

	CloseHandle(handle);
}
