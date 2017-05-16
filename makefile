CC=g++
CFLAGS=-Wall -I. -std=c++11
DEPS=symbol.h 
OBJ=main.o symbol.o

program: $(OBJ)
    $(CC) -o $@ $^ -I.
%.o:%.cpp $(DEPS)
    $(CC) -o $@ -c $< $(CFLAGS)
.PHONY:clean
clean:
    rm -f *.o
    rm -f program

