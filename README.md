# ⚡ WinHook: Modern C++17 Process Interception & VMT Hooking Engine

A lightweight, high-performance C++17 library for Windows dynamic memory analysis, inline function detouring, and Virtual Method Table (VMT) manipulation. Designed with strict RAII safety guarantees and zero heap-allocation overhead.

![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20x86%2Fx64-0078D6.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

---

## 🛠️ Key Capabilities

* **Modern VMT Swapping:** Safely hooks virtual method tables by re-allocating shadow tables with full RTTI alignment.
* **Inline API Detours:** Leverages MinHook for atomic assembly instruction detouring.
* **RAII-Backed Lifecycle:** Automatic hook restoration and memory deallocation on object destruction to prevent process crashes.
* **Executability Validation:** Page-protection checking (`VirtualQuery`) to ensure memory safety prior to hook execution.

---

## 🚀 Quickstart Example

```cpp
#include "Hooking.h"

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
