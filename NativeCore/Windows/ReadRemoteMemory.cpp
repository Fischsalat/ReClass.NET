#include <windows.h>
#include <algorithm>

#include "NativeCore.hpp"
#include "CommandParameters.hpp"
#include "Communication.hpp"

bool RC_CallConv ReadRemoteMemory(RC_Pointer handle, RC_Pointer address, RC_Pointer buffer, int offset, int size)
{
	buffer = reinterpret_cast<RC_Pointer>(reinterpret_cast<uintptr_t>(buffer) + offset);

	ReadRemoteMemory_Params* Params = static_cast<ReadRemoteMemory_Params*>(GetSharedMemoryParamSpace());

	printf("&Params->InVirtualAddress: %p\n", &Params->InVirtualAddress);
	printf("&Params: %p\n", Params);
	printf("VirtualAddress: %p, Size: 0x%X\n", address, size);
	Params->InNumBytesToRead = size;
	Params->InVirtualAddress = address;

	SendCommandInSharedMemory(ECommandType::ReadRemoteMemory);

	printf("Params->OutNumBytesRead: 0x%llX / 0x%X\n", Params->OutNumBytesRead, size);
	memcpy(buffer, Params->OutBuffer, std::min(Params->OutNumBytesRead, RC_Size(size)));

	return Params->OutNumBytesRead == size;
}
