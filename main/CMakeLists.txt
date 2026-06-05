idf_component_register(SRCS "App.cpp" "GPIOTask.cpp" "ListenerTask.cpp" "SignalProcessor.cpp" "utils.cpp" "NoiseDetectionTask.cpp" "WifiConnectTask.cpp" "TimeSyncTask.cpp"
                       PRIV_REQUIRES spi_flash esp_wifi lwip nvs_flash
                       REQUIRES esp_driver_gpio core_logic esp_driver_i2s 
                       INCLUDE_DIRS "include")
