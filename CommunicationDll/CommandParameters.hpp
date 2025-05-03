#pragma once

#include "ReclassTypes.hpp"

#include "Constants.hpp"

enum class ECommandType : uint8_t
{
	InvalidCommand,

	GetRemotePEB,
	GetRemoteModules,
	GetRemoteSections,

	ControlRemoteProcess,

	ReadRemoteMemory,
	WriteRemoteMemory,
	GetCurrentProcessInfo,
};

struct ParamHeader
{
	ECommandType Type;
	uint8_t Pad[0x10 - sizeof(ECommandType)];
};
static_assert(sizeof(ParamHeader) == 0x10);


struct GetRemotePEB_Params
{
	RC_Pointer OutPEB;
};

struct GetRemoteModules_Params
{
	RC_Size OutNumModules;
	RemoteModuleData OutModuleInfoBuffer[MaxNumModulesInBuffer];
};

struct GetRemoteSections_Params
{
	RC_Size OutNumSections;
	RemoteSectionData OutSectionInfoBuffer[MaxNumSectionsInBuffer];
};

struct ControlRemoteProcess_Params
{
	ControlRemoteProcessAction InRemoteAction;

	bool OutWasSuccessfull;
};

struct ReadRemoteMemory_Params
{
	RC_Pointer InVirtualAddress;
	RC_Size InNumBytesToRead;

	RC_Size OutNumBytesRead;
	uint8_t OutBuffer[0x1];
};

struct WriteRemoteMemory_Params
{
	RC_Size OutNumBytesWritten;

	RC_Pointer InVirtualAddress;
	RC_Size InNumBytesToWrite;
	uint8_t InBuffer[0x1]; 
};

struct GetCurrentProcessInfo_Params
{
	EnumerateProcessData OutCurrentProcessData;
};
