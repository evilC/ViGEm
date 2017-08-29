/*
MIT License

Copyright (c) 2016 Benjamin "Nefarius" Höglinger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


// x360ce2ViGEm.cpp : Defines the entry point for the console application.
//

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <ViGEmClient.h>
#include <chrono>
#include <thread>

#pragma warning(disable: 4995)

typedef BOOL(WINAPI* HidGuardianOpen_t)();
typedef BOOL(WINAPI* HidGuardianClose_t)();

static HidGuardianOpen_t fpOpen;
static HidGuardianOpen_t fpClose;

BOOL WINAPI HandlerRoutine(
    _In_ DWORD dwCtrlType
);

static PVIGEM_TARGET targets[XUSER_MAX_COUNT];
static PVIGEM_CLIENT g_client = nullptr;

int main()
{
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds

    SetConsoleTitle(L"x360ce to ViGEm demo application");
    SetConsoleCtrlHandler(HandlerRoutine, TRUE);

    auto cerberus = LoadLibrary(L"HidCerberus.Lib.dll");

    if (cerberus == INVALID_HANDLE_VALUE)
    {
        printf("Couldn't find HidCerberus.Lib.dll\n");
        return 1;
    }

    fpOpen = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianOpen"));
    fpClose = reinterpret_cast<HidGuardianOpen_t>(GetProcAddress(cerberus, "HidGuardianClose"));

    printf("Bypassing HidGuardian\n");
    if (fpOpen && fpOpen()) printf("Process white-listed\n");
    else printf("Warning: couldn't contact HidGuardian\n");

    printf("Initializing emulation driver\n");

    g_client = vigem_alloc();

    auto ret = vigem_connect(g_client);
    if (!VIGEM_SUCCESS(ret)) {
        printf("Couldn't initialize emulation driver\n");
        return 1;
    }

    for (auto i = 0; i < _countof(targets); i++)
    {
        targets[i] = vigem_target_x360_alloc();
    }

    printf("Enabling XInput\n");

    auto xinputMod = LoadLibrary(L"XInput1_3.dll");
    if(!xinputMod)
    {
        printf("XInput1_3.dll not found\n");
        return 1;
    }

    auto pXInputEnable = reinterpret_cast<VOID(WINAPI*)(BOOL)>(GetProcAddress(xinputMod, "XInputEnable"));
    auto pXInputGetState = reinterpret_cast<DWORD(WINAPI*)(DWORD, XINPUT_STATE*)>(GetProcAddress(xinputMod, "XInputGetState"));

    pXInputEnable(TRUE);

    DWORD result;
    XINPUT_STATE state;

    printf("Starting translation, close window to exit...\n");

    while (true)
    {
        auto begin = high_resolution_clock::now();

        for (auto i = 0; i < XUSER_MAX_COUNT; i++)
        {
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            result = pXInputGetState(i, &state);

            if (result == ERROR_SUCCESS)
            {
                if (VIGEM_SUCCESS(vigem_target_add(g_client, targets[i])))
                {
                    printf("Plugged in controller %d\t\t\t\t\n", vigem_target_get_index(targets[i]));
                }

                vigem_target_x360_update(g_client, targets[i], *reinterpret_cast<XUSB_REPORT*>(&state.Gamepad));
            }
            else
            {
                if (VIGEM_SUCCESS(vigem_target_remove(g_client, targets[i])))
                {
                    printf("Unplugged controller %d\t\t\t\t\n", vigem_target_get_index(targets[i]));
                }
            }
        }

        auto end = high_resolution_clock::now();
        auto dur = end - begin;
        auto ns = duration_cast<nanoseconds>(dur);
        auto delay = milliseconds(5) - ns;

        sleep_for(delay);

        auto finished = high_resolution_clock::now();

        printf("Polling delay: %1lld ms (Frequency: %3.2f Hz)\t\t\r",
            duration_cast<milliseconds>(delay).count(),
            (1.0 / duration_cast<milliseconds>(finished - begin).count()) * 1000);
    }

    return 0;
}

BOOL WINAPI HandlerRoutine(
    _In_ DWORD dwCtrlType
)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT: // Ctrl+C
    case CTRL_BREAK_EVENT: // Ctrl+Break
    case CTRL_CLOSE_EVENT: // Closing the console window
        if (fpClose)fpClose();

        for (auto i = 0; i < _countof(targets); i++)
        {
            vigem_target_remove(g_client, targets[i]);
            vigem_target_free(targets[i]);
        }

        vigem_disconnect(g_client);
        vigem_free(g_client);

        return TRUE;
    default: break;
    }

    return FALSE;
}

