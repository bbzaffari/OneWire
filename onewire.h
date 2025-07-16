// main.c - Testa se o sensor DS18B20 está presente no barramento OneWire
#pragma once
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_log.h"


//------------------------------------------------------------------------------------------------------
// 1 WIRE
// Estrutura do barramento OneWire

typedef struct {
    gpio_num_t gpio;
} onewire_t;

extern onewire_t onewire_bus;

esp_err_t onewire_init(onewire_t *ow, gpio_num_t gpio);

void onewire_delay_us(uint32_t us);

void onewire_write_bit(onewire_t *ow, uint8_t bit);

uint8_t onewire_read_bit(onewire_t *ow);

void onewire_write_byte(onewire_t *ow, uint8_t byte);

uint8_t onewire_read_byte(onewire_t *ow);

bool onewire_reset(onewire_t *ow);

void onewire_parasite_power_enable(onewire_t *ow);

void onewire_parasite_power_disable(onewire_t *ow);