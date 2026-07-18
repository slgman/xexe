#include "util/midhook.hpp"

#include <Geode/Geode.hpp>

namespace util {

void installMid(uintptr_t address, std::string_view label,
                void (*callback)(safetyhook::Context&)) {
    MidhookRegistry::get().keep(safetyhook::create_mid(address, callback));
    geode::log::info("[mid] {} @ 0x{:x}", label, address);
}

}  // namespace util
