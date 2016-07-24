/*
* The MIT License (MIT)
* 
* Copyright (c) 2016 Johan Kanflo
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

#ifdef CONFIG_CMD_BUF_SIZE
 #define CMD_BUF_SIZE CONFIG_CMD_BUF_SIZE
#else
 #define CMD_BUF_SIZE (255)
#endif 

#ifdef CONFIG_CMD_EXTRA_PADDING
 #define CMD_EXTRA_PADDING CONFIG_CMD_EXTRA_PADDING
#else
 #define CMD_EXTRA_PADDING (4)
#endif 

#ifdef CONFIG_CMD_MAX_ARGC
 #define MAX_ARGC CONFIG_CMD_MAX_ARGC
#else
 #define MAX_ARGC (16)
#endif 

#ifdef CONFIG_PROMPT_STR
 #define PROMPT_STR CONFIG_PROMPT_STR
#else
 #define PROMPT_STR "==>"
#endif 

#ifdef CONFIG_ARG_DELIMITER_STR
 #define ARG_DELIMITER_STR CONFIG_ARG_DELIMITER_STR
#else
 #define ARG_DELIMITER_STR " "
#endif 


// Original function by Alex Stewart (@foogod)
static size_t cli_readline(char *buffer, size_t buf_size, bool echo) {
    size_t i = 0;
    int c;

    while (true) {
        c = getchar();
        if (c == '\r') {
            if (echo) putchar('\n');
            break;
        } else if (c == '\b' || c == 0x7f) {
            if (i) {
                if (echo) printf("\b \b");
                i--;
            }
        } else if (c < 0x20) {
            /* Ignore other control characters */
        } else if (i >= buf_size - 1) {
            if (echo) putchar('\a');
        } else {
            buffer[i++] = c;
            if (echo) putchar(c);
        }
    }

    buffer[i] = 0;
    return i;
}

void cli_run(const command_t cmds[], const uint32_t num, const char *app_name)
{
    char *cmd_buffer = malloc(CMD_BUF_SIZE);
    size_t len;

    if (!cmd_buffer) {
        printf("ERROR: Cannot allocate command buffer for CLI!\n");
        return;
    }

    if (app_name) {
	    printf("\nWelcome to %s!", app_name);
	} else {
	    printf("\nWelcome!");
	}
    printf(" Enter 'help' for available commands.\n\n");

    while (true) {
    	bool found_cmd = false;
        printf("%s ", PROMPT_STR);
        len = cli_readline(cmd_buffer, CMD_BUF_SIZE, true);
        if (!len) continue;
	    // Split string "<command> <argument 1> <argument 2>  ...  <argument N>"
		// into argc, argv style
	    char *argv[MAX_ARGC];
	    int argc = 1;
	    char *temp, *rover;
	    memset((void*) argv, 0, sizeof(argv));
	    argv[0] = cmd_buffer;
	    rover = cmd_buffer;
		while(argc < MAX_ARGC && (temp = strstr(rover, ARG_DELIMITER_STR))) { // @todo: fix for multiple delimiters
    		rover = &(temp[1]);
    		argv[argc++] = rover;
    		*temp = 0;
		}
        for (int i=0; i<num; i++) {
            if (cmds[i].cmd && strcmp(argv[0], cmds[i].cmd) == 0) {
            	if (cmds[i].min_arg > argc-1 || cmds[i].max_arg < argc-1) {
			        printf("Wrong number of arguments %d (%d..%d).\n", argc-1, cmds[i].min_arg, cmds[i].max_arg);
			        if (cmds[i].usage) {
			        	printf("Usage: %s%s%s\n", cmds[i].cmd, ARG_DELIMITER_STR, cmds[i].usage);
			        }
        		} else {
	            	cmds[i].handler(argc, argv);
        		}
            	found_cmd = true;
            }
        }

        if (!found_cmd) {
        	if (strcmp(argv[0], "help") == 0) {
        		uint32_t max_len = 0;
        		// Make sure command list is nicely padded
		        for (int i=0; i<num; i++) {
		            if (cmds[i].help && max_len < strlen(cmds[i].cmd)) {
		            	max_len = strlen(cmds[i].cmd);
		            }
		        }
		        for (int i=0; i<num; i++) {
		            if (cmds[i].help) {
		            	printf("%-*s %s\n", max_len+CMD_EXTRA_PADDING, cmds[i].cmd, cmds[i].help);
		            }
		        }
    		} else {
	        	printf("Unknown command\n");
    		}
        }
    }

}
