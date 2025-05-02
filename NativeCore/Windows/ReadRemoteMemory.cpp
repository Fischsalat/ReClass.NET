#include <windows.h>

#include "NativeCore.hpp"
#include "CommandParameters.hpp"
#include "Communication.hpp"

bool RC_CallConv ReadRemoteMemory(RC_Pointer handle, RC_Pointer address, RC_Pointer buffer, int offset, int size)
{
	buffer = reinterpret_cast<RC_Pointer>(reinterpret_cast<uintptr_t>(buffer) + offset);

	ReadRemoteMemory_Params* Params = static_cast<ReadRemoteMemory_Params*>(GetSharedMemoryParamSpace());

	Params->InNumBytesToRead = size;
	Params->InVirtualAddress = address;

	SendCommandInSharedMemory(ECommandType::ReadRemoteMemory);

	memcpy(buffer, Params->OutBuffer, size);

	return Params->OutNumBytesRead == size;
}
