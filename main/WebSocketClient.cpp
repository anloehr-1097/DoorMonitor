#include "WebsocketClient.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "portmacro.h"
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
                                handler, nullptr);
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
