#include "swg_string.h"

namespace swg
{
constexpr int defaultInititalSize = 8; // 8 Is the default size in STLPort

WString::WString()
{
    begin = StringAllocator<char16_t>::allocate(defaultInititalSize);
    end = begin;
    allocEnd = end + defaultInititalSize;
}

WString::WString(const size_t size)
{
    begin = StringAllocator<char16_t>::allocate(size);
    end = begin;
    allocEnd = end + size;
}

WString::WString(const WString& value)
{
    create<char16_t>(value.begin);
}

WString::WString(const wchar_t* value)
{
    create<wchar_t>(value);
}

WString::WString(const char16_t* value)
{
    create<char16_t>(value);
}

WString::WString(const char* value)
{
    create<char>(value);
}

WString::~WString()
{
    if (begin != nullptr)
    {
        StringAllocator<char16_t>::deallocate(begin, allocEnd - begin);
    }
}

bool WString::operator==(const WString& rhs) const
{
    return compare<char16_t>(rhs.begin);
}

bool WString::operator==(const wchar_t* rhs) const
{
    return compare<wchar_t>(rhs);
}

bool WString::operator==(const char16_t* rhs) const
{
    return compare<char16_t>(rhs);
}

bool WString::operator==(const char* rhs) const
{
    return compare<char>(rhs);
}


}