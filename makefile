CC=g++
CFLAGS=-Wall -I. -std=c++11
DEPS=TS_entry.h line.h util.h
OBJ=main.o TS_entry.o line.o util.o

program: $(OBJ)
	$(CC) -o $@ $^ -I.
%.o:%.cpp $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
.PHONY:clean
clean:
	rm -f *.o
	rm -f program

