CC = gcc
CFLAGS = -Wall
RM = rm
RMFLAGS = -rf
OBJS = bar.o foo.o

main: $(OBJS)
	$(CC) -o $@ $^

bar.o: bar.c bar.h
	$(CC) $(CFLAGS) -c bar.c -o bar.o

foo.o: foo.c foo.h bar.h
	$(CC) $(CFLAGS) -c foo.c -o foo.o

clean:
	$(RM) $(RMFLAGS) $(OBJS)


