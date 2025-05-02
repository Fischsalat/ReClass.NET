#pragma once
#include "CommandParameters.hpp"

// Returns the PEB
void GetRemotePEB(GetRemotePEB_Params* Params);

// Returns a list of all modules loaded by the exe
void GetRemoteModules(GetRemoteModules_Params* Params);

// Returns a list of all sections in this process
void GetRemoteSections(GetRemoteSections_Params* Params);

// Suspends, resumes or terminates this process
void ControlRemoteProcess(ControlRemoteProcess_Params* Params);

// Reads memory from this process
void ReadRemoteMemory(ReadRemoteMemory_Params* Params);

// Writes memory in this process
void WriteRemoteMemory(WriteRemoteMemory_Params* Params);