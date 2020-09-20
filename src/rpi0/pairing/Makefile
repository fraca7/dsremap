
TARGET=pairing
OBJS=pairing.o utils.o config.o

CC=arm-linux-gnueabi-gcc
CFLAGS=-g -Wall -march=armv6 -mfloat-abi=soft -mfpu=vfp
LDFLAGS=

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS) *~
