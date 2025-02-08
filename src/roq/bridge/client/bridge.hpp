/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <memory>
#include <span>
#include <string_view>

#include "roq/codec/decoder.hpp"
#include "roq/codec/encoder.hpp"
#include "roq/codec/type.hpp"

#include "roq/io/context.hpp"

#include "roq/client/poller.hpp"

#include "roq/bridge/client/config.hpp"
#include "roq/bridge/client/settings.hpp"

namespace roq {
namespace bridge {
namespace client {

struct Bridge final : public roq::client::Poller::Handler {
  struct Text final {
    std::string_view payload;
  };

  struct Binary final {
    std::span<std::byte const> payload;
  };

  struct Handler {
    virtual void operator()(Text const &) = 0;
    virtual void operator()(Binary const &) = 0;
  };

  Bridge(Handler &, Settings const &, Config const &, io::Context &, std::span<std::string_view const> const &params, codec::Type);

  Bridge(Bridge const &) = delete;

  // client => websocket
  inline void dispatch() { (*client_).dispatch(*this); }

  // websocket => client
  void dispatch(std::string_view const &message);
  void dispatch(std::span<std::byte const> const &message);

 protected:
  // roq::client::Poller::Handler

  void operator()(Event<Ready> const &) override;

  // utils

  void decode(auto &message);
  void encode(auto &event);

 private:
  Handler &handler_;
  std::unique_ptr<roq::client::Poller> const client_;
  std::unique_ptr<codec::Encoder> const encoder_;
  std::unique_ptr<codec::Decoder> const decoder_;
};

}  // namespace client
}  // namespace bridge
}  // namespace roq
