#include "NoiseDetectionTask.h"
#include "driver/gpio.h"
#include "driver/i2s_common.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"
#include "esp_err.h"
#include "hal/gpio_types.h"
#include "hal/i2s_types.h"
#include "include/utils.h"
#include "portmacro.h"
#include "soc/clk_tree_defs.h"
#include <iostream>

NoiseDetectionTask::NoiseDetectionTask() = default;

void NoiseDetectionTask::setup(const NoiseDetectionI2SConfig &config,
                               const int host_port) {
  // TODO(al) Initialize driver: master, receiver, (DMA?), rx (= receiver)
  // config

  // TODO(al) In theory need to verify that host_port is valid, but for now just
  // assume it is
  i2s_chan_config_t chan_config =
      I2S_CHANNEL_DEFAULT_CONFIG(host_port, I2S_ROLE_MASTER);
  i2s_new_channel(&chan_config, NULL, &read_chan);

  // Communication config
  i2s_std_config_t comm_config = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(config.sample_rate),
      .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(config.bits_per_sample,
                                                      I2S_SLOT_MODE_MONO),
      .gpio_cfg = {.mclk = I2S_GPIO_UNUSED,
                   .bclk = config.BCKL_pin,
                   .ws = config.WS_pin,
                   .dout = I2S_GPIO_UNUSED,
                   .din = config.SD_pin,
                   .invert_flags = {}}};
  comm_config.clk_cfg.mclk_multiple =
      I2S_MCLK_MULTIPLE_384; // need for 24 bit compatibility with the mic we
                             // use

  gpio_set_pull_mode(config.SD_pin, GPIO_PULLDOWN_ONLY);
  esp_err_t err = i2s_channel_init_std_mode(read_chan, &comm_config);
  handle_error(err);
  // enable channel -> ready to read data
  err = i2s_channel_enable(read_chan);
  handle_error(err);
}

void NoiseDetectionTask::task() {

  size_t bytes_read = 0;
  while (true) {
    esp_err_t err =
        i2s_channel_read(read_chan, data_buffer, sizeof(data_buffer),
                         &bytes_read, portMAX_DELAY);
    handle_error(err);

    std::cout << "Read " << bytes_read << " bytes from I2S channel.\n"
              << "Error code:  " << err << std::endl;
    std::cout << "First 100 samples: ";
    for (int i = 0; i < 100 && i < bytes_read / sizeof(int); i++) {
      std::cout << std::hex << (data_buffer[i] >> 8) << " ";
    }
    std::cout << std::endl;

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
