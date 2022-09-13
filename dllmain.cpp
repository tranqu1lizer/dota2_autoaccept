#include "memory.hpp"

inline uintptr_t aSendReadyUpLobbyUpdate;
inline uintptr_t aDOTAGCClientSystem;
inline uintptr_t aSetDialogVariable;

using SetDialogVariable_t = void(__fastcall*)(void* thisptr, const char* name1, const char* name2);
inline SetDialogVariable_t fpSetDialogVariable;

void __fastcall CPanel2D__SetDialogVariable(void* thisptr, const char* name1, const char* name2) {
    if (!std::string_view(name2).compare("#dota_accept_match_accept")) {
        (((uintptr_t(__fastcall*)(uintptr_t, bool))aSendReadyUpLobbyUpdate)(aDOTAGCClientSystem, 1));
    }

    fpSetDialogVariable(thisptr, name1, name2);
}

bool __stdcall DllMain(void*, int call_reason, void*)
{
    if (call_reason == 1) {
        const auto aClient = reinterpret_cast<uintptr_t>(GetModuleHandleA("client.dll"));

        if (!aClient) {
            MessageBoxA(nullptr, "client.dll not found", "auto accept error", MB_ICONERROR);
            return 1;
        }

        aSetDialogVariable = FindPattern(aClient, "48 89 5C 24 18 48 89 6C 24 20 56 48 81 EC ? ? ? ? 49 8B D8", 0);
        aSendReadyUpLobbyUpdate = FindPattern(aClient, "48 89 5C 24 08 48 89 6C 24 10 57 48 81 EC ? ? ? ? 48 8B 05", 0);
        aDOTAGCClientSystem = GetAbsoluteAddress(FindPattern(aClient, "48 8B 05 ? ? ? ? 48 FF A0 30 01 00 00 CC CC CC CC CC CC CC CC 48 83 EC ? C7 44 24 40 ? ? ? ? 48 8D 0D ? ? ? ? 8B 44 24 40 41 B9 ? ? ? ? 45 33 C0 89 44 24 20 33 D2 FF 15 ? ? ? ? 89 05 ? ? ? ? 48 83 C4 ? C3 CC CC CC CC CC CC CC CC CC 48 81 EC ? ? ? ? 0F 57 C0 48 8D 0D ? ? ? ? 33 C0 41 B8", 0), 3, 7);

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