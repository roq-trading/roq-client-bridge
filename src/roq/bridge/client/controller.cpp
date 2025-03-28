/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bridge/client/controller.hpp"

#include "roq/logging.hpp"

#include "roq/io/network_address.hpp"

#include "roq/io/sys/scheduler.hpp"

using namespace std::literals;
using namespace std::chrono_literals;

namespace roq {
namespace bridge {
namespace client {

// === CONSTANTS ===

namespace {
size_t const DISPATCH_THIS_MANY_BEFORE_CHECKING_CLOCK = 1000;
auto const YIELD_FREQUENCY = 1000ms;
auto const TIMER_FREQUENCY = 100ms;
auto const HEARTBEAT_FREQUENCY = 5s;
auto const CLEANUP_FREQUENCY = 1s;
}  // namespace

// === HELPERS ===

namespace {
auto create_timer(auto &handler, auto &context) {
  return context.create_timer(handler, TIMER_FREQUENCY);
}

auto create_tcp_listener(auto &handler, auto &settings, auto &context) {
  io::NetworkAddress address{settings.flags.client_listen_address};
  return context.create_tcp_listener(handler, address);
}
}  // namespace

// === IMPLEMENTATION ===

Controller::Controller(Settings const &settings, Config const &config, io::Context &context, std::span<std::string_view const> const &params)
    : shared_{settings, config, params}, context_{context}, terminate_{context.create_signal(*this, io::sys::Signal::Type::TERMINATE)},
      interrupt_{context.create_signal(*this, io::sys::Signal::Type::INTERRUPT)}, timer_{create_timer(*this, context_)},
      listener_{create_tcp_listener(*this, settings, context_)} {
}

void Controller::dispatch() {
  (*timer_).resume();
  std::chrono::nanoseconds next_yield = {};
  while (!stop_) {
    // yield?
    if (YIELD_FREQUENCY.count() > 0) {
      auto now = clock::get_system();
      if (next_yield < now) [[unlikely]] {
        next_yield = now + YIELD_FREQUENCY;
        io::sys::Scheduler::yield();
      }
    }
    // drain shared memory (note! we expect this to be more frequent than i/o)
    for (size_t i = 0; i < DISPATCH_THIS_MANY_BEFORE_CHECKING_CLOCK; ++i)
      for (auto &[_, session] : sessions_)
        (*session).dispatch();
    // drain i/o
    context_.drain();
  }
}

// io::sys::Signal::Handler

void Controller::operator()(io::sys::Signal::Event const &event) {
  log::warn("*** SIGNAL: {} ***"sv, event.type);
  stop_ = true;
}

// io::sys::Timer

void Controller::operator()(io::sys::Timer::Event const &event) {
  auto now = event.now;
  if (next_heartbeat_ < now) {
    for (auto &[_, session] : sessions_)
      (*session).ping(now);
    next_heartbeat_ = now + HEARTBEAT_FREQUENCY;
  }
  if (next_cleanup_ < now) {
    next_cleanup_ = now + CLEANUP_FREQUENCY;
    remove_zombies();
  }
}

// io::net::tcp::Listener::Handler

void Controller::operator()(io::net::tcp::Connection::Factory &factory) {
  auto session_id = ++next_session_id_;
  log::info("Connected (session_id={})"sv, session_id);
  create_session(factory, session_id);
}

void Controller::operator()(io::net::tcp::Connection::Factory &factory, io::NetworkAddress const &network_address) {
  auto session_id = ++next_session_id_;
  log::info("Connected (session_id={}, peer={})"sv, session_id, network_address.to_string_2());
  create_session(factory, session_id);
}

// Session::Handler

void Controller::operator()(Session::Disconnect const &disconnect) {
  log::info("Detected zombie session"sv);
  // shared_.session_logout(disconnect.session_id);
  zombies_.emplace(disconnect.session_id);
}

// utils

void Controller::create_session(io::net::tcp::Connection::Factory &factory, uint64_t session_id) {
  auto session = std::make_unique<Session>(*this, shared_, context_, factory, session_id);
  sessions_.try_emplace(session_id, std::move(session));
}

void Controller::remove_zombies() {
  auto count = std::size(zombies_);
  if (count == 0)
    return;
  for (auto session_id : zombies_)
    sessions_.erase(session_id);
  zombies_.clear();
  log::info("Removed {} zombied session(s) (remaining: {})"sv, count, std::size(sessions_));
}

}  // namespace client
}  // namespace bridge
}  // namespace roq
