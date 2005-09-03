#ifndef TEST_H
#define TEST_H

//#include "ta_string.h"

#include "test_TA_type.h"

class MyClass {
public:
  MyClass() {}
  virtual ~MyClass() {}
};

class MySubClass: public MyClass {
public:
  MySubClass() {}
};

template<class T> 
class MyTemplate {
public:
  T   myVar;
  virtual void  setVar(const T& val) {myVar = val;}
  virtual void	Set(int i, const T& item) 	{myVar = val;}
  virtual ~MyTemplate() {}
};

class MyTemplate_MyClass: public MyTemplate<MyClass> {
public:
  MyTemplate_MyClass() {}
};

#endif
