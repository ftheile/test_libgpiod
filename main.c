#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

// Data for the client.
// Conventions:
// - struct in C == class in C++
// - client's member functions are always named "client_..."
// - first argument to client's member functions is always
//     struct client* me
// - me-pointer in C == this-pointer in C++
struct client {
	struct gpiod_chip* chip;
	struct gpiod_line* line;
};

// Static, local instance for now...:
static struct client theApp;

int main(int argc, char* argv[])
{
	printf("Version: %s\n", gpiod_version_string());
	theApp.chip = gpiod_chip_open("/dev/gpiochip0");
	if (theApp.chip) {
		theApp.line = gpiod_chip_get_line(theApp.chip, 4);
		if (theApp.line) {
			if (gpiod_line_request_output(theApp.line, argv[0], 0) == 0) {
				if (gpiod_line_set_value(theApp.line, 1) == 0) {
					usleep(5000*1000);
				} else {
					printf("gpiod_line_set_value() error\n");
				}
				gpiod_line_release(theApp.line);
			} else {
				printf("gpiod_line_request_output() error\n");
			}
		} else {
			printf("gpiod_chip_get_line() error\n");
		}
		gpiod_chip_close(theApp.chip);
	} else {
		printf("gpiod_chip_open() error\n");
	}
	return 0;
}
