include ../common.mk

SDK := D:/esp8266/ESP8266_NONOS_SDK
SDK_INC := $(SDK)/include
SDK_DRIVER_INC := $(SDK)/driver_lib/include

XTENSA := D:/esp8266/xtensa-lx106-elf
CC := $(XTENSA)/bin/xtensa-lx106-elf-gcc
AR := $(XTENSA)/bin/xtensa-lx106-elf-ar
LD := $(XTENSA)/bin/xtensa-lx106-elf-gcc
SIZE := $(XTENSA)/bin/xtensa-lx106-elf-size

CC_FLAGS := -g -O0 -std=gnu99 -Wpointer-arith -Wundef -Werror -Wl,-EL \
	-fno-inline-functions -nostdlib -mlongcalls \
	-mtext-section-literals -mno-serialize-volatile -D__ets__ \
	-DICACHE_FLASH -c

LD_FLAGS := -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,\
	-static -Wl,--start-group -lc -lgcc -lphy -lpp -lnet80211 \
	-llwip -lwpa -lmain -lcrypto
	
ESPTOOL := esptool.py

.PHONY: all clean

src/main.o: src/main.c inc/user_config.h
	@echo "CC src/main.o"
	@$(CC) -Iinc -I$(SDK_INC) -I$(SDK_INC)/json -I$(SDK_DRIVER_INC) $(CC_FLAGS) src/main.c -o src/main.o

build/app_app.a: src/main.o src/lcd.o src/i2c_user.o
	@echo "AR build/app_app.a"
	@$(AR) cru build/app_app.a src/main.o src/lcd.o src/i2c_user.o

src/lcd.o: src/lcd.c inc/user_config.h
	@echo "CC src/lcd.o"
	@$(CC) -Iinc -I$(SDK_INC) -I$(SDK_INC)/json -I$(SDK_DRIVER_INC) $(CC_FLAGS) src/lcd.c -o src/lcd.o
	
src/i2c_user.o: src/i2c_user.c inc/user_config.h
	@echo "CC src/lcd.o"
	@$(CC) -Iinc -I$(SDK_INC) -I$(SDK_INC)/json -I$(SDK_DRIVER_INC) $(CC_FLAGS) src/i2c_user.c -o src/i2c_user.o
	
build/app.out: build/app_app.a
	@echo "LD build/app.out"
	@$(LD) -L$(SDK)/lib -T$(SDK)/ld/eagle.app.v6.ld $(LD_FLAGS) build/app_app.a $(SDK)/lib/libdriver.a \
	-Wl,--end-group -o build/app.out

all: build/app.out
	@echo "SIZE build/app.out"
	@$(SIZE) build/app.out
	@echo "ESPTOOL build/app.out-0x00000.bin build/app.out-0x10000.bin"
	@$(ESPTOOL) elf2image build/app.out
	
flash:
	@$(ESPTOOL) -p $(PORT) -b $(BAUDRATE) write_flash --no-compress -ff 40m -fm qio -fs 32m 0x00000 build/app.out-0x00000.bin 0x10000 build/app.out-0x10000.bin

clean:
	@rm -v src/main.o src/lcd.o src/i2c_user.o build/app_app.a build/app.out build/app.out-0x00000.bin build/app.out-0x10000.bin