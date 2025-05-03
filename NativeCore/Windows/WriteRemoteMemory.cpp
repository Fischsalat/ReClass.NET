#include <windows.h>
#include <algorithm>

#include "NativeCore.hpp"
#include "CommandParameters.hpp"
#include "Communication.hpp"


bool RC_CallConv WriteRemoteMemory(RC_Pointer handle, RC_Pointer address, RC_Pointer buffer, int offset, int size)
{
	buffer = reinterpret_cast<RC_Pointer>(reinterpret_cast<uintptr_t>(buffer) + offset);

	WriteRemoteMemory_Params* Params = static_cast<WriteRemoteMemory_Params*>(GetSharedMemoryParamSpace());

	Params->InNumBytesToWrite = size;
	Params->InVirtualAddress = address;
	memcpy(Params->InBuffer, buffer, std::min(Params->OutNumBytesWritten, RC_Size(size)));

	SendCommandInSharedMemory(ECommandType::WriteRemoteMemory);

	return Params->OutNumBytesWritten == size;
}
