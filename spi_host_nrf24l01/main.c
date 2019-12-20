/* Name: nrf24le1 Downloader USBasp 1.0
   основан на проекте https://github.com/derekstavis/nrf24le1-libbcm2835 .
   страничка проекта http://homes-smart.ru/index.php/oborudovanie/bez-provodov-2-4-ggts/55-programmirovanie-nrf24le1-cherez-usbasp
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb.h>        /* this is libusb */
#include "opendevice.h" /* common code moved to separate module */
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "usbspi.h"

#include "rf/src/rf.h"

#if HIDMODE /* device's VID/PID and names */
#include "../firmware/usbconfigHID.h"
#else
#include "../firmware/usbconfig.h"
#endif

uint8_t rf_irq_pin_active();
void delay_us(uint32_t us);
usb_dev_handle      *handle = NULL;
char                buffer[32];
int                 cnt;


uint8_t RF_CE=0;
uint8_t RF_CSN=0;
void rf_clear_csn(){set_csn(0);}
void rf_set_csn(){set_csn(1);}
uint8_t rf_get_csn(){return RF_CSN;}
//-----------------------------------------------------------
int main(int argc, char **argv) {
  //usb_dev_handle      *handle = NULL;
  const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
  char                vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
  //char                buffer[8];

  int                  vid, pid;

  usb_init();
  /* compute VID/PID from usbconfig.h so that there is a central source of information */
  vid = rawVid[1] * 256 + rawVid[0];
  pid = rawPid[1] * 256 + rawPid[0];
  /* The following function is in opendevice.c: */
  if(usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL) != 0){
    fprintf(stderr, "Could not find USB device \"%s\" with vid=0x%x pid=0x%x\n", product, vid, pid);
    exit(1);
  }


  /*
  uint8_t bufread[17000];
  unsigned long off = 0;
  size_t count =16384;

  memset(bufread, 0, sizeof(bufread));

  nrf24le1_init();

  enable_program(1);

  if(strcasecmp(argv[1], "test") == 0) da_test_show();

  else
    if(strcasecmp(argv[1], "write") == 0){

      nrf_restore_data(bufread, count, "./main.bin");
      uhet_write(bufread, 16384, &off);


    } else 
      if(strcasecmp(argv[1], "read") == 0){


        memset(bufread, 0, sizeof(bufread));
        uhet_read(bufread, count, &off);

        nrf_save_data(bufread, count, "./main-dump.bin");

      }

  //da_erase_all_store();

  enable_program(0);

  wiring_destroy();
  */
  /*
  uint8_t n=32;
  uint32_t n4=0x12345678;
  struct timespec ts={0,500000000};
  uint8_t b1;
  uint8_t b2;
  uint8_t b3;
  uint8_t b4;
  while(1){
    ss_enable();
    spi_send4(n4);
    n4++;

    b1=spi_send1(n);
    n++;

    b2=spi_send1(n);
    n++;

    b3=spi_send1(n);
    n++;

    b4=spi_send1(n);
    n++;
    printf("%08x -> %02x %02x %02x %02x.\n",n,b1,b2,b3,b4);

    spi_read32(buffer,sizeof(buffer));

    ss_disable();
    nanosleep(&ts,NULL);
    printf("\n");
  }
  */
  /*
  int x;
  while(1){
    usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
     15 , 0, 0, NULL, 0, 5000);
    x=read_gpi();
    printf("GPI %d\n",x&1);
    prog_enable();
    sleep(1);
    usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
     14 , 0, 0, NULL, 0, 5000);
    x=read_gpi();
    printf("GPI %d\n",x&1);

    prog_disable();
    sleep(1);
  }
  exit;
  */

  unsigned char arr[5]={0x21,0x22,0x23,0x24,0x25};
//  rf_write_register(0x0a, arr, 5);
  arr[0]=0x04;
  rf_write_register(0x00, arr, 1);
  delay_us(50000);
  arr[0]=0x06;
  rf_write_register(0x00, arr, 1);
  delay_us(50000);



  led_on();
#if 1
  rf_configure_debug_lite(false, 1); //initialize the rf to the debug configuration as TX, 1 data bytes, and auto-ack disabled
  delay_us(130); //wait for remote unit to come from standby to RX

  exit;

  unsigned int count; //counter for for loop
  //main program loop
  uint8_t txn=0;
  while(1) {
    txn++;
    printf("tx %04x %8d Status %02x\n",txn, count, rf_get_status());

    rf_write_tx_payload(&txn, 1, true); //transmit received char over RF

    //wait until the packet has been sent or the maximum number of retries has been reached
    //delay_us(1000);
    while(!(rf_irq_pin_active() && rf_irq_tx_ds_active()));

    led_off();
    rf_irq_clear_all(); //clear all interrupts in the 24L01
    rf_set_as_rx(true); //change the device to an RX to get the character back from the other 24L01

    //wait a while to see if we get the data back (change the loop maximum and the lower if
    //  argument (should be loop maximum - 1) to lengthen or shorten this time frame
    for(count = 0; count < 250; count++) {
      //check to see if the data has been received.  if so, get the data and exit the loop.
      //  if the loop is at its last count, assume the packet has been lost
      //  If neither of these is true, keep looping.
      //delay_us(1000);
      if((rf_irq_pin_active() && rf_irq_rx_dr_active())) {
        unsigned char datavar; //register to data sent and received
        rf_read_rx_payload(&datavar, 1); //get the payload into data
        printf(" rx %04x",datavar);
        led_on();
        //gpio_pin_val_sbit_set(P0_SB_D1);

        goto success;
      }

      //if loop is on its last iteration, assume packet has been lost.
      /*
         if(count == 24999)
         gpio_pin_val_sbit_clear(P0_SB_D1);
         */

    }
    led_off();
    
success:

    rf_irq_clear_all(); //clear interrupts again

    rf_set_as_tx(); //resume normal operation as a TX
    delay_us(1300); //wait for remote unit to come from standby to RX

    //gpio_pin_val_sbit_complement(P0_SB_D0); //toggle the output pin as an indication that the loop has completed

  }
#endif


  unsigned char x;
  /*
    ss_enable();
    x=spi_send1(0xff);
    printf("%02x: ",x);
    ss_disable();
    */


  /*
    ss_enable();
    x=spi_send1(0x2a);
    printf("%02x: ",x);
    x=spi_send1(0x01);
    x=spi_send1(0x02);
    x=spi_send1(0x03);
    x=spi_send1(0x04);
    x=spi_send1(0x05);
    ss_disable();
*/

  while(0){
    ss_enable();
    x=spi_send1(0x0a);
    printf("%02x: ",x);
    for(int i=0;i<=4;i++){
      x=spi_send1(0x00);
      printf("%02x ",x);
    }
    printf("\n");

    ss_disable();
    sleep(1);
  }



  usb_close(handle);
  return 0;
}

void rf_irq_clear_all(void){
  char x=0b01110000;
  rf_write_register(RF_STATUS,&x,1);

};

uint8_t rf_irq_pin_active(){
  return !read_gpi();
}

void set_ce(uint8_t CE){
  RF_CE=CE;
  if(CE)
    prog_enable();
  else
    prog_disable();
}


void set_csn(uint8_t CSN){
  RF_CSN=CSN;
  if(!CSN)
    ss_enable();
  else
    ss_disable();
}

void rf_spi_configure_enable(void){
}
void rf_power_up_param(bool rx_active_mode, uint8_t config){
}

uint8_t rf_spi_send_read_byte(uint8_t tosend){
  return spi_send1(tosend);
}

void delay_us(uint32_t us) {
  struct timespec ts={0,us*1000};
  nanosleep(&ts,NULL);
}

