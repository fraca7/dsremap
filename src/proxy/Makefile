
TARGET=proxy
L2CAP_PROXY_SRC=../l2cap_proxy
OBJS=proxy.o optparse.o listen.o channel.o imu.o cpp/IMUIntegrator.o vm.o cpp/VM.o $(L2CAP_PROXY_SRC)/l2cap_con.o $(L2CAP_PROXY_SRC)/bt_utils.o

CC=gcc
CPPFLAGS=-I$(L2CAP_PROXY_SRC) $(shell pkg-config --cflags-only-I gio-2.0 bluez) -DG_LOG_DOMAIN=\"dsremap\" -DTARGET_PC -DTIMINGS
#CFLAGS=-O3 -Wall $(shell pkg-config --cflags-only-other gio-2.0 bluez)
#CFLAGS=-fsanitize=address -O1 -fno-omit-frame-pointer -g -Wall $(shell pkg-config --cflags-only-other gio-2.0 bluez)
CFLAGS=-Wall $(shell pkg-config --cflags-only-other gio-2.0 bluez)
CXXFLAGS=$(CFLAGS)
LDFLAGS=$(shell pkg-config --libs-only-L gio-2.0 bluez)
LIBS=$(shell pkg-config --libs-only-l gio-2.0 bluez) -lstdc++ -lz

all: release

release: CFLAGS+=-O3
release: $(TARGET)

debug: CFLAGS+=-g
debug: $(TARGET)

asan: CFLAGS+=-fsanitize=address -fno-omit-frame-pointer -g
asan: LDFLAGS+=-lasan
asan: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET) *~
