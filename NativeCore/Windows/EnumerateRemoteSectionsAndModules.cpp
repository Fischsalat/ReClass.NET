#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <functional>

#include "NativeCore.hpp"
#include "CommandParameters.hpp"
#include "Communication.hpp"

PPEB GetRemotePeb(const HANDLE process)
{
	GetRemotePEB_Params* Params = static_cast<GetRemotePEB_Params*>(GetSharedMemoryParamSpace());

	SendCommandInSharedMemory(ECommandType::GetRemotePEB);

	return reinterpret_cast<PPEB>(Params->OutPEB);
}

using InternalEnumerateRemoteModulesCallback = std::function<void(EnumerateRemoteModuleData&)>;

bool EnumerateRemoteModulesNative(const RC_Pointer process, const InternalEnumerateRemoteModulesCallback& callback)
{
	GetRemoteModules_Params* Params = static_cast<GetRemoteModules_Params*>(GetSharedMemoryParamSpace());

	SendCommandInSharedMemory(ECommandType::GetRemoteModules);

	for (int i = 0; i < Params->OutNumModules; i++)
	{
		callback(Params->OutModuleInfoBuffer[i]);
	}

	return Params->OutNumModules != 0;
}

bool EnumerateRemoteModulesWinapi(const RC_Pointer process, const InternalEnumerateRemoteModulesCallback& callback)
{
	return true;
}

void RC_CallConv EnumerateRemoteSectionsAndModules(RC_Pointer process, EnumerateRemoteSectionsCallback callbackSection, EnumerateRemoteModulesCallback callbackModule)
{
	if (callbackSection == nullptr && callbackModule == nullptr)
	{
		return;
	}

	GetRemoteSections_Params* Params = static_cast<GetRemoteSections_Params*>(GetSharedMemoryParamSpace());
	
	SendCommandInSharedMemory(ECommandType::GetRemotePEB);


	std::vector<EnumerateRemoteSectionData> sections;

	for (int i = 0; i < Params->OutNumSections; i++)
	{
		sections.push_back(Params->OutSectionInfoBuffer[i]);
	}

	const auto moduleEnumerator = [&](EnumerateRemoteModuleData& data)
	{
		if (callbackModule != nullptr)
		{
			callbackModule(&data);
		}

		if (callbackSection != nullptr)
		{
			auto it = std::lower_bound(std::begin(sections), std::end(sections), static_cast<LPVOID>(data.BaseAddress), [&sections](const auto& lhs, const LPVOID& rhs)
			{
				return lhs.BaseAddress < rhs;
			});

			IMAGE_DOS_HEADER imageDosHeader = {};
			IMAGE_NT_HEADERS imageNtHeaders = {};

			if (!ReadRemoteMemory(process, data.BaseAddress, &imageDosHeader, 0, sizeof(IMAGE_DOS_HEADER))
				|| !ReadRemoteMemory(process, PUCHAR(data.BaseAddress) + imageDosHeader.e_lfanew, &imageNtHeaders, 0, sizeof(IMAGE_NT_HEADERS)))
			{
				return;
			}

			std::vector<IMAGE_SECTION_HEADER> sectionHeaders(imageNtHeaders.FileHeader.NumberOfSections);
			ReadRemoteMemory(process, PUCHAR(data.BaseAddress) + imageDosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS), sectionHeaders.data(), 0, imageNtHeaders.FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
			for (auto&& sectionHeader : sectionHeaders)
			{
				const auto sectionAddress = reinterpret_cast<size_t>(data.BaseAddress) + sectionHeader.VirtualAddress;

				for (; it != std::end(sections); ++it)
				{
					auto&& section = *it;
					
					if (sectionAddress >= reinterpret_cast<size_t>(section.BaseAddress) 
						&& sectionAddress < reinterpret_cast<size_t>(section.BaseAddress) + static_cast<size_t>(section.Size)
						&& sectionHeader.VirtualAddress + sectionHeader.Misc.VirtualSize <= data.Size)
					{
						if ((sectionHeader.Characteristics & IMAGE_SCN_CNT_CODE) == IMAGE_SCN_CNT_CODE)
						{
							section.Category = SectionCategory::CODE;
						}
						else if (sectionHeader.Characteristics & (IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_CNT_UNINITIALIZED_DATA))
						{
							section.Category = SectionCategory::DATA;
						}

						try
						{
							// Copy the name because it is not null padded.
							char buffer[IMAGE_SIZEOF_SHORT_NAME + 1] = { 0 };
							std::memcpy(buffer, sectionHeader.Name, IMAGE_SIZEOF_SHORT_NAME);
							MultiByteToUnicode(buffer, section.Name, IMAGE_SIZEOF_SHORT_NAME);
						}
						catch (std::range_error &)
						{
							std::memset(section.Name, 0, sizeof(section.Name));
						}
						std::memcpy(section.ModulePath, data.Path, std::min(MAX_PATH, PATH_MAXIMUM_LENGTH));

						break;
					}
				}
			}
		}
	};
	
	if (!EnumerateRemoteModulesNative(process, moduleEnumerator))
	{
		EnumerateRemoteModulesWinapi(process, moduleEnumerator);
	}

	if (callbackSection != nullptr)
	{
		for (auto&& section : sections)
		{
			callbackSection(&section);
		}
	}
}
