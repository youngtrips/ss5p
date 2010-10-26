OBJS=server.o \
	 network.o \
	 handle_accept.o \
	 handle_connect.o \
	 utils.o \
	 protocol.o \
	 buffer.o \
	 wrapper.o

TARGET=server
CFLAGS=-g
LIBS=
CC=gcc

$(TARGET): $(OBJS)
			 $(CC) -o $(TARGET) $(OBJS) $(LIBS) $(CFLAGS)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o -f
