/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/bridge/client/config.hpp"
#include "roq/bridge/client/settings.hpp"

namespace roq {
namespace bridge {
namespace client {

struct Shared final {
  Shared(Settings const &, Config const &);

  Shared(Shared const &) = delete;

  Settings const &settings;
};

}  // namespace client
}  // namespace bridge
}  // namespace roq
