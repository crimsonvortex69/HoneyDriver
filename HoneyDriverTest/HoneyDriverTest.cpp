#include <windows.h>
#include <iostream>

#define IOCTL_HONEY_PING \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

int main()
{
    std::cout << "HoneyDriver IOCTL Test\n";
    std::cout << "======================\n";

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
        std::cout << "FAILED: Could not open HoneyDriver.\n";
        std::cout << "Error code: " << GetLastError() << "\n";
        return 1;
    }

    std::cout << "[+] HoneyDriver device opened successfully.\n";

    char outputBuffer[16] = {};
    DWORD bytesReturned = 0;

    BOOL result = DeviceIoControl(
        hDevice,
        IOCTL_HONEY_PING,
        nullptr,
        0,
        outputBuffer,
        sizeof(outputBuffer),
        &bytesReturned,
        nullptr
    );

    if (!result)
    {
        std::cout << "[-] IOCTL_HONEY_PING failed.\n";
        std::cout << "Error code: " << GetLastError() << "\n";

        CloseHandle(hDevice);
        return 1;
    }

    std::cout << "[+] IOCTL_HONEY_PING succeeded.\n";
    std::cout << "[+] Response: " << outputBuffer << "\n";
    std::cout << "[+] Bytes returned: " << bytesReturned << "\n";

    CloseHandle(hDevice);

    return 0;
}