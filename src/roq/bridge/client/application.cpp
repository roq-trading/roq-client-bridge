/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/bridge/client/application.hpp"

#include "roq/logging.hpp"

#include "roq/io/engine/context_factory.hpp"

#include "roq/bridge/client/config.hpp"
#include "roq/bridge/client/controller.hpp"
#include "roq/bridge/client/settings.hpp"

using namespace std::literals;

namespace roq {
namespace bridge {
namespace client {

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  auto params = args.params();
  if (std::empty(params))
    log::fatal("Expected arguments"sv);
  Settings settings{args};
  log::info("settings={}"sv, settings);
  Config config{settings};
  log::info("config={}"sv, config);
  auto context = io::engine::ContextFactory::create();
  log::info("Starting the bridge"sv);
  Controller{settings, config, *context, params}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace client
}  // namespace bridge
}  // namespace roq
