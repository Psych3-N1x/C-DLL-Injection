Injection de DLL en C++
📜 Description

Ce projet montre comment réaliser une injection dynamique de DLL dans un processus Windows, en l’occurrence Google Chrome (modifiable pour d’autres processus).
L’objectif est de charger une bibliothèque externe (.dll) directement dans la mémoire d’un processus cible à l’exécution.

Il utilise les API Windows suivantes :

    CreateToolhelp32Snapshot, Process32FirstW, Process32NextW pour parcourir les processus actifs et récupérer leur PID.

    OpenProcess, VirtualAllocEx, WriteProcessMemory pour réserver et écrire de la mémoire dans le processus distant.

    GetModuleHandleA, GetProcAddress pour récupérer l’adresse de LoadLibraryA.

    CreateRemoteThread pour exécuter le chargement de la DLL dans le processus cible.

# ⚠️ **Avertissement**

    Ce projet est à but éducatif uniquement.
    L’injection de DLL peut être utilisée à des fins malveillantes, ce qui est illégal dans de nombreux pays.
    Utilisez-le uniquement sur vos propres applications ou dans un environnement de test.

📂 Fonctionnalités

    🔍 Recherche automatique du processus cible par son nom.

    🚀 Lancement du processus s’il n’est pas déjà ouvert (ici Chrome).

    ⏳ Attente avec délai configurable pour détecter le démarrage du processus.

    📥 Injection de DLL via un thread distant.

    🧹 Nettoyage mémoire après l’injection.

🛠️ Compilation
Prérequis

    Windows (WinAPI nécessaire)

    MinGW, Visual Studio ou tout autre compilateur compatible C++

    Activer le support de la bibliothèque Windows.h

Commande (exemple MinGW) :

g++ main.cpp -o injector.exe -static -lws2_32

⚙️ Utilisation

    Modifier le chemin de la DLL à injecter dans le code :

std::string dllPath = "C:\\chemin\\vers\\ma_dll.dll";

Compiler le projet.

Exécuter l’injecteur :

    injector.exe

    Le programme :

        Cherche chrome.exe dans la liste des processus

        Le lance s’il n’est pas trouvé

        Attends que Chrome démarre

        Injecte la DLL dans Chrome

        Affiche un message de succès ou d’échec

📜 Exemple de sortie console

chrome is not running, launching...
Waiting for chrome to start...
chrome started with PID: 4560
Attempting to inject DLL into Chrome...
DLL injected successfully into PID: 4560
DLL injection successful!

📌 Notes techniques

    L’injection repose sur LoadLibraryA qui charge la DLL à partir du chemin passé en mémoire distante.

    Le processus est ouvert avec PROCESS_ALL_ACCESS, ce qui nécessite l’exécution en administrateur si la cible est protégée.

    VirtualAllocEx réserve un espace mémoire pour y écrire le chemin de la DLL.

    CreateRemoteThread appelle LoadLibraryA pour charger la DLL.

    Le code peut être facilement adapté pour injecter dans d’autres processus (changer le nom du .exe).
**MIT LICENSE ADDED**



