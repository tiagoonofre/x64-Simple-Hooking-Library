#include "Hooking/Hooking.h"

#include <string_view>

// Define original function signature
typedef int (WINAPI* fnMessageBoxA)(HWND, LPCSTR, LPCSTR, UINT);
InlineHooking hkMessageBox;

// Detour Callback
int WINAPI hkMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    auto oOriginal = hkMessageBox.GetOriginal<fnMessageBoxA>();

    // Inspect or modify parameters safely using C++ string views
    if (lpCaption && std::string_view(lpCaption) == "Target Exception") {
        lpText = "Interrupted by Hook Engine!";
    }

    return oOriginal(hWnd, lpText, lpCaption, uType);
}

int main() {
    Hooking::Init();

    // Setup hook on target API
    auto* pTarget = GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");
    hkMessageBox.Setup(reinterpret_cast<uintptr_t*>(pTarget), hkMessageBoxA);

    // Call target (Will hit the hook)
    MessageBoxA(0, "Original Message", "Target Exception", MB_OK);

    Hooking::Shutdown();
}