#include "utility.h"

namespace utility
{
void showMessageBox(const std::string& title, const std::string& text)
{
    MessageBoxA(nullptr, text.c_str(), title.c_str(), MB_OK);
}

void showMessageBox(const std::string& text)
{
    MessageBoxA(nullptr, text.c_str(), "", MB_OK);
}

void showMessageBoxEmpty()
{
    MessageBoxA(nullptr, "", "", MB_OK);
}

void showLastErrorMessageBox()
{
    DWORD errorMessageId = GetLastError();

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorMessageId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

    std::string message(messageBuffer, size);

    LocalFree(messageBuffer);
    MessageBox(0, message.c_str(), "Error", MB_OK);
}

std::string ws2s(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
    return str;
}
}