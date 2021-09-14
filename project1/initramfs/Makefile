.PHONY: all clean

INITRAMFS := initramfs

INIT_OBJECTS := debug-init test-init
TRAFFICSIM_OBJECTS := trafficsim trafficsim-mutex trafficsim-strict-order

all: $(INITRAMFS)

clean:
	rm -f $(INITRAMFS) $(INIT_OBJECTS) $(TRAFFICSIM_OBJECTS) *.d

-include $(INIT_OBJECTS:=.d)
-include $(TRAFFICSIM_OBJECTS:=.d)

$(INITRAMFS): $(INIT_OBJECTS) $(TRAFFICSIM_OBJECTS)
	ls $(INIT_OBJECTS) $(TRAFFICSIM_OBJECTS) | cpio -H newc -o -F $(INITRAMFS)

%: %.c
	$(CC) -O0 -g3 -m32 $< -lm -static -I ../linux-5.10.10/arch/x86/include/generated/uapi -I ../linux-5.10.10/arch/x86/include/uapi -MMD -o $@
