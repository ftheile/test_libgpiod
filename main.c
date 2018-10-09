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
	printf("%s [-r <gpio_no> <count>] | [-w <gpio_no> <count> <delay>]\n", prog);
	printf("  -r:        read GPIO\n");
	printf("  -w:        write GPIO\n");
	printf("  <gpio_no>: GPIO number\n");
	printf("  <count>:   No. of times\n");
	printf("  <delay>:   Delay in ms\n");
}

int main(int argc, char* argv[])
{
	if (argc > 3) {
		printf("libgpiod version: %s\n", gpiod_version_string());
		theApp.chip = gpiod_chip_open("/dev/gpiochip0");
		if (theApp.chip) {
			theApp.line = gpiod_chip_get_line(theApp.chip, atoi(argv[2]));
			if (theApp.line) {
				int cnt = atoi(argv[3]);
				if (!strcmp(argv[1], "-w") && (argc == 5)) {
					// Write GPIO:
					if (!gpiod_line_request_output(theApp.line, argv[0], 0)) {
						int delay = 1000 * atoi(argv[4]);
						int value = 1;
						cnt *= 2;
						do {
							if (!gpiod_line_set_value(theApp.line, value)) {
								usleep(delay);
							} else {
								perror("gpiod_line_set_value()");
							}
							value = !value;
						} while (cnt--);
						gpiod_line_release(theApp.line);
					} else {
						perror("gpiod_line_request_output()");
					}
				} else {
					if (!strcmp(argv[1], "-r") && (argc == 4)) {
						// Read GPIO:
						if (!gpiod_line_request_falling_edge_events(theApp.line, argv[0])) {
							do {
								int r = gpiod_line_event_wait(theApp.line, NULL);
								if (r == 1) {
									struct gpiod_line_event evt;
									if (!gpiod_line_event_read(theApp.line, &evt)) {
										printf("Event!\n");
									} else {
										perror("gpiod_line_event_read()");
									}
								} else {
									if (r == 0) {
										printf("Timeout\n");
									} else {
										perror("gpiod_line_event_wait()");
									}
								}
							} while (cnt--);
							gpiod_line_release(theApp.line);
						} else {
							perror("gpiod_line_request_falling_edge_events()");
						}
					} else {
						Usage(argv[0]);
					}
				}
			} else {
				perror("gpiod_chip_get_line()");
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
