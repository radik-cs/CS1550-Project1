#include <linux/reboot.h>
#include <asm/unistd.h>

#include "cs1550-spawn.h"

void reboot()
{
	syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART, NULL);
}

int main()
{
	spawn("/trafficsim");
	spawn("/trafficsim-mutex");
	spawn("/trafficsim-strict-order");

	reboot();

	return 0;
}
