# Makefile para tests - tu estructura específica
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0

# Ejecutables de cada test
TEST_VECTOR = test_vector
TEST_LISTA = test_lista  
TEST_LISTAMAP = test_lista_map  
TEST_MAP = test_map  
TEST_HASHMAP = test_hashmap
TEST_TOPIC = test_topic
TEST_INIT = test_init

# Compilar todos los tests
all: $(TEST_VECTOR) $(TEST_LISTA) $(TEST_LISTAMAP) $(TEST_MAP) $(TEST_HASHMAP) $(TEST_TOPIC) $(TEST_INIT)
	@echo "✅ Todos los tests compilados"

# Cada test se compila incluyendo TODAS las dependencias necesarias
$(TEST_VECTOR): vector.cpp ../vector.h
	@echo "🔨 Compilando test_vector..."
	$(CXX) $(CXXFLAGS) -o $@ vector.cpp

$(TEST_LISTA): lista.cpp ../lista.h ../data.h ../vector.h ../lista.cpp
	@echo "🔨 Compilando test_lista..."
	$(CXX) $(CXXFLAGS) -o $@ lista.cpp ../lista.cpp

$(TEST_LISTAMAP): listamap.cpp ../listamap.h ../data.h ../vector.h
	@echo "🔨 Compilando test_lista_map..."
	$(CXX) $(CXXFLAGS) -o $@ listamap.cpp

$(TEST_MAP): map.cpp ../map.h ../data.h ../vector.h
	@echo "🔨 Compilando test_map..."
	$(CXX) $(CXXFLAGS) -o $@ map.cpp

$(TEST_HASHMAP): hashmap.cpp ../hashmap.h ../lista.h ../data.h ../vector.h ../lista.cpp
	@echo "🔨 Compilando test_hashmap..."
	$(CXX) $(CXXFLAGS) -o $@ hashmap.cpp ../lista.cpp

$(TEST_TOPIC): topic.cpp ../topic.h ../lista.h ../data.h ../hashmap.h ../minaux.h ../vector.h ../lista.cpp
	@echo "🔨 Compilando test_topic..."
	$(CXX) $(CXXFLAGS) -o $@ topic.cpp ../lista.cpp

$(TEST_INIT): init.cpp ../init.h ../topic.h ../lista.h ../data.h ../hashmap.h ../minaux.h ../vector.h ../lista.cpp
	@echo "🔨 Compilando test_init..."
	$(CXX) $(CXXFLAGS) -o $@ init.cpp ../lista.cpp

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
	@echo "3. Test de Lista Map:"
	@./$(TEST_LISTAMAP) && echo "✅ test_lista_map pasó" || echo "❌ test_lista_map falló"
	@echo "=================================="
	@echo "4. Test de Map:"
	@./$(TEST_MAP) && echo "✅ test_map pasó" || echo "❌ test_map falló"
	@echo "=================================="
	@echo "6. Test de HashMap:"
	@./$(TEST_HASHMAP) && echo "✅ test_hashmap pasó" || echo "❌ test_hashmap falló"
	@echo "=================================="
	@echo "7. Test de Topic:"
	@./$(TEST_TOPIC) && echo "✅ test_topic pasó" || echo "❌ test_topic falló"
	@echo "=================================="
	@echo "8. Test de Init:"
	@./$(TEST_INIT) && echo "✅ test_init pasó" || echo "❌ test_init falló"
	@echo "=================================="
	@echo "🎉 Ejecución completada"

# Ejecutar tests individuales
run-vector: $(TEST_VECTOR)
	@./$(TEST_VECTOR)

run-lista: $(TEST_LISTA)
	@./$(TEST_LISTA)

run-listamap: $(TEST_LISTAMAP)
	@./$(TEST_LISTAMAP)

run-map: $(TEST_MAP)
	@./$(TEST_MAP)

run-hashmap: $(TEST_HASHMAP)
	@./$(TEST_HASHMAP)

run-topic: $(TEST_TOPIC)
	@./$(TEST_TOPIC)

run-init: $(TEST_INIT)
	@./$(TEST_INIT)

# Limpiar
clean:
	@echo "🧹 Limpiando ejecutables..."
	@rm -f $(TEST_VECTOR) $(TEST_LISTA) $(TEST_LISTAMAP) $(TEST_MAP) $(TEST_HASHMAP) $(TEST_TOPIC) $(TEST_INIT)

# Ayuda
help:
	@echo "Uso:"
	@echo "  make all     - Compilar todos los tests"
	@echo "  make test    - Compilar y ejecutar todos los tests"
	@echo "  make run-*   - Ejecutar test individual"
	@echo "  make clean   - Limpiar ejecutables"

.PHONY: all test run-* clean help