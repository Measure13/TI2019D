idf_component_register(SRCS "Rainmaker_driver.c" "Rainmaker.c"
                   REQUIRES esp_rainmaker esp_schedule rmaker_common app_insights app_wifi SPI_AD9833
                    INCLUDE_DIRS "include")
