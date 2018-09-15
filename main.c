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
	int dummy;  // no members yet, but struct can't be empty...
};

// Static, local instance for now...:
static struct client theApp;

// Called after manipulating the GPIO,
// before clean up by libgpiod
static void client_wait(struct client* me)
{
	usleep(5000*1000);
}

int main(int argc, char* argv[])
{
	printf("Version: %s\n", gpiod_version_string());
	// Set line 4 on gpiochip0 'high' and call client_wait() afterwards: 
	gpiod_ctxless_set_value("/dev/gpiochip0", 4, 1, false, argv[0], client_wait, &theApp);
	return 0;
}
