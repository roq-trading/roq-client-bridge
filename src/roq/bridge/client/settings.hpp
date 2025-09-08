/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/compat/fmt.hpp"

#include <fmt/format.h>

#include "roq/client.hpp"

#include "roq/bridge/client/flags/flags.hpp"

namespace roq {
namespace bridge {
namespace client {

struct Settings final : public roq::client::flags::Settings {
  explicit Settings(args::Parser const &);

  flags::Flags flags;
};

}  // namespace client
}  // namespace bridge
}  // namespace roq

template <>
struct fmt::formatter<roq::bridge::client::Settings> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::bridge::client::Settings const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(flags={}, )"
        R"(client={})"
        R"(}})"sv,
        value.flags,
        static_cast<roq::client::Settings2 const &>(value));
  }
};
