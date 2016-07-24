/* Command line interface
 *
 * This sample code is in the public domain.
 */

#include <stdlib.h>
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"
#include "ota-tftp.h"
#include "cli.h"
#include <ssid_config.h>


void on_cmd(uint32_t argc, char *argv[])
{
  for (int i=1; i<argc; i++) {
    uint32_t gpio = atoi(argv[i]);
    printf(" Turning on GPIO %d\n", gpio);
    gpio_enable(gpio, GPIO_OUTPUT);
    gpio_write(gpio, true);

  }
}

void off_cmd(uint32_t argc, char *argv[])
{
  for (int i=1; i<argc; i++) {
    uint32_t gpio = atoi(argv[i]);
    printf(" Turning off GPIO %d\n", gpio);
    gpio_enable(gpio, GPIO_OUTPUT);
    gpio_write(gpio, false);
  }
}

void cli_task(void *pvParameters) {
  const command_t cmds[] = {
    {
      .cmd = "on",
      .handler = &on_cmd,
      .min_arg = 1, .max_arg = 16,
      .help = "Turn on one or more GPIOs",
      .usage = "<gpio> [<gpio>]*",
    },
    {
      .cmd = "off",
      .handler = &off_cmd,
      .min_arg = 1, .max_arg = 16,
      .help = "Turn off one or more GPIOs",
      .usage = "<gpio> [<gpio>]*",
    }
  };
  cli_run(cmds, sizeof(cmds) / sizeof(command_t), "the GPIO demo");
}

void user_init(void)
{
    uart_set_baud(0, 115200);

#ifndef CONFIG_NO_WIFI
  // Wifi not necessary for the CLI demo but I use OTA for flashing
    ota_tftp_init_server(TFTP_PORT);
    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };
    // required to call wifi_set_opmode before station_set_config
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);
#endif // CONFIG_NO_WIFI

    xTaskCreate(cli_task, (signed char *)"cli_task", 512, NULL, 2, NULL);
}
