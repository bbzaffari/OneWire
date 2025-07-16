# OneWire Driver (ESP-IDF, C)

**This code is shared publicly with no copyrights; anyone is free to use, modify, or redistribute it.**

This driver was developed by me, Bruno Bavaresco Zaffari, as part of my undergraduate final thesis project in Computer Engineering. It is one of the modules included in the `TCC` directory of the main repository, serving as a key component for managing low-level hardware communication in the final system. Everyone is free to use, modify, and adapt this code as they wish, with no need for copyrights.

## What is OneWire?

OneWire is a communication protocol developed by Dallas Semiconductor (now Maxim Integrated) that allows data and power transmission over a **single data wire plus ground**. This reduces wiring complexity, making it ideal for embedded systems where space and cost are constrained.

**Key characteristics:**

* Uses a **single bi-directional line** (open-drain) with pull-up resistor.
* Supports **multiple devices** on the same bus via unique 64-bit addresses.
* Requires **precise timing** to differentiate between logical '1' and '0'.
* Typical applications: temperature sensors (e.g., DS18B20), ID chips (e.g., DS2401), EEPROMs, and battery monitoring ICs.

In essence, OneWire allows many devices to communicate over just one wire by sending short, timed pulses that represent commands and data.

## How does this driver work?

This driver, written in C using the ESP-IDF framework, manages all low-level OneWire operations:

### Why use esp\_rom\_delay\_us instead of vTaskDelay or other delays?

In the OneWire driver, precise timing at the microsecond level is critical. Commands like writing or reading a bit require specific pulse durations (e.g., 6 µs, 60 µs, 480 µs) to meet the OneWire protocol specification.

The ESP-IDF provides several delay mechanisms:

* **vTaskDelay()** → delays in milliseconds, designed for task scheduling, not for precise hardware timing.
* **ets\_delay\_us() / esp\_rom\_delay\_us()** → busy-wait microsecond delays, ensuring precise short waits without yielding control.

In this driver, **esp\_rom\_delay\_us()** is used because:
* It provides the required microsecond accuracy.
* It avoids the variability and scheduler influence of FreeRTOS delays.
* It's lightweight and minimal, suitable for tight hardware loops.

If slower delays like vTaskDelay were used, the OneWire signal would break, causing devices to miss or misinterpret commands. That's why microsecond-level busy-wait delays are essential for reliable communication.

### Functions explained

\**onewire\_init(onewire\_t *ow, gpio\_num\_t gpio)**

* Configures the GPIO pin as **open-drain with pull-up**, preparing it for OneWire communication.
* Sets direction, pull-up/pull-down, and interrupt types.

\**onewire\_reset(onewire\_t *ow)**

* Sends a **reset pulse** (drive low \~480µs), then waits for devices to respond with a **presence pulse** (\~60–240µs low).
* Returns `true` if at least one device responds.

\**onewire\_write\_bit(onewire\_t *ow, uint8\_t bit)**

* Writes a single bit by **pulling the line low** and releasing it at precise times depending on whether writing '1' or '0'.
* For '1': short low pulse; for '0': long low pulse.

\**onewire\_read\_bit(onewire\_t *ow)**

* Reads a bit by pulling the line low briefly, then switching to input and sampling the line after \~9µs.

\**onewire\_write\_byte(onewire\_t *ow, uint8\_t byte)**

* Writes a byte, bit by bit, using `onewire_write_bit()` 8 times (LSB first).

\**onewire\_read\_byte(onewire\_t *ow)**

* Reads a byte by calling `onewire_read_bit()` 8 times and assembling the bits (LSB first).

**onewire\_delay\_us(uint32\_t us)**

* Provides precise microsecond delays, essential for meeting OneWire protocol timings.

## Practical example

1. Initialize bus:

```c
onewire_t ow;
onewire_init(&ow, GPIO_NUM_4);
```

2. Reset and check presence:

```c
if (onewire_reset(&ow)) {
    printf("Device detected!\n");
}
```

3. Write and read:

```c
onewire_write_byte(&ow, 0xCC);  // Skip ROM command
uint8_t temp = onewire_read_byte(&ow);
```

## Learning points

* OneWire relies heavily on **software timing**; hardware timers or precise delay functions are critical.
* You must understand **GPIO modes**: input, output, open-drain.
* Bus must have a **pull-up resistor** (usually 4.7kΩ) to work properly.
* Useful for building sensors, identification systems, and low-cost data networks.

---
If you would like, you can also contribute to this project. Feel free to fork the repository, suggest improvements, report issues, or submit pull requests. Any feedback or contribution is welcome to help improve the code and documentation. Thank you for your interest and support!
