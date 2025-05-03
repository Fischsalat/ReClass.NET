#pragma once
#include <Windows.h>
#include <iostream>

// Intermeidate buffer for command-parameters
constexpr auto SharedMemoryName = "Local\\ReclassCommSharedMem";
constexpr auto SharedMemorySize = 0x10000;

inline HANDLE SharedMemoryHandle = nullptr;
inline void* SharedMemoryAddress = nullptr;


// Triggered by reclass to signal the availability of a new command that needs to be processed
constexpr auto NewCommandAvailableEventName = "Local\\SigReclassCmdAvailable";
inline HANDLE CommandAvailableEvent = nullptr;

// Triggered by the dll to signal that the command was processed
constexpr auto CommandProcessingFinishedEventName = "Local\\SigReclassCmdFinished";
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

void DeleteSharedMemory()
{
	UnmapViewOfFile(SharedMemoryAddress);
	CloseHandle(SharedMemoryHandle);
}

bool SetupCommunicationEvents()
{
	CommandAvailableEvent = CreateEventA(NULL, FALSE, FALSE, NewCommandAvailableEventName);
	CommandFinishedEvent = CreateEventA(NULL, FALSE, FALSE, CommandProcessingFinishedEventName);

	return CommandAvailableEvent && CommandFinishedEvent;
}

void DeleteCommunicationEvents()
{
	CloseHandle(CommandAvailableEvent);
	CloseHandle(CommandFinishedEvent);
}


void WaitForNewCommand()
{
	WaitForSingleObject(CommandAvailableEvent, INFINITE);
}

void SignalCommandProcessed()
{
	SetEvent(CommandFinishedEvent);
}