#include "memory.hpp"

inline std::uintptr_t aSendReadyUpLobbyUpdate;
inline std::uintptr_t aDOTAGCClientSystem;
inline std::uintptr_t aSetDialogVariable;

using SetDialogVariable_t = void(__fastcall*)(void* rcx, const char* rdx, const char* r8);
inline SetDialogVariable_t fpSetDialogVariable;

void __fastcall CPanel2D__SetDialogVariable(void* rcx, const char* rdx, const char* r8 ) {
    if (r8 && !strcmp(r8, "#dota_accept_match_accept")) {
        (((std::uintptr_t(__fastcall*)(std::uintptr_t, bool))aSendReadyUpLobbyUpdate)(aDOTAGCClientSystem, 1));
    }

    fpSetDialogVariable(rcx, rdx, r8);
}

bool __stdcall DllMain(void*, int call_reason, void*)
{
    if (call_reason == 1) {
        const auto aClient = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("client.dll"));

        if (!aClient) {
            MessageBoxA(nullptr, "client.dll not found", "auto accept error", MB_ICONERROR);
            return 1;
        }

        aSetDialogVariable = FindPattern(aClient, "48 89 5C 24 18 48 89 6C 24 20 56 48 81 EC ? ? ? ? 49 8B D8", 0);
        aSendReadyUpLobbyUpdate = FindPattern(aClient, "48 89 5C 24 08 48 89 6C 24 10 57 48 81 EC ? ? ? ? 48 8B 05", 0);
        aDOTAGCClientSystem = GetAbsoluteAddress(FindPattern(aClient, "84 C9 74 ? 80 7F 08", 0) + 0xE, 3, 7);

        if (!aSetDialogVariable) {
            MessageBoxA(nullptr, "aSetDialogVariable pattern not found", "auto accept error", MB_ICONERROR);
            return 1;
        }
        if (!aSendReadyUpLobbyUpdate) {
            MessageBoxA(nullptr, "SendReadyUpLobbyUpdate pattern not found", "auto accept error", MB_ICONERROR);
            return 1;
        }
        if (!aDOTAGCClientSystem) {
            MessageBoxA(nullptr, "aDOTAGCClientSystem pattern not found", "auto accept error", MB_ICONERROR);
            return 1;
        }
        if (MH_Initialize() != MH_STATUS::MH_OK) {
            MessageBoxA(nullptr, "minhook not inited", "auto accept error", MB_ICONERROR);
            return 1;
        }

        MH_CreateHook(reinterpret_cast<LPVOID>(aSetDialogVariable), &CPanel2D__SetDialogVariable, reinterpret_cast<LPVOID*>(&fpSetDialogVariable));
        if (MH_EnableHook(reinterpret_cast<LPVOID>(aSetDialogVariable)) != MH_STATUS::MH_OK) {
            MessageBoxA(nullptr, "hook not installed", "auto accept error", MB_ICONERROR);
            return 1;
        }

        MessageBoxA(nullptr, "injected", "auto accept", MB_OK);
    }

    else if (call_reason == 0) {
        MH_DisableHook(reinterpret_cast<LPVOID>(aSetDialogVariable));
        MH_Uninitialize();
    }
    return true;
}
