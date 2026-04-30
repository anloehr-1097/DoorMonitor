idf_component_register(SRCS "App.cpp" "InfraredTask.cpp" "ListenerTask.cpp"
                       PRIV_REQUIRES spi_flash
                       REQUIRES esp_driver_gpio core_logic
                       INCLUDE_DIRS "include")
