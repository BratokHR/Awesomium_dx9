#include "dllmain.h"

cg_t *          cg = (cg_t *)0x01379000;

HWND handle_window;
char path_game[MAX_PATH];

oRegisterFunction console_func;

void _main()
{
	while (GetModuleHandleA("gfx_d3d_mp_x86_s.dll") == 0 ||
		GetModuleHandleA("mssmp3.asi") == 0 ||
		GetModuleHandleA("d3d9.dll") == 0)
		Sleep(100);

	console_func = (oRegisterFunction)GetProcAddress(GetModuleHandle("cmdconsole.asi"), "RegisterCommand");
	handle_window = ::FindWindow(0, NAME_WINDOW);

	if (!console_func || !handle_window)
	{
		char err[256];
		sprintf_s<256>(err, "%i", GetLastError());
		MessageBox(0, err, err, 0);
		PostQuitMessage(0);
	}

	init_hook();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID)
{
	DisableThreadLibraryCalls(hinstDLL);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DWORD n = GetModuleFileName(hinstDLL, path_game, MAX_PATH);
		for (char *p = &path_game[n]; *p != '\0'; p--)
			*p = '\0';

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_main, 0, 0, 0);
	}

	return 1;
}