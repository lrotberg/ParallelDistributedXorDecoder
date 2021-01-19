build:
	gcc -c new.c
	gcc -o new new.o -lm

clean:
	rm -f *.o new crpt decrypt.txt

cb: clean build

cry:
	./new < text.txt > crpt

de:
	./new crpt2 words.txt

dec:
	./new < crpt > decrypt.txt
