/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/bridge/client/bridge.hpp"

using namespace std::literals;

namespace roq {
namespace bridge {
namespace client {

// === CONSTANTS ===

namespace {}  // namespace

// === HELPERS ===

namespace {
auto create_poller(auto &settings, auto &config, auto &context, auto &params, auto &user) {
  auto settings_2 = settings;
  settings_2.app.name = user;
  return roq::client::Poller::create(settings_2, config, context, params);
}

auto create_encoder(auto type) {
  return codec::Encoder::create(type);
}

auto create_decoder(auto type) {
  return codec::Decoder::create(type);
}

struct Bridge2 final : public codec::Decoder::Handler {
  explicit Bridge2(roq::client::Poller &dispatcher) : dispatcher_{dispatcher} {}

 protected:
  void operator()(Event<Control> const &) override {}
  void operator()(Event<ControlAck> const &) override {}
  void operator()(Event<ServiceUpdate> const &) override {}
  void operator()(Event<StrategyUpdate> const &) override {}
  void operator()(Event<LegsUpdate> const &) override {}
  void operator()(Event<DownloadBegin> const &) override {}
  void operator()(Event<DownloadEnd> const &) override {}
  void operator()(Event<Ready> const &) override {}
  void operator()(Event<GatewaySettings> const &) override {}
  void operator()(Event<StreamStatus> const &) override {}
  void operator()(Event<ExternalLatency> const &) override {}
  void operator()(Event<RateLimitsUpdate> const &) override {}
  void operator()(Event<RateLimitTrigger> const &) override {}
  void operator()(Event<GatewayStatus> const &) override {}
  void operator()(Event<ReferenceData> const &) override {}
  void operator()(Event<MarketStatus> const &) override {}
  void operator()(Event<TopOfBook> const &) override {}
  void operator()(Event<MarketByPriceUpdate> const &) override {}
  void operator()(Event<MarketByOrderUpdate> const &) override {}
  void operator()(Event<TradeSummary> const &) override {}
  void operator()(Event<StatisticsUpdate> const &) override {}
  void operator()(Event<TimeSeriesUpdate> const &) override {}
  void operator()(Event<Subscribe> const &) override {}
  void operator()(Event<CreateOrder> const &event) override { dispatcher_.send(event.value, event.message_info.source, event.message_info.is_last); }
  void operator()(Event<ModifyOrder> const &) override {}
  void operator()(Event<CancelOrder> const &) override {}
  void operator()(Event<CancelAllOrders> const &) override {}
  void operator()(Event<CancelAllOrdersAck> const &) override {}
  void operator()(Event<OrderAck> const &) override {}
  void operator()(Event<OrderUpdate> const &) override {}
  void operator()(Event<TradeUpdate> const &) override {}
  void operator()(Event<PositionUpdate> const &) override {}
  void operator()(Event<FundsUpdate> const &) override {}
  void operator()(Event<CustomMetrics> const &) override {}
  void operator()(Event<CustomMetricsUpdate> const &) override {}
  void operator()(Event<CustomMatrix> const &) override {}
  void operator()(Event<CustomMatrixUpdate> const &) override {}
  void operator()(Event<ParametersUpdate> const &) override {}
  void operator()(Event<Portfolio> const &) override {}
  void operator()(Event<PortfolioUpdate> const &) override {}
  void operator()(Event<RiskLimits> const &) override {}
  void operator()(Event<RiskLimitsUpdate> const &) override {}
  void operator()(Event<MassQuote> const &) override {}
  void operator()(Event<MassQuoteAck> const &) override {}
  void operator()(Event<CancelQuotes> const &) override {}
  void operator()(Event<CancelQuotesAck> const &) override {}

 private:
  roq::client::Poller &dispatcher_;
};
}  // namespace

// === IMPLEMENTATION ===

Bridge::Bridge(
    Handler &handler,
    Settings const &settings,
    roq::client::Config const &config,
    io::Context &context,
    std::span<std::string_view const> const &params,
    std::string_view const &user,
    codec::Type type)
    : handler_{handler}, client_{create_poller(settings, config, context, params, user)}, encoder_{create_encoder(type)}, decoder_{create_decoder(type)} {
}

void Bridge::stop() {
  (*client_).stop();
}

void Bridge::dispatch(std::string_view const &message) {
  decode(message);
}

void Bridge::dispatch(std::span<std::byte const> const &message) {
  decode(message);
}

// roq::client::Poller::Handler

void Bridge::operator()(Event<roq::Start> const &) {
  Start start;
  handler_(start);
}

void Bridge::operator()(Event<roq::Stop> const &) {
  Stop stop;
  handler_(stop);
}

void Bridge::operator()(Event<Ready> const &event) {
  encode(event);
}

// utils

void Bridge::decode(auto &message) {
  Bridge2 bridge{*client_};
  MessageInfo message_info;
  (*decoder_)(bridge, message, message_info);
}

void Bridge::encode(auto &event) {
  auto message = (*encoder_)(event);
  auto binary = Binary{
      .payload = message,
  };
  handler_(binary);
}

}  // namespace client
}  // namespace bridge
}  // namespace roq
