#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ViGEmClient.h>
#include <cstdio>
#include <stdlib.h>


VOID my_x360_callback(
    ULONG Index,
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    UCHAR LedNumber)
{
    printf("X360 Response - Serial: %d, LM: %d, SM: %d, LED: %d\n",
        Index,
        LargeMotor,
        SmallMotor,
        LedNumber);
}

VOID my_ds4_callback(
    ULONG Index,
    UCHAR LargeMotor,
    UCHAR SmallMotor,
    DS4_LIGHTBAR_COLOR LightbarColor)
{
    printf("DS4 Response - Serial: %d, LM: %d, SM: %d, R: %d, G: %d, B: %d\n",
        Index,
        LargeMotor,
        SmallMotor,
        LightbarColor.Red,
        LightbarColor.Green,
        LightbarColor.Blue);
}

void main()
{
    auto driver = vigem_alloc();

    auto ret = vigem_connect(driver);
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Couldn't connect to driver");
        return;
    }

    auto x360 = vigem_target_x360_alloc();

    ret = vigem_target_add(driver, x360);
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Couldn't add virtual X360 device");
        return;
    }

    ret = vigem_target_x360_register_notification(driver, x360, reinterpret_cast<PVIGEM_X360_NOTIFICATION>(my_x360_callback));
    if (!VIGEM_SUCCESS(ret))
    {
        printf("Couldn't add notification callback");
        return;
    }

    XUSB_REPORT report;
    XUSB_REPORT_INIT(&report);

    while(1)
    {
        report.wButtons= rand() | ((rand() & 1) << 15); //rand() | rand() << 15 | rand() % 4 << 30;
        report.bLeftTrigger++;
        report.bRightTrigger++;
        report.sThumbLX++;
        report.sThumbLY++;
        report.sThumbRX++;
        report.sThumbRY++;

        ret = vigem_target_x360_update(driver, x360, report);
        if (!VIGEM_SUCCESS(ret))
        {
            printf("Couldn't submit report update");
            return;
        }

        Sleep(10);
    }
}
