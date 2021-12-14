all: unitests

CC = g++ -O2
FLAGS = -std=c++14 -c -g #-Wall

OBJS1 = Unitest.o functions.o discretef.o

EXEC1 = unitests

$(EXEC1): $(OBJS1)
	$(CC)  $(OBJS1) ../Fred-master/src/*.cpp -o $(EXEC1) -lcunit

discretef.o: ../curves/discretef.cpp
	$(CC) $(FLAGS) ../curves/discretef.cpp

functions.o: ../functions/functions.cpp
	$(CC) $(FLAGS) ../functions/functions.cpp

Unitest.o: Unitest.cpp
	$(CC) $(FLAGS) Unitest.cpp

clean:
	rm -f $(EXEC1) $(OBJS1)
