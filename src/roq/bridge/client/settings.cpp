/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bridge/client/settings.hpp"

#include "roq/logging.hpp"

#include "roq/client/flags/settings.hpp"

using namespace std::literals;

namespace roq {
namespace bridge {
namespace client {

Settings::Settings(args::Parser const &args) : roq::client::flags::Settings{args}, flags{flags::Flags::create()} {
  log::info("settings={}"sv, *this);
}

}  // namespace client
}  // namespace bridge
}  // namespace roq
