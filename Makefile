ifdef PS5_PAYLOAD_SDK
include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
else
$(error PS5_PAYLOAD_SDK is undefined)
endif

CFLAGS := -Wall -Wextra -O2 -Isource
TARGET := ps5-disc-fallback-probe.elf
OBJS := source/main.o source/notify.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^
	$(STRIP) $@

clean:
	rm -f $(OBJS) $(TARGET)
