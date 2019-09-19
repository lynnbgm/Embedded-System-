# Simply execute "make" in working directory to create executable
# Running "make clean" will remove the executable

CXX = gcc
CXXFLAGS = -std=c99
edit :  SimpleISS

SimpleISS: SimpleISS.o
	$(CXX) $(CXXFLAGS) -o SimpleISS SimpleISS.o

SimpleISS.o: SimpleISS.c
	$(CXX) $(CXXFLAGS) -c SimpleISS.c

clean:
	rm -rf SimpleISS *.o

