#include <unistd.h>
#include <stdio.h>
#include <linux/reboot.h>
#include <asm/unistd.h>

#include "cs1550-spawn.h"

void reboot()
{
        syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART, NULL);
}

static void process_option()
{
	int option;

	puts("1) trafficsim 2) trafficsim-mutex 3) trafficsim-strict-order 4) exit");
	printf("Select an option: ");
	scanf("%d", &option);

	switch (option) {
	case 1:
		spawn("/trafficsim");
		break;
	case 2:
		spawn("/trafficsim-mutex");
		break;
	case 3:
		spawn("/trafficsim-strict-order");
		break;
	case 4:
		reboot();
		break;
	default:
		puts("Unknown option\n");
		break;
	}

	puts("");
}

int main()
{
	while (1)
		process_option();

	return 0;
}
