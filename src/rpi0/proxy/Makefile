
TARGET=proxy
OBJS=proxy.o optparse.o listen.o channel.o imu.o cpp/IMUIntegrator.o vm.o cpp/VM.o ../l2cap_proxy/l2cap_con.o ../l2cap_proxy/bt_utils.o

CC=gcc
CPPFLAGS=-I../l2cap_proxy $(shell pkg-config --cflags-only-I gio-2.0 bluez) -DG_LOG_DOMAIN=\"dsremap\" -DTARGET_PC
CFLAGS=-O3 -Wall $(shell pkg-config --cflags-only-other gio-2.0 bluez)
CXXFLAGS=$(CFLAGS)
LDFLAGS=$(shell pkg-config --libs-only-L gio-2.0 bluez)
LIBS=$(shell pkg-config --libs-only-l gio-2.0 bluez) -lstdc++

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET) *~
