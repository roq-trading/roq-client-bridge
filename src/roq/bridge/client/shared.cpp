/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bridge/client/shared.hpp"

using namespace std::literals;

namespace roq {
namespace bridge {
namespace client {

// === HELPERS ===

namespace {}  // namespace

// === IMPLEMENTATION ===

Shared::Shared(Settings const &settings, Config const &config, std::span<std::string_view const> const &params)
    : settings{settings}, config{config}, params{params} {
}

}  // namespace client
}  // namespace bridge
}  // namespace roq
