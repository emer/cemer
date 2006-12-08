class MyClass { 
public:
  int val;
};

class nxClass { 
public:
  int val3;
};

class vpClass : virtual public MyClass, virtual public nxClass { 
public:
  int val2;
};
