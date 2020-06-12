#include <iostream>
#include <string.h>
using namespace std;

class A {
public:
  virtual int f() = 0;
};
class B : public A {
public:
  int f() {
    cout << b << "B\n";
    return 1;
  }

protected:
  static int b;
};
class C : public B {
public:
  C() { b++; }
  int f() {
    cout << b << "C\n";
    return 2;
  }
};

int B::b = 0;
int main() {
  B *b = new C;
  cout << b->f() << " ";
  A *a = new C;
  cout << a->f() << " ";
  delete a;
  delete b;
  return 0;
}
