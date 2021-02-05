build:
	gcc -c functions.c
	gcc -c new.c
	gcc -o new functions.o new.o -lm

dev:
	gcc -c encode.c
	gcc -o encode encode.o -lm

clean:
	rm -f *.o ./new ./encode cryptedText.txt

cb: clean build

crypt:
	./encode < text.txt > cryptedText.txt

run4:
	mpiexec ./new 4 cryptedText.txt words.txt

run2:
	./new 2 cryptedText.txt words.txt

run1:
	./new 1 cryptedText.txt words.txt

all4: clean dev crypt build run4

all2: clean dev crypt build run2

all1: clean dev crypt build run1

de:
	./new 4 crpt2 words.txt

# decrypt:
# 	./new < cryptedText.txt > decryptedText.txt

git:
	git config --global user.name "Lior Rotberg"
	git config --global user.email Exolite@gmail.com
