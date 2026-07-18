#pragma once

#include <safetyhook.hpp>
#include <string_view>
#include <vector>

namespace util {

class MidhookRegistry {
public:
    static MidhookRegistry& get() {
        static MidhookRegistry instance;
        return instance;
    }

    MidhookRegistry(MidhookRegistry&&) = delete;

    void keep(SafetyHookMid&& hook) { m_hooks.push_back(std::move(hook)); }

private:
    MidhookRegistry() = default;
    std::vector<SafetyHookMid> m_hooks;
};

void installMid(uintptr_t address, std::string_view label,
                void (*callback)(safetyhook::Context&));

}  // namespace util
