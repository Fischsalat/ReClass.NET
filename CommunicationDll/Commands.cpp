#include "Commands.hpp"
#include "Utils.h"

void GetRemotePEB(GetRemotePEB_Params* Params)
{
	Params->OutPEB = GetPEB();
}

void GetRemoteModules(GetRemoteModules_Params* Params)
{
	Params->OutNumModules = 0;

	PEB* Peb = GetPEB();
	PEB_LDR_DATA* Ldr = Peb->Ldr;

	int NumEntriesLeft = Ldr->Length;

	for (LIST_ENTRY* P = Ldr->InMemoryOrderModuleList.Flink; P && NumEntriesLeft-- > 0; P = P->Flink)
	{
		if (Params->OutNumModules >= MaxNumModulesInBuffer)
		{
			break;
		}

		LDR_DATA_TABLE_ENTRY* Entry = reinterpret_cast<LDR_DATA_TABLE_ENTRY*>(P);

		std::wstring WideModuleName(Entry->BaseDllName.Buffer, Entry->BaseDllName.Length >> 1);
		std::string ModuleName = std::string(WideModuleName.begin(), WideModuleName.end());

		RemoteModuleData& ModuleInfo = Params->OutModuleInfoBuffer[Params->OutNumModules];
		Params->OutNumModules++;

		ModuleInfo.BaseAddress = Entry->DllBase;
		ModuleInfo.Size = Entry->SizeOfImage;
		wcscpy_s(reinterpret_cast<wchar_t*>(ModuleInfo.Path), PATH_MAXIMUM_LENGTH, Entry->BaseDllName.Buffer);
	}
}

void GetRemoteSections(GetRemoteSections_Params* Params)
{
	Params->OutNumSections = 0;

	MEMORY_BASIC_INFORMATION memory = { };
	memory.RegionSize = 0x1000;
	size_t address = 0;
	while (VirtualQuery(reinterpret_cast<LPCVOID>(address), &memory, sizeof(MEMORY_BASIC_INFORMATION)) != 0 && address + memory.RegionSize > address)
	{
		if (memory.State != MEM_COMMIT)
		{
			address = reinterpret_cast<size_t>(memory.BaseAddress) + memory.RegionSize;
			continue;
		}

		// We can't fit any more sections into the buffer
		if (Params->OutNumSections >= MaxNumSectionsInBuffer)
		{
			break;
		}

		RemoteSectionData& section = Params->OutSectionInfoBuffer[Params->OutNumSections];
		Params->OutNumSections++;

		section.BaseAddress = memory.BaseAddress;
		section.Size = memory.RegionSize;

		section.Protection = SectionProtection::NoAccess;
		if ((memory.Protect & PAGE_EXECUTE) == PAGE_EXECUTE) section.Protection |= SectionProtection::Execute;
		if ((memory.Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ) section.Protection |= SectionProtection::Execute | SectionProtection::Read;
		if ((memory.Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE) section.Protection |= SectionProtection::Execute | SectionProtection::Read | SectionProtection::Write;
		if ((memory.Protect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY) section.Protection |= SectionProtection::Execute | SectionProtection::Read | SectionProtection::CopyOnWrite;
		if ((memory.Protect & PAGE_READONLY) == PAGE_READONLY) section.Protection |= SectionProtection::Read;
		if ((memory.Protect & PAGE_READWRITE) == PAGE_READWRITE) section.Protection |= SectionProtection::Read | SectionProtection::Write;
		if ((memory.Protect & PAGE_WRITECOPY) == PAGE_WRITECOPY) section.Protection |= SectionProtection::Read | SectionProtection::CopyOnWrite;
		if ((memory.Protect & PAGE_GUARD) == PAGE_GUARD) section.Protection |= SectionProtection::Guard;

		switch (memory.Type)
		{
		case MEM_IMAGE:
			section.Type = SectionType::Image;
			break;
		case MEM_MAPPED:
			section.Type = SectionType::Mapped;
			break;
		case MEM_PRIVATE:
			section.Type = SectionType::Private;
			break;
		}

		section.Category = section.Type == SectionType::Private ? SectionCategory::HEAP : SectionCategory::Unknown;

		address = reinterpret_cast<size_t>(memory.BaseAddress) + memory.RegionSize;
	}
}

void ControlRemoteProcess(ControlRemoteProcess_Params* Params)
{

}

void ReadRemoteMemory(ReadRemoteMemory_Params* Params)
{
	Params->OutNumBytesRead = 0;

	if (Params->InNumBytesToRead > MaxNumBytesToRead)
		return;

	// Validate that the being and end addresses are in the process range
	if (IsBadReadPtr(Params->InVirtualAddress) || IsBadReadPtr(reinterpret_cast<uintptr_t>(Params->InVirtualAddress) + Params->InNumBytesToRead))
		return;

	memcpy(Params->OutBuffer, Params->InVirtualAddress, Params->InNumBytesToRead);
	Params->OutNumBytesRead = Params->InNumBytesToRead;
}

void WriteRemoteMemory(WriteRemoteMemory_Params* Params)
{
	Params->OutNumBytesWritten = 0;

	if (Params->InNumBytesToWrite > MaxNumBytesToWrite)
		return;

	// Validate that the being and end addresses are in the process range
	if (IsBadReadPtr(Params->InVirtualAddress) || IsBadReadPtr(reinterpret_cast<uintptr_t>(Params->InVirtualAddress) + Params->InNumBytesToWrite))
		return;

	memcpy(Params->InVirtualAddress, Params->InBuffer, Params->InNumBytesToWrite);
	Params->OutNumBytesWritten = Params->InNumBytesToWrite;
}