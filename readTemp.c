#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mraa.h"

#define MAXTIMINGS 85
#define DHTPIN 11
char dht11_dat[5] = {0,0,0,0,0};
mraa_result_t r = MRAA_SUCCESS;
mraa_gpio_context gpio;

void read_dht11_dat(void)
{
  uint8_t laststate = 1;
  uint8_t counter = 0;
  uint8_t j = 0, i;
  float f; // fahrenheit

  dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

  // pull pin down for 18 milliseconds
  r = mraa_gpio_dir(gpio, MRAA_GPIO_OUT_HIGH);
  if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
  }

  r = mraa_gpio_write(gpio, 0);
  usleep(19000);
  // then pull it up for 40 microseconds
  r = mraa_gpio_write(gpio, 1);
  usleep(40);

  r = mraa_gpio_dir(gpio, MRAA_GPIO_IN);
  if (r != MRAA_SUCCESS) {
        mraa_result_print(r);
  }
  
  // detect change and read data
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while (mraa_gpio_read(gpio) == laststate) {
      counter++;
      usleep(1);
      
      if (counter == 255) {
        break;
      }
    }

    laststate = mraa_gpio_read(gpio);

    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // show each bit into the storage bytes
      dht11_dat[j/8] <<= 1;
      if (counter >2)
        dht11_dat[j/8] |= 1;
      j++;
    }
  }

  // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
  // print it out if data is good
  if ((j >= 40) && 
      (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)) ) {
    f = dht11_dat[2] * 9. / 5. + 32;
    printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n", 
            dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f);
  }
  else
  {
    for(int i= 0; i < 5;i++)
    {
      printf("%d\t",dht11_dat[i]);
    }
    printf("\n");
    printf("Data not good, skip\n");
  }
}

int main (void)
{

  printf ("Raspberry Pi MRAA DHT11 Temperature test program\n") ;

  if (mraa_init() == -1)
    exit (1) ;

  fprintf(stdout, "MRAA Version: %s\nStarting Blinking on IO%d\n", mraa_get_version(), DHTPIN);
  
  gpio = mraa_gpio_init(DHTPIN);
  if (gpio == NULL) {
        fprintf(stderr, "Are you sure that pin%d you requested is valid on your platform?", DHTPIN);
        exit(1);
  }
  printf("Initialised pin%d\n", DHTPIN);

  while (1) 
  {
     read_dht11_dat();
     usleep(100000);
  }
  return 0 ;
}
