#include "../vector.h"

#include <cassert>
#include <iostream>

void test_cvector() {
  std::cout << "=== TESTING CVECTOR ===" << std::endl;

  // Test 1: Constructor y push_back
  CVector<int> vec;
  vec.push_back(1);
  vec.push_back(2);
  vec.push_back(3);
  assert(vec.size() == 3);
  assert(vec[0] == 1);
  assert(vec[1] == 2);
  assert(vec[2] == 3);
  std::cout << "✓ Test 1 pasado: push_back básico" << std::endl;

  // Test 2: Redimensionamiento automático
  CVector<int> vec2;
  for (int i = 0; i < 10; i++) {
    vec2.push_back(i);
  }
  assert(vec2.size() == 10);
  assert(vec2[9] == 9);
  std::cout << "✓ Test 2 pasado: redimensionamiento" << std::endl;

  // Test 3: push_front
  CVector<int> vec3;
  vec3.push_front(3);
  vec3.push_front(2);
  vec3.push_front(1);
  assert(vec3.size() == 3);
  assert(vec3[0] == 1);
  assert(vec3[1] == 2);
  assert(vec3[2] == 3);
  std::cout << "✓ Test 3 pasado: push_front" << std::endl;

  // Test 4: pop_back y pop_front
  CVector<int> vec4;
  vec4.push_back(1);
  vec4.push_back(2);
  vec4.push_back(3);

  vec4.pop_back();
  assert(vec4.size() == 2);
  assert(vec4[0] == 1);
  assert(vec4[1] == 2);

  vec4.pop_front();
  assert(vec4.size() == 1);
  assert(vec4[0] == 2);
  std::cout << "✓ Test 4 pasado: pop_back y pop_front" << std::endl;

  // Test 5: Copy constructor
  CVector<int> vec5;
  vec5.push_back(10);
  vec5.push_back(20);
  CVector<int> vec6(vec5);
  assert(vec6.size() == 2);
  assert(vec6[0] == 10);
  assert(vec6[1] == 20);
  std::cout << "✓ Test 5 pasado: copy constructor" << std::endl;

  // Test 6: Assignment operator
  CVector<int> vec7;
  vec7 = vec5;
  assert(vec7.size() == 2);
  assert(vec7[0] == 10);
  assert(vec7[1] == 20);
  std::cout << "✓ Test 6 pasado: assignment operator" << std::endl;
  // TEst 7: push front y pop_back y pop_front
  CVector<int> vec8;
  vec8.push_front(3);
  vec8.push_front(2);
  vec8.push_front(1);
  vec8.push_back(4);
  vec8.push_back(5);
  vec8.push_back(6);
  assert(vec8.size() == 6);
  assert(vec8[0] == 1);
  assert(vec8[1] == 2);
  assert(vec8[2] == 3);
  assert(vec8[3] == 4);
  assert(vec8[4] == 5);
  assert(vec8[5] == 6);
  vec8.pop_front();
  assert(vec8[0] == 2);
  vec8.push_front(1);
  assert(vec8[0] == 1);
  vec8.pop_back();
  assert(vec8.size() == 5);
  assert(vec8[4] == 5);
  vec8.push_back(7);
  assert(vec8[5] == 7);

  std::cout << "✓ Test 7 pasado: push front y pop_back y pop_front"
            << std::endl;
  std::cout << "✅ TODOS LOS TESTS PASADOS!" << std::endl;
}

int main() {
  test_cvector();
  return 0;
}