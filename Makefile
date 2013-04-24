CC=gcc
OBJ=bot.o main.o
NAME=bot
CFLAGS=-c

all: $(NAME)

debug: CFLAGS += -DDEBUG
debug: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f *.o
