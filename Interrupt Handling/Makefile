twenntyfour : main.o fifo.o actors.o 
	gcc -o twenntyfour main.o fifo.o actors.o 

main.o : main.c fifo.h actors.h
	gcc -c main.c
fifo.o : fifo.c fifo.h
	gcc -c fifo.c
actors.o : actors.c actors.h
	gcc -c actors.c
clean :
	rm twenntyfour main.o fifo.o actors.o

