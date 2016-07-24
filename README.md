### CLI framework for ESP Open RTOS

Most of my ESP8266 projects requires a command line interface during development and I found myself copying a somewhat subpar implementation around making lots of changes for each project. As a remedy I created this ESP Open RTOS (EOR) extras. Usage is simple, declare a command struct, write handler functions and call ```cli_run(...)```.

First clone this repository:

```
export EOR_ROOT=/path/to/esp-open.rtos
cd $EOR_ROOT
git clone https://github.com/kanflo/eor-cli.git extras/cli
cd extras/cli/example
make -j8 && make flash
```

Then add the commands you need:

```
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
```

The framework takes care of validating the number of arguments before calling your handler taking part of the burden of argument checking from your sholders.