##
# TODO: Enter your project's name here
#
# @file
# @version 0.1

CFLAGS = -Wall -Wextra -lraylib -lm

main: main.c
	gcc main.c -o main $(CFLAGS)

clean:
	rm -f ./main

# end
