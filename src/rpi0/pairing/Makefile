
TARGET=pairing
OBJS=pairing.o utils.o config.o

CC=gcc
CFLAGS=-O3 -Wall
LDFLAGS=

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS) *~
