#include <iostream>
#include <string_view>
#include "Hooking/Hooking.h"

typedef int (WINAPI* fnMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);

InlineHooking hkMessageBox;

void ShowMessageBox()
{
	MessageBoxA(0, "This is the message before we hook!", "Hello World!", MB_OK);
}

int __stdcall hkMessageboxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) 
{
	auto oOriginal = hkMessageBox.GetOriginal<fnMessageBoxA>();

	if (lpCaption && std::string_view(lpCaption) == "Hello World!") {

		lpText = "Hey now, you're a rockstar!";
	}

	return oOriginal(hWnd, lpText, lpCaption, uType);
}

int main()
{
	Hooking::Init();

	HMODULE pUser32Dll = GetModuleHandleA("user32.dll");

	if (pUser32Dll == 0) {

		std::cout << "Failed to get user32.dll Adress" << std::endl;
		system("pause");
		exit(1);
	}

	uintptr_t* pMessageBoxA = (uintptr_t*)GetProcAddress(pUser32Dll, "MessageBoxA");

	if (!pMessageBoxA) {

		std::cout << "Failed to get MessageBoxA Adress" << std::endl;
		system("pause");
		exit(1);
	}

	ShowMessageBox();

	hkMessageBox.Setup(pMessageBoxA, hkMessageboxA);

	std::cout << "Function Hooked!" << std::endl;

	ShowMessageBox();

	Hooking::Shutdown();
}