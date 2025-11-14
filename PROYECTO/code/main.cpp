#include "init.h"

using namespace std;

int main() {

  Init init = {1, 20, 3, 100, 0};
  CInit app(init);
  app.run();
  app.print();

  return 0;
}