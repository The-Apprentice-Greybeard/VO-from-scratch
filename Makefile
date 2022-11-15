CXX := g++

INCLUDE := -Iglad/include
LIBS := -lglfw

vofs: vofs.o glad.o
	$(CXX) vofs.o glad.o $(LIBS) -o vofs

vofs.o: main.cc
	$(CXX) -c main.cc $(INCLUDE) -o vofs.o

glad.o:
	$(CXX) -c glad/src/glad.c $(INCLUDE) -o glad.o
