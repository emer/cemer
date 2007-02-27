template<class T, TypeDef* td = &TA_int>
class MyClassT { 
public:
  T val;
  void GetType(TypeDef* typ = td);
};

typedef MyClassT<int, TA_float> myguy;
