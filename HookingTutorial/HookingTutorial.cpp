#include <Windows.h>

bool Hook(void* hookAddress, void* ourFunc, int len) // Hàm Hook 
{
	DWORD protection;
	VirtualProtect(hookAddress, len, PAGE_EXECUTE_READWRITE, &protection);

	DWORD relativeAddress = ((DWORD)ourFunc - (DWORD)hookAddress) - 5;

	*(BYTE*)hookAddress = 0xE9;
	*(DWORD*)((DWORD)hookAddress + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(hookAddress, len, protection, &temp);

	return true;
}

DWORD jmpBack;

_declspec(naked) void ourFunc() // Assembly
{
	_asm
	{
		sub eax, 0 // 83 E8 00
		jmp[jmpBack] // E9 [????????]
	}
}

DWORD WINAPI MainThread(LPVOID param)
{
	int hookLength = 8;

	DWORD hookAddress = 0x009425D7;
	jmpBack = hookAddress + hookLength;

	if (Hook((void*)hookAddress, ourFunc, hookLength)) {
		MessageBoxA(0, "Hook thanh cong!", "Thanh cong!", 0);
	}
	else
		MessageBoxA(0, "Hook that bai!", "That bai!", 0);

	while (true)
	{
		if (GetAsyncKeyState(VK_END))
		{
			break;
		}

		Sleep(40);
	}

	MessageBoxA(0, "Da xoa vung nho DLL!", "Bye!", 0);
	FreeLibraryAndExitThread((HMODULE)param, 0);

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, MainThread, hModule, 0, 0);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
		
	return TRUE;
}