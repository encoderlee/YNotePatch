#include <windows.h>
#include "APIHook.h"

static UINT g_format = 0;
static bool g_switch = true;

UINT __stdcall My_RegisterClipboardFormatW(LPCWSTR lpszFormat)
{
	UINT ret = RegisterClipboardFormat(lpszFormat);
	if (wcscmp(lpszFormat, L"HTML Format") == 0)
		g_format = ret;
	return ret;
}


BOOL __stdcall My_IsClipboardFormatAvailable(UINT format)
{
	BOOL ret = IsClipboardFormatAvailable(format);
	if (g_switch && format == g_format)
		ret = FALSE;
	return ret;
}

void WorkThread(void *param)
{
	const UINT Q_KEY = 0x51;
	if (!RegisterHotKey(NULL, GlobalAddAtom(L"MyHotKey"), MOD_CONTROL | MOD_NOREPEAT, Q_KEY))
		return;
	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			if (g_switch)
				g_switch = false;
			else
				g_switch = true;
		}
	}
}


CAPIHook hooker_RegisterClipboardFormatW("User32.dll", "RegisterClipboardFormatW", reinterpret_cast<PROC>(My_RegisterClipboardFormatW));

CAPIHook hooker_IsClipboardFormatAvailable("User32.dll", "IsClipboardFormatAvailable", reinterpret_cast<PROC>(My_IsClipboardFormatAvailable));