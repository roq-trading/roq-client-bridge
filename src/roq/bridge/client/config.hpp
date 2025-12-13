/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include "roq/client/config.hpp"

#include "roq/bridge/client/settings.hpp"

namespace roq {
namespace bridge {
namespace client {

struct Config final : public roq::client::Config {
  explicit Config(Settings const &);

  Config(Config &&) = default;
  Config(Config const &) = delete;

 protected:
  Settings const &settings_;
  void dispatch(Handler &) const override;
};

}  // namespace client
}  // namespace bridge
}  // namespace roq

template <>
struct fmt::formatter<roq::bridge::client::Config> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::bridge::client::Config const &, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(}})"sv);
  }
};
