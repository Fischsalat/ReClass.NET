#pragma once

#include "ReclassTypes.hpp"

constexpr auto MaxNumSectionsInBuffer = 0x180;
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
	GetCurrentProcessInfo,
};

struct ParamHeader
{
	ECommandType Type;
	uint8_t Pad[0xF];
};


struct GetRemotePEB_Params : public ParamHeader
{
	RC_Pointer OutPEB;
};

struct GetRemoteModules_Params : public ParamHeader
{
	RC_Size OutNumModules;
	RemoteModuleData OutModuleInfoBuffer[MaxNumModulesInBuffer];
};

struct GetRemoteSections_Params : public ParamHeader
{
	RC_Size OutNumSections;
	RemoteSectionData OutSectionInfoBuffer[MaxNumSectionsInBuffer];
};

struct ControlRemoteProcess_Params : public ParamHeader
{
	ControlRemoteProcessAction InRemoteAction;

	bool OutWasSuccessfull;
};

struct ReadRemoteMemory_Params : public ParamHeader
{
	RC_Pointer InVirtualAddress;
	RC_Size InNumBytesToRead;

	RC_Size OutNumBytesRead;
	uint8_t OutBuffer[0x1];
};

struct WriteRemoteMemory_Params : public ParamHeader
{
	RC_Size OutNumBytesWritten;

	RC_Pointer InVirtualAddress;
	RC_Size InNumBytesToWrite;
	uint8_t InBuffer[0x1]; 
};

struct GetCurrentProcessInfo_Params : public ParamHeader
{
	EnumerateProcessData OutCurrentProcessData;
};
