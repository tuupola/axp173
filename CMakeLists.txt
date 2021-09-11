if (ESP_PLATFORM)
    idf_component_register(
        SRCS "axp173.c"
        INCLUDE_DIRS "."
    )
    add_definitions("-DAXP173_INCLUDE_SDKCONFIG_H")
endif()
