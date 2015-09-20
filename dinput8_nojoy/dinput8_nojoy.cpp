#include "dinput8_nojoy.hpp"

typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);
typedef HRESULT (WINAPI *EnumDevices_t)(LPDIRECTINPUT8, DWORD, LPDIENUMDEVICESCALLBACK, LPVOID, DWORD);

HMODULE dinput8;
DirectInput8Create_t DirectInput8Create_orig;
EnumDevices_t EnumDevices_orig;
LPDIENUMDEVICESCALLBACK EnumDevices_callback_orig;

BOOL WINAPI DllMain(
	HINSTANCE inst,
	unsigned long reason,
	void* info
	) {
	switch (reason) {
		case DLL_PROCESS_ATTACH: {
			dinput8 = LoadLibrary(LR"(C:\Windows\System32\dinput8.dll)");
			DirectInput8Create_orig = reinterpret_cast<DirectInput8Create_t>(GetProcAddress(dinput8, "DirectInput8Create"));
		} break;
		case DLL_PROCESS_DETACH: {
			FreeLibrary(dinput8);
		} break;
	}

	return TRUE;
}

BOOL WINAPI EnumDevices_callback_hook(LPCDIDEVICEINSTANCE device, LPVOID data) {
	auto type = device->dwDevType & 0x1F;
	if (type == DI8DEVTYPE_MOUSE || type == DI8DEVTYPE_KEYBOARD) {
		return EnumDevices_callback_orig(device, data);
	} else {
		return DIENUM_CONTINUE;
	}
}

HRESULT WINAPI EnumDevices_hook(
	LPDIRECTINPUT8 iface,
	DWORD type,
	LPDIENUMDEVICESCALLBACK callback,
	LPVOID data,
	DWORD flags) {
	EnumDevices_callback_orig = callback;
	return EnumDevices_orig(iface, type, EnumDevices_callback_hook, data, flags);
}

extern "C" long WINAPI DirectInput8Create_hook(
	HINSTANCE inst,
	DWORD ver,
	REFIID id,
	LPVOID* iface,
	LPUNKNOWN aggreg) {
	auto retval = DirectInput8Create_orig(inst, ver, id, iface, aggreg);

	auto idinput8 = reinterpret_cast<IDirectInput8*>(*iface);
	auto& EnumDevices_vtable = idinput8->lpVtbl->EnumDevices;

	if (EnumDevices_vtable != EnumDevices_hook) {
		EnumDevices_orig = EnumDevices_vtable;
		EnumDevices_vtable = EnumDevices_hook;
	}

	return retval;
}
