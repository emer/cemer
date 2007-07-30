class MyClass {
public:
  int my_int;
  float my_float;
  void myFunc(int myParam);
  MyClass();
};

class MySubClass: public MyClass {
public:
  int my_int2;
  float my_float2;
  void myFunc2(int myParam);
  MySubClass();
};

