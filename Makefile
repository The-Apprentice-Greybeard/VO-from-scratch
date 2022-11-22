CXX := g++

CFLAGS := -std=c++17
INCLUDE := -Iglad/include
LIBS := -lglfw

vofs: vofs.o glad.o
	$(CXX) vofs.o glad.o $(LIBS) -o vofs

vofs.o: main.cc
	$(CXX) -c main.cc $(CFLAGS) $(INCLUDE) -o vofs.o

glad.o:
	$(CXX) -c glad/src/glad.c $(CFLAGS) $(INCLUDE) -o glad.o

.PHONY: clean
clean:
	rm -f glad.o vofs.o vofs
