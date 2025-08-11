#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

DWORD GetProcessPID(const std::wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(hSnapshot, &pe32)) {
        do {
            if (processName == pe32.szExeFile) {
                DWORD pid = pe32.th32ProcessID;
                CloseHandle(hSnapshot);
                return pid; 
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return 0; 
}

void run_chrome() {
    system("start chrome");
}

DWORD WaitForProcessToStart(const std::wstring& processName, int timeoutMs = 5000) {
    int elapsed = 0;
    const int checkInterval = 100;
    
    while (elapsed < timeoutMs) {
        DWORD pid = GetProcessPID(processName);
        if (pid != 0) {
            return pid; 
        }
        
        Sleep(checkInterval);
        elapsed += checkInterval;
    }
    
    return 0;
}

bool InjectDLL(DWORD processId, const std::string& dllPath) {
    // 1. Ouvrir le processus cible avec le PID fourni
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::cerr << "Failed to open target process (PID: " << processId << ")\n";
        return false;
    }

    // 2. Allouer de la mémoire dans le processus cible
    LPVOID allocMemory = VirtualAllocEx(hProcess, nullptr, dllPath.size() + 1, 
                                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!allocMemory) {
        std::cerr << "Failed to allocate memory in target process\n";
        CloseHandle(hProcess);
        return false;
    }

    // 3. Écrire le chemin de la DLL dans le processus
    if (!WriteProcessMemory(hProcess, allocMemory, dllPath.c_str(), dllPath.size() + 1, nullptr)) {
        std::cerr << "Failed to write DLL path to process memory\n";
        VirtualFreeEx(hProcess, allocMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // 4. Récupérer l'adresse de LoadLibraryA
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        std::cerr << "Failed to get handle to kernel32.dll\n";
        VirtualFreeEx(hProcess, allocMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    FARPROC loadLibraryAddr = GetProcAddress(hKernel32, "LoadLibraryA");
    if (!loadLibraryAddr) {
        std::cerr << "Failed to get address of LoadLibraryA\n";
        VirtualFreeEx(hProcess, allocMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // 5. Créer un thread distant
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, 
        (LPTHREAD_START_ROUTINE)loadLibraryAddr, allocMemory, 0, nullptr);
    
    if (!hThread) {
        std::cerr << "Failed to create remote thread\n";
        VirtualFreeEx(hProcess, allocMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // 6. Attendre que le thread se termine
    WaitForSingleObject(hThread, INFINITE);

    // 7. Nettoyage
    VirtualFreeEx(hProcess, allocMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    std::cout << "DLL injected successfully into PID: " << processId << "\n";
    return true;
}

int main() {
    DWORD pid = GetProcessPID(L"chrome.exe");
    
    if (pid != 0) {
        std::wcout << L"chrome is running with PID: " << pid << std::endl;
    } else {
        std::wcout << L"chrome is not running, launching..." << std::endl;
        run_chrome();
        
        std::wcout << L"Waiting for chrome to start..." << std::endl;
        pid = WaitForProcessToStart(L"chrome.exe");
        
        if (pid != 0) {
            std::wcout << L"chrome started with PID: " << pid << std::endl;
        } else {
            std::wcout << L"Failed to detect chrome startup (timeout)" << std::endl;
            return 1;
        }
    }
    
    // Maintenant injecter la DLL dans le processus Chrome trouvé
    std::string dllPath = "C:\\Users\\chmod\\Documents\\Projects\\C++\\chrome\\mydll.dll"; // CHANGEZ CE CHEMIN !
    
    std::wcout << L"Attempting to inject DLL into Chrome..." << std::endl;
    if (InjectDLL(pid, dllPath)) {
        std::wcout << L"DLL injection successful!" << std::endl;
    } else {
        std::wcout << L"DLL injection failed!" << std::endl;
        return 1;
    }
   
    return 0;
}