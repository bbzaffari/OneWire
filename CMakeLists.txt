idf_component_register(
    SRCS onewire.c
    INCLUDE_DIRS .
    REQUIRES driver freertos log  esp_timer
)
