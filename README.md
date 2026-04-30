# Door Sensor

A door sensor system using ESP32-S3 to monitor door events via reed and infrared sensors on GPIO pins. Planned: Audio sensor for voice/breath detection.

## Hardware Requirements

- ESP32-S3 microcontroller
- Reed sensor
- Infrared sensor
- Wires and breadboard

Future: Audio sensor (TBD, e.g., I2S microphone).

## Software Setup

Build with ESP-IDF and CMake.

## Usage

Run `App.cpp`. Sensors print the current GPIO levels (0 or 1) to the console.

- Reed sensor: Polls every 100ms.
- Infrared sensor: Task-based, reads every 1s, uses shared buffer.

## Features

- **Reed Sensor**: Connected to GPIO 39, polled in main loop.
- **Infrared Sensor**: Connected to GPIO 21, handled by dedicated task.
- **Core Components** (implemented from scratch):
  - `RingBuffer`: Fixed-size circular buffer for FIFO data queuing.
  - `SharedRingContext`: Thread-safe wrapper around the RingBuffer with semaphore for inter-task communication.

## Future

Audio sensor integration alongside existing sensors, with processing to filter for human voice/breath (details TBD).
