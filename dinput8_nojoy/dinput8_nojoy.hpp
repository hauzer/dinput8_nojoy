#pragma once

#define DIRECTINPUT_VERSION 0x0800
#define CINTERFACE
#include <dinput.h>

extern "C" HRESULT WINAPI DirectInput8Create_hook(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
HRESULT WINAPI EnumDevices_hook(LPDIRECTINPUT8, DWORD, LPDIENUMDEVICESCALLBACK, LPVOID, DWORD);
BOOL WINAPI EnumDevices_callback_hook(LPCDIDEVICEINSTANCE, LPVOID);
