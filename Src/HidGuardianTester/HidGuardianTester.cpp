// HidGuardianTester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <SetupAPI.h>
#include <hidsdi.h>

#pragma comment(lib, "hid.lib")

typedef void (WINAPI* HidGuardianOpen_t)();
typedef void (WINAPI* HidGuardianClose_t)();


#include <pshpack1.h>
#include <time.h>
#include <vector>
#include <thread>

typedef struct _HIDGUARDIAN_GET_CREATE_REQUEST
{
    //
    // Arbitrary value to match request and response
    // 
    IN ULONG RequestId;

    //
    // ID of the process this request is related to
    // 
    OUT ULONG ProcessId;

    //
    // Index of device this request belongs to (zero-based)
    // 
    OUT ULONG DeviceIndex;

    //
    // Buffer containing Hardware ID string
    // 
    OUT PWSTR HardwareIdBuffer;

    //
    // Size of buffer for Hardware ID
    // 
    OUT ULONG HardwareIdBufferLength;

} HIDGUARDIAN_GET_CREATE_REQUEST, *PHIDGUARDIAN_GET_CREATE_REQUEST;

typedef struct _HIDGUARDIAN_SET_CREATE_REQUEST
{
    //
    // Arbitrary value to match request and response
    // 
    IN ULONG RequestId;

    //
    // Index of device this request belongs to (zero-based)
    // 
    IN ULONG DeviceIndex;

    //
    // TRUE if WdfRequestTypeCreate is allowed, FALSE otherwise
    // 
    IN BOOLEAN IsAllowed;

} HIDGUARDIAN_SET_CREATE_REQUEST, *PHIDGUARDIAN_SET_CREATE_REQUEST;

#include <poppack.h>

ULONG IOCTL_HIDGUARDIAN_GET_CREATE_REQUEST = 0x8000E004;
ULONG IOCTL_HIDGUARDIAN_SET_CREATE_REQUEST = 0x8000A008;
HANDLE hDevice = INVALID_HANDLE_VALUE;

void work()
{
    while (TRUE) {
        printf("Started thread\n");

        DWORD returned = 0;
        DWORD transfered = 0;
        OVERLAPPED lOverlapped = { 0 };
        lOverlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        HIDGUARDIAN_GET_CREATE_REQUEST cr;
        ZeroMemory(&cr, sizeof(HIDGUARDIAN_GET_CREATE_REQUEST));

        cr.RequestId = rand() % (4096 - 1024 + 1) + 1024;
        cr.HardwareIdBufferLength = 512;
        cr.HardwareIdBuffer = (PWSTR)malloc(cr.HardwareIdBufferLength);

        DeviceIoControl(hDevice,
            IOCTL_HIDGUARDIAN_GET_CREATE_REQUEST,
            (LPVOID)&cr,
            sizeof(HIDGUARDIAN_GET_CREATE_REQUEST),
            (LPVOID)&cr,
            sizeof(HIDGUARDIAN_GET_CREATE_REQUEST),
            &returned,
            &lOverlapped);

        GetOverlappedResult(hDevice, &lOverlapped, &transfered, TRUE);

        printf("DeviceIndex = %d\n", cr.DeviceIndex);
        printf("ProcessId = %d\n", cr.ProcessId);
        printf("HardwareId = %s\n", cr.HardwareIdBuffer);

        free(cr.HardwareIdBuffer);

        HIDGUARDIAN_SET_CREATE_REQUEST sr;
        ZeroMemory(&sr, sizeof(HIDGUARDIAN_SET_CREATE_REQUEST));

        sr.RequestId = cr.RequestId;
        sr.DeviceIndex = cr.DeviceIndex;
        sr.IsAllowed = TRUE;

        DeviceIoControl(hDevice,
            IOCTL_HIDGUARDIAN_SET_CREATE_REQUEST,
            (LPVOID)&sr,
            sizeof(HIDGUARDIAN_SET_CREATE_REQUEST),
            nullptr,
            0,
            &returned,
            &lOverlapped);

        GetOverlappedResult(hDevice, &lOverlapped, &transfered, TRUE);

        printf("Sent SET request\n");
    }
}

int main()
{
    // device found, open it
    hDevice = CreateFile(L"\\\\.\\HidGuardian",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_OVERLAPPED,
        nullptr);

    printf("hDevice = 0x%p, error = %d\n", hDevice, GetLastError());

    srand(time(NULL));

    std::vector<std::thread> threads;

    for (int i = 0; i < 20; ++i) {
        threads.push_back(std::thread(work));
    }

    getchar();

    return 0;


    SP_DEVICE_INTERFACE_DATA deviceInterfaceData = { 0 };
    deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
    DWORD memberIndex = 0;
    DWORD requiredSize = 0;

    GUID hidClass;
    HidD_GetHidGuid(&hidClass);

    HMODULE cerberus = LoadLibrary(L"HidCerberus.Lib.dll");
    HidGuardianOpen_t fpOpen = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianOpen"));
    HidGuardianOpen_t fpClose = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianClose"));

    if (!fpOpen || !fpClose) {
        printf("Exports not found, is HidCerberus.Lib.dll in my directory?\n");
        return -1;
    }

    fpOpen();

    auto deviceInfoSet = SetupDiGetClassDevs(&hidClass, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    while (SetupDiEnumDeviceInterfaces(deviceInfoSet, nullptr, &hidClass, memberIndex++, &deviceInterfaceData))
    {
        printf("Found one!\n");

        // get required target buffer size
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, nullptr, 0, &requiredSize, nullptr);

        // allocate target buffer
        auto detailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(requiredSize));
        detailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        // get detail buffer
        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, detailDataBuffer, requiredSize, &requiredSize, nullptr))
        {
            SetupDiDestroyDeviceInfoList(deviceInfoSet);
            free(detailDataBuffer);

            continue;
        }

        printf("%ls\n", detailDataBuffer->DevicePath);

        if (hDevice != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hDevice);
        }

        // device found, open it
        hDevice = CreateFile(detailDataBuffer->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            nullptr);

        printf("hDevice = 0x%p, error = %d\n", hDevice, GetLastError());

        free(detailDataBuffer);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    printf("Done\n");
    getchar();

    fpClose();

    return 0;
}

