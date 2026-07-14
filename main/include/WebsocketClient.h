#include "esp_event_base.h"
#include "esp_log.h"
#include "esp_log_level.h"
#include "esp_websocket_client.h"
#include <coroutine>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>

struct WaitTask {
  struct promise_type {
    WaitTask get_return_object() { return {}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::terminate(); }
  };
};

class WebsocketClient {

  struct ConnectAwaiter;

  static constexpr char *tag = "WebsocketClient";
  using ws_event_handler = void (*)(void *, esp_event_base_t, int32_t, void *);

  struct ClientHandleDeleter {
    void operator()(esp_websocket_client_handle_t handle) {
      ESP_LOGI(tag, "Destroying websocket client");
      esp_websocket_client_stop(handle);
      esp_websocket_client_destroy(handle);
    }
  };

  ws_event_handler handler{nullptr};
  std::unique_ptr<esp_websocket_client, ClientHandleDeleter> client_handle;
  bool ws_is_connected{false};
  std::optional<std::coroutine_handle<>> waiting_coro;

public:
  explicit WebsocketClient(const char *);
  void register_handler(ws_event_handler);
  void start();
  void send(const std::string &);
  WaitTask send_co(const std::string &);
  bool connected();
  void on_connected();
  ConnectAwaiter wait_connected();
};
