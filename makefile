CC = g++
CFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = 
SRC = $(wildcard *.cpp)
OBJ = $(filter-out main.o, $(SRC:.cpp=.o))
TARGET = app.exe

all: $(TARGET)

$(TARGET): $(OBJ) main.o
	$(CC) $(CFLAGS) $(OBJ) main.o -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) main.o $(TARGET)

.PHONY: all clean