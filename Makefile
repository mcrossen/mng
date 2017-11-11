CC          = aarch64-elf-gcc
LD          = aarch64-elf-ld
AS          = aarch64-elf-gcc
OBJCOPY     = aarch64-elf-objcopy
OBJDUMP     = aarch64-elf-objdump
GDB         = aarch64-elf-gdb
INCLUDES    = -I "lib" -I "src"
CFLAGS      = -g -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -c
ASFLAGS     = $(CFLAGS)
LDFLAGS     = -nostdlib --no-undefined -nostartfiles
BUILD_DIR   = build
OUT_DIR     = out
TARGET      = $(OUT_DIR)/kernel
OBJECTS     = build/boot.o build/main.o build/text.o build/display.o build/font.o
LINKER      = link.ld
.SUFFIXES:  .o .s .c .elf .img .list

all: $(TARGET).img $(TARGET).list

$(BUILD_DIR)/%.o: src/%.s $(BUILD_DIR)
	$(AS) $(ASFLAGS) $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: src/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: lib/%.s $(BUILD_DIR)
	$(AS) $(ASFLAGS) $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: lib/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: lib/%.psf $(BUILD_DIR)
	$(LD) -r -b binary -o $@ $<

$(TARGET).elf: $(OBJECTS) $(LINKER) $(OUT_DIR)
	$(LD) $(LDFLAGS) -T $(LINKER) $(OBJECTS) -o $@

.elf.img:
	$(OBJCOPY) $< -O binary $@

.elf.list:
	$(OBJDUMP) -d $< > $@

$(BUILD_DIR):
	mkdir $@

$(OUT_DIR):
	mkdir $@

debug: all
	@echo "launching emulator"
	qemu-system-aarch64 -cpu cortex-a53 -M versatilepb -m 1G -kernel $(TARGET).img -s -S &
	@echo "waiting for one second"
	sleep 1
	@echo "launching debugger"
	$(GDB) $(TARGET).elf -ex "target remote localhost:1234"

clean:
	-$(RM) -r $(BUILD_DIR)
	-$(RM) -r $(OUT_DIR)
