#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <errno.h>

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
				int value = 1;
				for (int i=0; i<10; i++) {
					if (gpiod_line_set_value(theApp.line, value) == 0) {
						usleep(250*1000);
					} else {
						perror("gpiod_line_set_value()");
					}
					value = !value;
				}
				gpiod_line_release(theApp.line);
			} else {
				perror("gpiod_line_request_output()");
			}
		} else {
			perror("gpiod_chip_get_line()");
		}
		gpiod_chip_close(theApp.chip);
	} else {
		perror("gpiod_chip_open()");
	}
	return 0;
}
