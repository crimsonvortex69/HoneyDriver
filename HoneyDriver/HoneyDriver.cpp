#include <ntddk.h>
#include <wdf.h>

#define HONEY_DRIVER_DEVICE_NAME L"\\Device\\HoneyDriver"
#define HONEY_DRIVER_SYMBOLIC_NAME L"\\DosDevices\\HoneyDriver"

extern "C"
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WDF_DRIVER_CONFIG config;
    WDFDRIVER driver;
    PWDFDEVICE_INIT deviceInit = nullptr;
    WDFDEVICE device;
    UNICODE_STRING deviceName;
    UNICODE_STRING symbolicName;
    NTSTATUS status;

    KdPrintEx((
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_INFO_LEVEL,
        "HoneyDriver: DriverEntry started.\n"
        ));

    //
    // Configure a basic KMDF driver.
    //
    WDF_DRIVER_CONFIG_INIT(&config, WDF_NO_EVENT_CALLBACK);

    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        &driver
    );

    if (!NT_SUCCESS(status))
    {
        KdPrintEx((
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "HoneyDriver: WdfDriverCreate failed: 0x%08X\n",
            status
            ));

        return status;
    }

    //
    // Allocate initialization structure for a control device.
    //
    deviceInit = WdfControlDeviceInitAllocate(
        driver,
        &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RW_RES_R
    );

    if (deviceInit == nullptr)
    {
        KdPrintEx((
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "HoneyDriver: WdfControlDeviceInitAllocate failed.\n"
            ));

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Give the decoy device a kernel-mode name.
    //
    RtlInitUnicodeString(
        &deviceName,
        HONEY_DRIVER_DEVICE_NAME
    );

    status = WdfDeviceInitAssignName(
        deviceInit,
        &deviceName
    );

    if (!NT_SUCCESS(status))
    {
        WdfDeviceInitFree(deviceInit);

        KdPrintEx((
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "HoneyDriver: WdfDeviceInitAssignName failed: 0x%08X\n",
            status
            ));

        return status;
    }

    //
    // Create the device object.
    //
    status = WdfDeviceCreate(
        &deviceInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &device
    );

    if (!NT_SUCCESS(status))
    {
        KdPrintEx((
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "HoneyDriver: WdfDeviceCreate failed: 0x%08X\n",
            status
            ));

        return status;
    }

    //
    // Create a DOS-visible symbolic link.
    // User-mode programs will eventually open:
    //
    //     \\.\\HoneyDriver
    //
    RtlInitUnicodeString(
        &symbolicName,
        HONEY_DRIVER_SYMBOLIC_NAME
    );

    status = WdfDeviceCreateSymbolicLink(
        device,
        &symbolicName
    );

    if (!NT_SUCCESS(status))
    {
        KdPrintEx((
            DPFLTR_IHVDRIVER_ID,
            DPFLTR_ERROR_LEVEL,
            "HoneyDriver: WdfDeviceCreateSymbolicLink failed: 0x%08X\n",
            status
            ));

        return status;
    }

    //
    // Finish initialization of the control device.
    //
    WdfControlFinishInitializing(device);

    KdPrintEx((
        DPFLTR_IHVDRIVER_ID,
        DPFLTR_INFO_LEVEL,
        "HoneyDriver: Decoy device initialized successfully.\n"
        ));

    return STATUS_SUCCESS;
}