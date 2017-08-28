#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ViGEmClient.h>
#include <cstdio>
#include <stdlib.h>


//
// Example implementation of PVIGEM_X360_NOTIFICATION callback
// 
VOID my_x360_callback(
    PVIGEM_CLIENT Client,
    PVIGEM_TARGET Target,
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    UCHAR LedNumber)
{
    printf("X360 Response - Serial: %d, LM: %d, SM: %d, LED: %d\n",
        vigem_target_get_index(Target),
        LargeMotor,
        SmallMotor,
        LedNumber);

    vigem_target_x360_unregister_notification(Target);
}

//
// Example implementation of PVIGEM_DS4_NOTIFICATION callback
// 
VOID my_ds4_callback(
    PVIGEM_CLIENT Client,
    PVIGEM_TARGET Target,
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    DS4_LIGHTBAR_COLOR LightbarColor)
{
    printf("DS4 Response - Serial: %d, LM: %d, SM: %d, R: %d, G: %d, B: %d\n",
        vigem_target_get_index(Target),
        LargeMotor,
        SmallMotor,
        LightbarColor.Red,
        LightbarColor.Green,
        LightbarColor.Blue);
}

//
// Example implementation of PVIGEM_TARGET_ADD_RESULT callback
// 
VOID my_target_add_callback(
    PVIGEM_CLIENT Client,
    PVIGEM_TARGET Target,
    VIGEM_ERROR Result)
{
    printf("Target with type %d and serial %d is live with result %d\n",
        vigem_target_get_type(Target),
        vigem_target_get_index(Target), Result);

    if (!VIGEM_SUCCESS(Result))
    {
        printf("Device plugin didn't work!\n");
        return;
    }

    //
    // Register PVIGEM_DS4_NOTIFICATION on success
    // 
    auto ret = vigem_target_ds4_register_notification(Client, Target, reinterpret_cast<PVIGEM_DS4_NOTIFICATION>(my_ds4_callback));
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Couldn't add DS4 notification callback\n");
    }

    DS4_REPORT report;
    DS4_REPORT_INIT(&report);

    while (1)
    {
        //
        // Generate random button and axis values
        // 
        report.bSpecial = rand() | ((rand() & 1) << 15);
        report.wButtons = rand() | ((rand() & 1) << 15);
        report.bThumbLX++;
        report.bThumbLY++;
        report.bThumbRX++;
        report.bThumbRY++;
        report.bTriggerL++;
        report.bTriggerR++;

        //
        // Submit report to device
        // 
        ret = vigem_target_ds4_update(Client, Target, report);
        if (!VIGEM_SUCCESS(ret))
        {
            printf("Couldn't submit report update");
            return;
        }

        Sleep(10);
    }
}

void main()
{
    //
    // Allocate driver connection object
    // 
    auto driver = vigem_alloc();

    //
    // Attempt to connect to bus driver
    // 
    auto ret = vigem_connect(driver);
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Couldn't connect to driver");
        return;
    }

    //
    // Allocate target device object of type Xbox 360 Controller
    // 
    auto x360 = vigem_target_x360_alloc();

    //
    // Add new Xbox 360 device to bus.
    // 
    // This call blocks until the device reached working state 
    // or an error occurred.
    // 
    ret = vigem_target_add(driver, x360);
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Couldn't add virtual X360 device");
        return;
    }

    //
    // Register a PVIGEM_X360_NOTIFICATION callback
    // 
    ret = vigem_target_x360_register_notification(driver, x360, reinterpret_cast<PVIGEM_X360_NOTIFICATION>(my_x360_callback));
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Couldn't add X360 notification callback");
        return;
    }

    //
    // Allocate target device object of type DualShock 4
    // 
    auto ds4 = vigem_target_ds4_alloc();

    //
    // Add new DualShock 4 device to bus.
    // 
    // This call immediately returns. If a callback is provided,
    // this callback will be invoked when the device reached 
    // working state or an error occurred.
    // 
    ret = vigem_target_add_async(driver, ds4, reinterpret_cast<PVIGEM_TARGET_ADD_RESULT>(my_target_add_callback));
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Async plugin failed");
        return;
    }

    getchar();

    XUSB_REPORT report;
    XUSB_REPORT_INIT(&report);

    while (1)
    {
        //
        // Generate random button and axis values
        // 
        report.wButtons = rand() | ((rand() & 1) << 15); //rand() | rand() << 15 | rand() % 4 << 30;
        report.bLeftTrigger++;
        report.bRightTrigger++;
        report.sThumbLX++;
        report.sThumbLY++;
        report.sThumbRX++;
        report.sThumbRY++;

        //
        // Submit report to device
        // 
        ret = vigem_target_x360_update(driver, x360, report);
        if (!VIGEM_SUCCESS(ret))
        {
            printf("Couldn't submit report update");
            return;
        }

        Sleep(10);
    }
}
