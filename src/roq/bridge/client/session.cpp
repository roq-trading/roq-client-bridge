/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bridge/client/session.hpp"

#include <magic_enum/magic_enum_format.hpp>

#include <cassert>

#include "roq/logging.hpp"

#include "roq/clock.hpp"

#include "roq/utils/enum.hpp"
#include "roq/utils/update.hpp"

using namespace std::literals;

namespace roq {
namespace bridge {
namespace client {

// === CONSTANTS ===

namespace {
auto const TIMEOUT = 5s;
}  // namespace

// === HELPERS ===

namespace {
auto create_server(auto &handler, auto &settings, auto &factory) {
  auto config = web::rest::Server::Config{
      .decode_buffer_size = {},
      .encode_buffer_size = {},
      .connection = {},
      .request_timeout = {},
      .server = {},  // XXX FIXME TODO
      .access_control_allow_origin = "*"sv,
      .url_prefix = settings.flags.url_prefix,
  };
  return web::rest::Server::create(handler, factory, config);
}

struct Query final {
  explicit Query(web::rest::Server::Request const &request) {
    enum class Key {
      MODE,
      ENCODING,
    };
    for (auto &[k, v] : request.query) {
      auto key = utils::parse_enum<Key>(k);
      switch (key) {
        case Key::MODE:
          mode = v;
          break;
        case Key::ENCODING:
          encoding = v;
          break;
      };
    }
  }

  std::string_view mode;
  std::string_view encoding;
};
}  // namespace

// === IMPLEMENTATION ===

Session::Session(Handler &handler, Settings const &settings, io::net::tcp::Connection::Factory &factory, uint64_t session_id)
    : handler_{handler}, session_id_{session_id}, server_{create_server(*this, settings, factory)}, last_refresh_{clock::get_system()} {
  assert(last_refresh_.count());
}

void Session::ping(std::chrono::nanoseconds now) {
  switch (state_) {
    using enum State;
    case CONNECTED:
      if ((last_refresh_ + TIMEOUT) < now) {
        log::info("[{}] Detected timeout"sv, session_id_);
        disconnect();
      }
      break;
    case READY:
      // note! web framework will manage ping/pong and timeout
      break;
    case ZOMBIE:
      return;
  }
}

// web::rest::Server::Handler

void Session::operator()(web::rest::Server::Disconnected const &) {
  log::info("[{}] Disconnected"sv, session_id_);
  last_refresh_ = clock::get_system();
  disconnect();
}

void Session::operator()(web::rest::Server::Request const &request) {
  log::info("[{}] Request"sv, session_id_);
  last_refresh_ = clock::get_system();
  if (request.headers.connection.has(web::http::Connection::UPGRADE)) {
    check_upgrade(request);
    log::info("[{}] Upgrading to websocket..."sv, session_id_);
    (*server_).upgrade(request);
    state_ = State::READY;
  } else {
    log::warn("[{}] Expected upgrade request"sv, session_id_);
    disconnect();
  }
}

void Session::operator()(web::rest::Server::Text const &) {
  log::info("[{}] Text"sv, session_id_);
  last_refresh_ = clock::get_system();
}

void Session::operator()(web::rest::Server::Binary const &) {
  log::info("[{}] Binary"sv, session_id_);
  last_refresh_ = clock::get_system();
}

// utils

void Session::operator()(State state) {
  if (!utils::update(state_, state))
    return;
  log::info("[{}] state={}"sv, session_id_, state_);
}

void Session::check_upgrade(web::rest::Server::Request const &request) {
  Query query{request};
  if (std::empty(query.mode) || std::empty(query.encoding))
    throw RuntimeError{"Unexpected: missing 'mode' and/or 'encoding' query params"sv};
  mode_ = utils::parse_enum<Mode>(query.mode);
  encoding_ = utils::parse_enum<Encoding>(query.encoding);
}

void Session::disconnect() {
  switch (state_) {
    using enum State;
    case CONNECTED: {
      (*this)(State::ZOMBIE);
      auto disconnect = Disconnect{
          .session_id = session_id_,
      };
      handler_(disconnect);
      break;
    }
    case READY: {
      (*server_).close();
      (*this)(State::ZOMBIE);
      auto disconnect = Disconnect{
          .session_id = session_id_,
      };
      handler_(disconnect);
      break;
    }
    case ZOMBIE:
      return;
  }
}

}  // namespace client
}  // namespace bridge
}  // namespace roq
