#ifndef NOISEDETECTIONTASK_H
#define NOISEDETECTIONTASK_H

#include "TaskTemplate.h"
#include "WebsocketClient.h"
#include "driver/i2s_std.h"
#include "hal/i2s_types.h"
#include "soc/gpio_num.h"
#include <cstdint>
#include <memory>

struct NoiseDetectionI2SConfig {
  // Configuration for I2S interface
  unsigned int sample_rate;
  gpio_num_t SD_pin;
  gpio_num_t BCKL_pin;
  gpio_num_t WS_pin;
  const i2s_data_bit_width_t bits_per_sample;
};

class NoiseDetectionTask : public TaskTemplate {
private:
  i2s_chan_handle_t read_chan;
  int data_buffer[512] = {0}; // TODO(al) figure out how big this should be
  std::shared_ptr<WebsocketClient> ws;

public:
  explicit NoiseDetectionTask();
  void task() override;
  void setup(const NoiseDetectionI2SConfig &config, const int host_port);
  void addWsHandle(std::shared_ptr<WebsocketClient> ws);
};

#endif // NOISEDETECTIONTASK_H
