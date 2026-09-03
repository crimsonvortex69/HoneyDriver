#include <windows.h>
#include <iostream>

int main()
{
    HANDLE hDevice = CreateFileW(
        L"\\\\.\\HoneyDriver",
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        std::cout << "FAILED to open HoneyDriver.\n";
        std::cout << "Error code: " << GetLastError() << "\n";
        return 1;
    }

    std::cout << "SUCCESS: HoneyDriver decoy device opened.\n";

    CloseHandle(hDevice);
    return 0;
}