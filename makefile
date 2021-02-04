build:
	gcc -c functions.c -o functions.o
	gcc -c new.c -o new.o
	gcc new functions.o new.o -lm

dev:
	gcc -c encode.c
	gcc -o encode encode.o -lm

clean:
	rm -f *.o new encode cryptedText.txt

cb: clean build

crypt:
	./encode < text.txt > cryptedText.txt

de:
	./new 4 crpt2 words.txt

# decrypt:
# 	./new < cryptedText.txt > decryptedText.txt

gitconf:
	git config --global user.name "Lior Rotberg"
	git config --global user.email Exolite@gmail.com
