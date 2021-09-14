GDBPORT = $(shell expr `id -u` % 5000 + 25000)
KERNEL_OPTS = console=ttyS0 spectre_v2=off l1tf=off quiet kernel.panic=-1 tsc=unstable

.PHONY: all kernel initramfs debug debug-gdb test

all: initramfs kernel

debug: initramfs kernel
	qemu-system-i386 -no-reboot -nographic -kernel linux-5.10.10/arch/x86/boot/bzImage -initrd initramfs/initramfs -append "$(KERNEL_OPTS) init=/debug-init" -gdb tcp::$(GDBPORT)

debug-gdb:
	gdb linux-5.10.10/vmlinux -ex "target remote localhost:$(GDBPORT)"

test: initramfs kernel
	qemu-system-i386 -no-reboot -nographic -kernel linux-5.10.10/arch/x86/boot/bzImage -initrd initramfs/initramfs -append "$(KERNEL_OPTS) init=/test-init"

kernel:
	$(MAKE) -C linux-5.10.10

initramfs: kernel
	$(MAKE) -C initramfs
