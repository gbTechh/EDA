CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
TARGET = programa

SOURCES = main.cpp lista.cpp init.cpp

OBJECTS = $(SOURCES:.cpp=.o)

# Regla principal para compilar el programa
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para limpiar
clean:
	rm -f $(OBJECTS) $(TARGET)

# Nueva Regla para compilar y ejecutar
run: $(TARGET)
	./$(TARGET)

.PHONY: clean run