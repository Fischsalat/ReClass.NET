#pragma once

#include "NativeCore.hpp"

constexpr auto MaxNumSectionsInBuffer = 0x20;
constexpr auto MaxNumModulesInBuffer = 0x20;

constexpr auto MaxNumBytesToRead = 0x10000;
constexpr auto MaxNumBytesToWrite = 0x10000;

enum class ECommandType
{
	InvalidCommand,

	GetRemotePEB,
	GetRemoteModules,
	GetRemoteSections,

	ControlRemoteProcess,

	ReadRemoteMemory,
	WriteRemoteMemory,
};

struct ParamHeader
{
	ECommandType Type;
	uint8_t Pad[0xF];
};


struct GetRemotePEB_Params
{
	RC_Pointer OutPEB;
};

struct GetRemoteModules_Params
{
	RC_Size OutNumModules;
	EnumerateRemoteModuleData OutModuleInfoBuffer[MaxNumModulesInBuffer];
};

struct GetRemoteSections_Params
{
	RC_Size OutNumSections;
	EnumerateRemoteSectionData OutSectionInfoBuffer[MaxNumSectionsInBuffer];
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
