#include "usbspi.h"
#include <netinet/in.h>
#include <usb.h>

extern usb_dev_handle *handle;
//extern char buffer[32];

/*
NAME
usb_control_msg - Builds a control urb, sends it off and waits for completion
SYNOPSIS

int usb_control_msg(struct usb_device * dev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void * data, __u16 size, int timeout);

ARGUMENTS

dev pointer to the usb device to send the message to
pipe endpoint "pipe" to send the message to
request USB message request value
requesttype USB message request type value
value USB message value
index USB message index value
data pointer to the data to send 
size length in bytes of the data to send
timeout time in msecs to wait for the message to complete before timing out (if 0 the wait is forever)
    */
void send_usb(uint8_t cmd) {
  usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
      cmd, 0, 0, NULL, 0, 5000);
}

uint8_t read_gpi() {
  char buffer[1];
  usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
      12, 0, 0, buffer, 1, 5000);
  return buffer[0];
}

void led_on() {
  send_usb(15);
}

void led_off() {
  send_usb(14);
}

void prog_disable() {
  send_usb(10);
}

void prog_enable() {
  send_usb(11);
}

void ss_disable() {
  send_usb(20);
}

void ss_enable() {
  send_usb(21);
}

void spi_disable() {
  send_usb(40);
}

void spi_enable() {
  send_usb(41);
}

void spi_read32(char *buffer, uint8_t buflen) { //reads 32 bytes
  usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
      51, 0, 0, buffer, buflen, 5000);
}

void spi_send4 (uint32_t data){
  usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
      52, htons(data >> 16), htons(data & 0xffff), NULL, 0, 5000);
}

uint8_t spi_send1 (uint8_t data){ // отправка массива из 4 байт
  char buffer[1];
  uint16_t d=data;
  usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
      2, d, 0, buffer, 1, 5000);
  return buffer[0];
}

