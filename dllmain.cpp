#include <windows.h>
#include <MinHook.h>

// Lier la bibliothèque MinHook
#pragma comment(lib, "libMinHook.x64.lib")

// Prototype original
typedef int (WINAPI* MessageBoxW_t)(HWND, LPCWSTR, LPCWSTR, UINT);
MessageBoxW_t originalMessageBoxW = NULL;

// Hook fonction
int WINAPI HookedMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) {
    return originalMessageBoxW(hWnd, L"changed!", lpCaption, uType);
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    // Initialiser MinHook
    if (MH_Initialize() != MH_OK) {
        MessageBoxW(NULL, L"Failed to initialize MinHook", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Créer le hook
    if (MH_CreateHook(&MessageBoxW, &HookedMessageBoxW,
        reinterpret_cast<void**>(&originalMessageBoxW)) != MH_OK) {
        MessageBoxW(NULL, L"Failed to create hook", L"Error", MB_OK | MB_ICONERROR);
        MH_Uninitialize();
        return 1;
    }

    // Activer le hook
    if (MH_EnableHook(&MessageBoxW) != MH_OK) {
        MessageBoxW(NULL, L"Failed to enable hook", L"Error", MB_OK | MB_ICONERROR);
        MH_Uninitialize();
        return 1;
    }

    // Test du hook
    MessageBoxW(NULL, L"Original text", L"hooked", MB_OK);

    // Boucle principale (optionnelle selon votre usage)
    Sleep(5000); // Attendre 5 secondes au lieu d'une boucle infinie

    // Nettoyage
    MH_DisableHook(&MessageBoxW);
    MH_Uninitialize();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // Créer un thread pour éviter de bloquer le chargement de la DLL
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}