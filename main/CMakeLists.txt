idf_component_register(SRCS "App.cpp" "GPIOTask.cpp" "ListenerTask.cpp" "SignalProcessor.cpp" "utils.cpp" "NoiseDetectionTask.cpp"
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_driver_gpio core_logic esp_driver_i2s
                       INCLUDE_DIRS "include")
