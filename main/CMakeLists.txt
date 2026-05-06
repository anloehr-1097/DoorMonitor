idf_component_register(SRCS "App.cpp" "GPIOTask.cpp" "ListenerTask.cpp" "SignalProcessor.cpp"
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_driver_gpio core_logic
                       INCLUDE_DIRS "include")
