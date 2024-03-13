build:
	gcc -o snake main.c -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
clean:
	rm snake
run:
	./snake