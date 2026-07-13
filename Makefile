CFLAGS = -Wall -Wextra -lraylib -lm

SOURCE = main.c game.c
TARGET = main

$(TARGET): $(SOURCE)
	gcc $^ -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET)
