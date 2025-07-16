#include "onewire.h"

/// Initialize the OneWire bus on a given GPIO (open-drain, with pull-up)
esp_err_t onewire_init(onewire_t *ow, gpio_num_t gpio) {
    ow->gpio = gpio;

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,  // open-drain mode
        .pin_bit_mask = 1ULL << gpio,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };

    esp_err_t err = gpio_config(&io_conf);
    return err;
}

/// Microsecond-level delay (critical for OneWire timing)
/// We use esp_rom_delay_us() instead of vTaskDelay() to ensure precise hardware timing
void onewire_delay_us(uint32_t us) {
    esp_rom_delay_us(us);
}

/// Write one bit to the OneWire bus with precise pulse lengths
void onewire_write_bit(onewire_t *ow, uint8_t bit) {
    gpio_set_direction(ow->gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(ow->gpio, 0);
    if (bit) {
        onewire_delay_us(6);    // short low pulse for '1'
        gpio_set_level(ow->gpio, 1);
        onewire_delay_us(64);
    } else {
        onewire_delay_us(60);   // long low pulse for '0'
        gpio_set_level(ow->gpio, 1);
        onewire_delay_us(10);
    }
}

/// Read one bit from the OneWire bus with precise sampling
uint8_t onewire_read_bit(onewire_t *ow) {
    uint8_t bit = 0;
    gpio_set_direction(ow->gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(ow->gpio, 0);
    onewire_delay_us(6);    // brief low pulse
    gpio_set_level(ow->gpio, 1);
    gpio_set_direction(ow->gpio, GPIO_MODE_INPUT);
    onewire_delay_us(9);    // wait before sampling
    bit = gpio_get_level(ow->gpio);
    onewire_delay_us(55);   // recovery time
    return bit;
}

/// Write one byte, least significant bit first
void onewire_write_byte(onewire_t *ow, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        onewire_write_bit(ow, byte & 0x01);
        byte >>= 1;
    }
}

/// Read one byte, least significant bit first
uint8_t onewire_read_byte(onewire_t *ow) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte >>= 1;
        if (onewire_read_bit(ow)) {
            byte |= 0x80;
        }
    }
    return byte;
}

/// Send reset pulse and detect device presence on the OneWire bus
bool onewire_reset(onewire_t *ow) {
    gpio_set_direction(ow->gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(ow->gpio, 0);
    onewire_delay_us(480);   // hold low for reset
    gpio_set_level(ow->gpio, 1);
    gpio_set_direction(ow->gpio, GPIO_MODE_INPUT);
    onewire_delay_us(70);    // wait for presence pulse
    bool presence = !gpio_get_level(ow->gpio);
    onewire_delay_us(410);   // recovery time
    return presence;
}
