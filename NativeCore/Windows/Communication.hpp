#pragma once
#include <Windows.h>
#include <iostream>

#include "Constants.hpp"

// Intermeidate buffer for command-parameters
inline HANDLE SharedMemoryHandle = nullptr;

inline void* SharedMemoryAddress = nullptr;


// Triggered by reclass to signal the availability of a new command that needs to be processed
inline HANDLE CommandAvailableEvent = nullptr;

// Triggered by the dll to signal that the command was processed
inline HANDLE CommandFinishedEvent = nullptr;



static bool SetupSharedMemory()
{
	SharedMemoryHandle = OpenFileMappingA(
		FILE_MAP_ALL_ACCESS,  // Desired access
		FALSE,                // Do not inherit
		SharedMemoryName	  // Name of mapping
	);


	if (SharedMemoryHandle == nullptr)
	{
		SharedMemoryHandle = CreateFileMappingA(
			INVALID_HANDLE_VALUE,    // Use paging file
			NULL,                    // Default security
			PAGE_READWRITE,          // Read/write access
			0,                       // Max size (high)
			SharedMemorySize,        // Max size (low)
			SharedMemoryName         // Name of mapping
		);

		if (SharedMemoryHandle == nullptr)
		{
			printf("CreateFileMapping failed: %lu\n", GetLastError());
			return false;
		}
	}

	// Map view of the file into the address space
	SharedMemoryAddress = MapViewOfFile(
		SharedMemoryHandle,  // Handle to map object
		FILE_MAP_ALL_ACCESS, // Read/write permission
		0,
		0,
		SharedMemorySize
	);

	return SharedMemoryAddress != nullptr;
}

static void DeleteSharedMemory()
{
	UnmapViewOfFile(SharedMemoryAddress);
	CloseHandle(SharedMemoryHandle);
}

static bool SetupCommunicationEvents()
{
	CommandAvailableEvent = CreateEventA(NULL, FALSE, FALSE, NewCommandAvailableEventName);
	CommandFinishedEvent = CreateEventA(NULL, FALSE, FALSE, CommandProcessingFinishedEventName);

	return CommandAvailableEvent && CommandFinishedEvent;
}

static void DeleteCommunicationEvents()
{
	CloseHandle(CommandAvailableEvent);
	CloseHandle(CommandFinishedEvent);
}


static void WaitForCommandProcessed()
{
	WaitForSingleObject(CommandFinishedEvent, INFINITE);
}

static void SignalNewCommandAvailable()
{
	SetEvent(CommandAvailableEvent);
}

static void* GetSharedMemoryParamSpace()
{
	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(SharedMemoryAddress) + sizeof(ParamHeader));
}

static void SendCommandInSharedMemory(ECommandType Type)
{
	ParamHeader& Header = *reinterpret_cast<ParamHeader*>(SharedMemoryAddress);

	Header.Type = Type;
	
	// Let the dll know there's a new command to execute
	SignalNewCommandAvailable();

	// Wait until the command was executed and the result is available
	WaitForCommandProcessed();
}