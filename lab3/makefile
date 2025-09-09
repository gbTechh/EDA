# Compilador y flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g

# Nombres de archivos
TARGET = octree_program
SOURCES = main.cpp octree.cpp
HEADERS = octree.h
OBJECTS = $(SOURCES:.cpp=.o)

# Regla principal
all: $(TARGET)

# Enlazar objetos para crear el ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilar cada .cpp a .o
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET)

# Limpiar archivos compilados
clean:
	rm -f $(TARGET) $(OBJECTS)

# Recompilar desde cero y ejecutar
rebuild: clean all run

.PHONY: all run clean rebuild