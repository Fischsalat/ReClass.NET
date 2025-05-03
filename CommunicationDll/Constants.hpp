#pragma once


// Shared memory and event related constants
constexpr auto SharedMemoryName = "Local\\ReclassCommSharedMem";
constexpr auto SharedMemorySize = 0x800000;

constexpr auto NewCommandAvailableEventName = "Local\\SigReclassCmdAvailable";
constexpr auto CommandProcessingFinishedEventName = "Local\\SigReclassCmdFinished";


// Command related constants
constexpr auto ReadWriteMemoryPacketSizeWithoutBuffer = 0x30;

constexpr auto MaxNumSectionsInBuffer = 0x2A00;
constexpr auto MaxNumModulesInBuffer = 0x100;

constexpr auto MaxNumBytesToRead = SharedMemorySize - ReadWriteMemoryPacketSizeWithoutBuffer;
constexpr auto MaxNumBytesToWrite = SharedMemorySize - ReadWriteMemoryPacketSizeWithoutBuffer;
