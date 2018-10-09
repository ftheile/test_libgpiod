#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

static void Usage(const char* prog)
{
	printf("%s [-r <gpio_no>] | [-w <gpio_no> <delay> <count>]\n", prog);
	printf("  -r: read GPIO\n");
	printf("  -w: write GPIO\n");
	printf("  <gpio_no>: GPIO number\n");
	printf("  <delay>:   Delay in ms\n");
	printf("  <count>:   Blink count\n");
}

int main(int argc, char* argv[])
{
	if (argc > 2) {
		printf("libgpiod version: %s\n", gpiod_version_string());
		theApp.chip = gpiod_chip_open("/dev/gpiochip0");
		if (theApp.chip) {
			if (!strcmp(argv[1], "-w") && (argc == 5)) {
				// Write GPIO:
				theApp.line = gpiod_chip_get_line(theApp.chip, atoi(argv[2]));
				if (theApp.line) {
					if (gpiod_line_request_output(theApp.line, argv[0], 0) == 0) {
						int delay = 1000 * atoi(argv[3]);
						int cnt = 2 * atoi(argv[4]);
						int value = 1;
						while (cnt--) {
							if (gpiod_line_set_value(theApp.line, value) == 0) {
								usleep(delay);
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
			} else {
				if (!strcmp(argv[1], "-r") && (argc == 3)) {
					// Read GPIO:
					theApp.line = gpiod_chip_get_line(theApp.chip, atoi(argv[2]));
					if (theApp.line) {
					} else {
						perror("gpiod_chip_get_line()");
					}
				} else {
					Usage(argv[0]);
				}
			}
			gpiod_chip_close(theApp.chip);
		} else {
			perror("gpiod_chip_open()");
		}
	} else {
		Usage(argv[0]);
	}
	return 0;
}
