BitApp: main.o bits.o bits.h
	gcc main.o bits.o -o BitApp

main.o: main.c bits.h
	gcc -c main.c

bits.o: bits.c bits.h
	gcc -c bits.c

clean:
	rm *o BitApp