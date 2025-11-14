# Makefile para tests - compilar cada test individualmente
CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -I..

# Ejecutables de cada test
TEST_VECTOR = test_vector
TEST_LISTA = test_lista  
TEST_HEAP = test_heap
TEST_HASHMAP = test_hashmap

# Compilar todos los tests
all: $(TEST_VECTOR) $(TEST_LISTA) $(TEST_HEAP) $(TEST_HASHMAP)
	@echo "✅ Todos los tests compilados"

# Cada test se compila POR SEPARADO con su propio main
$(TEST_VECTOR): vector.cpp ../vector.h
	@echo "🔨 Compilando test_vector..."
	$(CXX) $(CXXFLAGS) -o $@ vector.cpp

$(TEST_LISTA): lista.cpp ../lista.cpp ../lista.h ../data.h ../vector.h
	@echo "🔨 Compilando test_lista..."
	$(CXX) $(CXXFLAGS) -o $@ lista.cpp ../lista.cpp

$(TEST_HEAP): heap.cpp ../vector.h ../minheap.h
	@echo "🔨 Compilando test_heap..."
	$(CXX) $(CXXFLAGS) -o $@ heap.cpp

$(TEST_HASHMAP): hashmap.cpp ../lista.cpp ../lista.h ../data.h ../hashmap.h ../vector.h
	@echo "🔨 Compilando test_hashmap..."
	$(CXX) $(CXXFLAGS) -o $@ hashmap.cpp ../lista.cpp

# Ejecutar todos los tests
test: all
	@echo "🧪 EJECUTANDO TODOS LOS TESTS..."
	@echo "=================================="
	@echo "1. Test de Vector:"
	@./$(TEST_VECTOR) && echo "✅ test_vector pasó" || echo "❌ test_vector falló"
	@echo "=================================="
	@echo "2. Test de Lista:"
	@./$(TEST_LISTA) && echo "✅ test_lista pasó" || echo "❌ test_lista falló"
	@echo "=================================="
	@echo "3. Test de Heap:"
	@./$(TEST_HEAP) && echo "✅ test_heap pasó" || echo "❌ test_heap falló"
	@echo "=================================="
	@echo "4. Test de HashMap:"
	@./$(TEST_HASHMAP) && echo "✅ test_hashmap pasó" || echo "❌ test_hashmap falló"
	@echo "=================================="
	@echo "🎉 Ejecución completada"

# Ejecutar tests individuales
run-vector: $(TEST_VECTOR)
	@./$(TEST_VECTOR)

run-lista: $(TEST_LISTA)
	@./$(TEST_LISTA)

run-heap: $(TEST_HEAP)
	@./$(TEST_HEAP)

run-hashmap: $(TEST_HASHMAP)
	@./$(TEST_HASHMAP)

# Limpiar
clean:
	@echo "🧹 Limpiando ejecutables..."
	@rm -f $(TEST_VECTOR) $(TEST_LISTA) $(TEST_HEAP) $(TEST_HASHMAP)

# Ayuda
help:
	@echo "Uso:"
	@echo "  make all     - Compilar todos los tests"
	@echo "  make test    - Compilar y ejecutar todos los tests"
	@echo "  make run-*   - Ejecutar test individual"
	@echo "  make clean   - Limpiar ejecutables"

.PHONY: all test run-* clean help