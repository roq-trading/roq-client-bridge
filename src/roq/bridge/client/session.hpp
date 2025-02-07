/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <memory>

#include "roq/io/net/tcp/connection.hpp"

#include "roq/web/rest/server.hpp"

#include "roq/bridge/client/settings.hpp"
#include "roq/bridge/client/shared.hpp"

namespace roq {
namespace bridge {
namespace client {

struct Session final : public web::rest::Server::Handler {
  struct Disconnect final {
    uint64_t session_id = {};
  };

  struct Handler {
    virtual void operator()(Disconnect const &) = 0;
  };

  Session(Handler &, Settings const &, io::net::tcp::Connection::Factory &, uint64_t session_id);

  Session(Session const &) = delete;

  void ping(std::chrono::nanoseconds now);

  void write(std::span<std::byte const> const &);

 protected:
  // web::rest::Server::Handler

  void operator()(web::rest::Server::Disconnected const &) override;
  void operator()(web::rest::Server::Request const &) override;
  void operator()(web::rest::Server::Text const &) override;
  void operator()(web::rest::Server::Binary const &) override;

  // utils

  enum class State {
    CONNECTED,
    READY,
    ZOMBIE,
  };

  enum class Mode {
    TEXT,
    BINARY,
  };

  enum class Encoding {
    JSON,
    FBS,
    SBE,
    ROQ,
  };

  void operator()(State);

  void check_upgrade(web::rest::Server::Request const &);

  void disconnect();

 private:
  Handler &handler_;
  uint64_t const session_id_;
  std::unique_ptr<web::rest::Server> const server_;
  std::chrono::nanoseconds last_refresh_ = {};
  State state_ = {};
  Mode mode_ = {};
  Encoding encoding_ = {};
};

}  // namespace client
}  // namespace bridge
}  // namespace roq
