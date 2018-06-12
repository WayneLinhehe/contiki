/**
 * \file
 *         Common code between collect sensor data from Arduino
 * \author
 *         White.H
 */

#include "contiki.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/serial-line.h"
#include <stdio.h>
#include <stdint.h>

static struct etimer et;
static char* collect_data;

/*------------------------------------------------------------------*/
PROCESS(collect_sensor_data, "sensor data");
AUTOSTART_PROCESSES(*collect_sensor_data);
/*------------------------------------------------------------------*/

unsigned int
uart1_send_bytes(const unsigned char *s, unsigned int len) 
{
  unsigned int i = 0;

  while(s && *s != 0) {
    if(i >= len) {
      break;
    }
    uart_write_byte(1, *s++);
    i++;
  }
  return i;
}

PROCESS_THREAD(collect_sensor_data, ev, data)
{
  char *rxdata;
  static uint16_t count=0;
  char string[20];

  PROCESS_BEGIN();
  uart_set_input(1, serial_line_input_byte);
  etimer_set(&et, CLOCK_SECOND * 2);
  leds_toggle(LEDS_GREEN);

  while(1) {
    PROCESS_WAIT_UNTIL(etimer_expired(&et));
    sprintf(string, "sending string %u.\n", ++count);
    uart1_send_bytes((uint8_t *)string, sizeof(string) - 1);
    etimer_reset(&et);
    printf("string is being sent\n");

    PROCESS_YIELD();
    if(ev == serial_line_event_message) {
      leds_toggle(LEDS_RED);

      rxdata = data;
      printf("Data received over UART %s\n", rxdata);

      // the data send to node.c, then compress into packet.
      collect_data_send(rxdata);

      collect_data = malloc(strlen(rxdata) +1); // allocation memeory locate.
      strcpy(collect_data, rxdata); // copy data to collect_data.
      printf("command_data: %s\n", command_data);
      
      PRINTF("Received Done.");
    }
  }
}
