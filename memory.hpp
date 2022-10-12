#include <vector>
#include <string_view>
#include "minhook/MinHook.h"
#pragma comment(lib, "libMinHook.x64.lib")

std::uintptr_t FindPattern(const std::uintptr_t pModuleBaseAddress, const char* szSignature, const std::size_t nSelectResultIndex) {
    if (!pModuleBaseAddress) return NULL;

    constexpr auto PatternToBytes = [](const char* szpattern) {
        auto       m_iBytes = std::vector<int>{};
        const auto szStartAddr = const_cast<char*>(szpattern);
        const auto szEndAddr = const_cast<char*>(szpattern) + strlen(szpattern);

        for (auto szCurrentAddr = szStartAddr; szCurrentAddr < szEndAddr; ++szCurrentAddr) {
            if (*szCurrentAddr == '?') {
                ++szCurrentAddr;
                if (*szCurrentAddr == '?') ++szCurrentAddr;
                m_iBytes.push_back(-1);
            }
            else m_iBytes.push_back(strtoul(szCurrentAddr, &szCurrentAddr, 16));
        }
        return m_iBytes;
    };

    const auto pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pModuleBaseAddress);
    const auto pNTHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(pModuleBaseAddress + pDosHeader->e_lfanew));
    const auto dwSizeOfImage = pNTHeaders->OptionalHeader.SizeOfImage;
    auto       m_iPatternBytes = PatternToBytes(szSignature);
    const auto pScanBytes = reinterpret_cast<std::uint8_t*>(pModuleBaseAddress);
    const auto m_iPatternBytesSize = m_iPatternBytes.size();
    const auto m_iPatternBytesData = m_iPatternBytes.data();
    std::size_t nFoundResults = 0;

    for (auto i = 0ul; i < dwSizeOfImage - m_iPatternBytesSize; ++i) {
        auto already_found = true;

        for (auto j = 0ul; j < m_iPatternBytesSize; ++j) {
            if (pScanBytes[i + j] != m_iPatternBytesData[j] && m_iPatternBytesData[j] != -1) {
                already_found = false;
                break;
            }
        }

        if (already_found) {
            if (nSelectResultIndex != 0) {
                if (nFoundResults < nSelectResultIndex) {
                    nFoundResults++;
                    already_found = false;
                }
                else return reinterpret_cast<std::uintptr_t>(&pScanBytes[i]);
            }
            else return reinterpret_cast<std::uintptr_t>(&pScanBytes[i]);
        }
    }
    return NULL;
}

std::uintptr_t GetAbsoluteAddress(const std::uintptr_t instruction_ptr, const short offset, const short size)
{
    return instruction_ptr + *reinterpret_cast<std::int32_t*>(instruction_ptr + offset) + size;
}
