#ifndef USBSPI_H
#define USBSPI_H
#include <stdint.h>
void send_usb(uint8_t cmd);
void ss_disable();
void ss_enable();
void spi_disable();
void spi_enable();
void prog_disable();
void prog_enable();
void spi_read32(char *buffer, uint8_t buflen);
void spi_send4 (uint32_t data);
uint8_t spi_send1 (uint8_t data);
uint8_t read_gpi();
void led_on();
void led_off();

#endif
