#pragma once
#include <cstdint>
#include <type_traits>

using RC_Pointer = void*;
using RC_Size = size_t;
using RC_UnicodeChar = char16_t;

// Constants

const int PATH_MAXIMUM_LENGTH = 260;


enum class SectionType
{
	Unknown,

	Private,
	Mapped,
	Image
};

enum class SectionCategory
{
	Unknown,
	CODE,
	DATA,
	HEAP
};

enum class ControlRemoteProcessAction
{
	Suspend,
	Resume,
	Terminate
};

enum class SectionProtection
{
	NoAccess = 0,

	Read = 1,
	Write = 2,
	CopyOnWrite = 4,
	Execute = 8,

	Guard = 16
};


inline SectionProtection operator|(SectionProtection lhs, SectionProtection rhs)
{
	using T = std::underlying_type_t<SectionProtection>;

	return static_cast<SectionProtection>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline SectionProtection& operator|=(SectionProtection& lhs, SectionProtection rhs)
{
	using T = std::underlying_type_t<SectionProtection>;

	lhs = static_cast<SectionProtection>(static_cast<T>(lhs) | static_cast<T>(rhs));

	return lhs;
}


struct RemoteModuleData
{
	RC_Pointer BaseAddress;
	RC_Size Size;
	RC_UnicodeChar Path[PATH_MAXIMUM_LENGTH];
};

struct RemoteSectionData
{
	RC_Pointer BaseAddress;
	RC_Size Size;
	SectionType Type;
	SectionCategory Category;
	SectionProtection Protection;
	RC_UnicodeChar Name[16];
	RC_UnicodeChar ModulePath[PATH_MAXIMUM_LENGTH];
};

constexpr int a = sizeof(RemoteSectionData); 

struct EnumerateProcessData
{
	RC_Size Id;
	RC_UnicodeChar Name[PATH_MAXIMUM_LENGTH];
	RC_UnicodeChar Path[PATH_MAXIMUM_LENGTH];
};
