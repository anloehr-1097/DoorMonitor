#include "WebsocketClient.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "portmacro.h"
#include <coroutine>
#include <memory>
#include <string>
#include <utility>

WebsocketClient::WebsocketClient(const char *uri) {
  esp_websocket_client_config_t ws_cfg{.uri = uri,
                                       .port = 8080,
                                       .reconnect_timeout_ms = 20000,
                                       .network_timeout_ms = 20000};
  client_handle = std::unique_ptr<esp_websocket_client, ClientHandleDeleter>(
      esp_websocket_client_init(&ws_cfg), ClientHandleDeleter());
}

// take a function pointer (e.g. a captureless lambda which implicitly converts
// to a fp) and assign to handler member
void WebsocketClient::register_handler(ws_event_handler hdlr) {
  handler = hdlr;
  esp_websocket_register_events(client_handle.get(), WEBSOCKET_EVENT_ANY,
                                handler, this);
}

void WebsocketClient::start() {
  // make sure the handler has been registered before
  if (handler) {
    esp_websocket_client_start(client_handle.get());
    // start
  } else {
    ESP_LOGI(tag, "Register handler using 'register' before starting client.");
    return;
  }
}

bool WebsocketClient::connected() {
  return esp_websocket_client_is_connected(client_handle.get());
}

void WebsocketClient::send(const std::string &text) {
  if (connected()) {
    auto sent = esp_websocket_client_send_text(
        client_handle.get(), text.c_str(), text.length(), portMAX_DELAY);
    if (sent < 0) {
      ESP_LOGI(tag, "Websocket sent failed with error code %d", sent);
    } else {
      ESP_LOGI(tag, "Sent %d bytes of data", sent);
    }
  }
}

// return type must be changed to coroutine Return type
WebsocketClient::WaitTask WebsocketClient::send_co(const std::string &text) {
  // connected = co_await WSConnectAwaitable{}
  // the awaitable get the coroutine handle here via await_suspend, pass that
  // handle to the callback registered WSConnectAwaitable.await_resume() is
  // called here
  auto sent = esp_websocket_client_send_text(client_handle.get(), text.c_str(),
                                             text.length(), portMAX_DELAY);
  if (sent < 0) {
    ESP_LOGI(tag, "Websocket sent failed with error code %d", sent);
  } else {
    ESP_LOGI(tag, "Sent %d bytes of data", sent);
  }
}

void WebsocketClient::on_connected() {
  ws_is_connected = true;
  if (waiting_coro) {
    auto h = *waiting_coro;
    waiting_coro.reset();
    h.resume();
  }
}

struct WebsocketClient::WaitTask {
  struct promise_type {
    WebsocketClient::WaitTask get_return_object() { return {}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }
  };
};

struct WebsocketClient::ConnectAwaiter {
  WebsocketClient &client;
  bool await_ready() const noexcept { return client.ws_is_connected; }
  void await_suspend(std::coroutine_handle<> h) { client.waiting_coro = h; }
  void await_resume() const noexcept {}
};

WebsocketClient::ConnectAwaiter WebsocketClient::wait_connected() {
  return WebsocketClient::ConnectAwaiter(*this);
}
