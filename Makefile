CC = gcc
CFLAGS = -Wall -Wextra -std=c11

OBJ = main.o utils.o
EXEC = sed_simplified

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

test: $(EXEC)
	./$(EXEC) input.txt 's/old/new/'
