#include <stdio.h>
#include <gpiod.h>

int main(int argc, char* argv[])
{
	printf("Version: %s\n", gpiod_version_string());
	return 0;
}
