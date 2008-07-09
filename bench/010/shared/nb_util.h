#ifndef NB_UTIL_H
#define NB_UTIL_H
////////////////////////////////////////////////////////////////////////////////////
// Netbench 
//
// Utilities
////////////////////////////////////////////////////////////////////////////////////

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#if (QT_VERSION >= 0x040400) 
# include <QtCore/QRunnable>
#endif

#define INHERITED(c) typedef c inherited;
#define override
#if defined(_MSC_VER) // evil MSVC
  typedef long long int int64_t; 
#endif

class TimeUsedP;
class Task;

class TimeUsed {
  // #INLINE #INLINE_DUMP  ##CAT_Program computes amount of time used (in high resolution) for a given process: start the timer at the start, then do EndTimer and it computes the amount used
public:
  double 	s_used;		// #SHOW #GUI_READ_ONLY total number of seconds used
  int		n_used;		// #SHOW #GUI_READ_ONLY number of individual times the timer has been used without resetting accumulation

  void	Start(bool reset_used = true);
  // record the current time as the starting time, and optionally reset the time used information
  void	Stop();	// record the current time as the ending time, and compute difference as the time used
  void	ResetUsed();
  // reset time used information

  TimeUsed() {Initialize();}
  ~TimeUsed();
protected:
  TimeUsedP*		d; // private impl data (depends on platform)
private:
  void 	Initialize();
  void	Destroy();
  TimeUsed(const TimeUsed& cp); // not impl
};


// returns value of i before add, then adds requested amount
#if defined(Q_OS_WIN)
inline int AtomicFetchAdd(volatile int *pointer, int value)
{
    __asm {
        mov EDX,pointer
        mov ECX,value
        lock xadd dword ptr[EDX],ECX
        mov value,ECX
    }
    return value;
}
#else // unix incl Intel Mac
inline int AtomicFetchAdd(int* operand, int incr)
{
  // atomically get value of operand before op, then add incr to it
  asm volatile (
    "lock\n\t"
    "xaddl %0, %1\n" // add incr to operand
    : "=r" (incr) // incr gets replaced by the value of operand before inc
    : "m"(*operand), "0"(incr)
  );
  return incr;
}
#endif

#if (QT_VERSION < 0x040400) 
class QRunnable {
public:
  virtual void	run() = 0;
  
  QRunnable() {}
  virtual ~QRunnable() {}
};
#endif

class Task : public QRunnable {
public:
  int		task_id;
  TimeUsed	start_latency; // amount of time waiting to start (n/a for main thread)
  TimeUsed	run_time; // amount of time actually running jobs
  TimeUsed	nibble_time; // (task 0 only) time spent nibbling (if applicable)
  TimeUsed	sync_time; // (task 0 only) time spent syncing (if applicable)
  TimeUsed	overhead; // for algos with overhead, like the Send_Array
  
  Task() {task_id = -1;}
};


class QTaskThread: public QThread {
public:
  
  inline bool	isActive() const {return m_active;}
  inline bool	isSuspended() const {return m_suspended;}
  
  Task*		task() const {return m_task;}
  void		setTask(Task* t) {m_task = t;}
  
  void 		suspend();
  void		resume();
  void		terminate();
  
  QTaskThread();
protected:
  Task*		m_task;
  QMutex	mutex;
  QWaitCondition	wc;
  bool		m_suspended;
  bool		m_active;
  Qt::HANDLE	m_thread_id; // for the thread, set in run
  
  void 		run();
  void		run_impl();
};


class taPtrList_impl {
public:
  void**	el;		// #READ_ONLY #NO_SAVE #NO_SHOW the elements themselves
  int 		alloc_size;	// #READ_ONLY #NO_SAVE allocation size
  int		size;		// #READ_ONLY #NO_SAVE #SHOW number of elements in the 
  
  void		Alloc(int i);
  void		SetSize(int i); // set size, setting unused to NULL
  taPtrList_impl() {el = NULL; alloc_size = 0; size = 0;}
  virtual ~taPtrList_impl() {if (alloc_size) {delete el; el = NULL; alloc_size = size = 0;}}
protected:
  void		Add_(void* it) {Alloc(size+1); el[size] = it; ++size;}
  void*		SafeEl_(int i) const
    {if ((i >= 0) && (i < size)) return el[i]; return NULL;}
  virtual void	Release_(int /*fm*/, int /*to*/) {} // only needed for taList

};

template<class T> 
class taPtrList : public taPtrList_impl { 
// unowned instances
public:
  // element at index
  T**		Els() const		{ return (T**)el; }
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  T*		SafeEl(int i) const	{ return (T*)SafeEl_(i); }
  T*		operator[](int i) const		{ return (T*)el[i]; }
  void		Set(T* it, int i)		{  el[i] = it; }
  
  void		Add(T* it) {Add_(it);}
  taPtrList() {}
  explicit taPtrList(int alloc) {Alloc(alloc);}
};

template<class T> 
class taList : public taPtrList_impl { 
// owned instances
public:
  // element at index
  T**		Els() const		{ return (T**)el; }
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  T*		SafeEl(int i) const	{ return (T*)SafeEl_(i); }
  T*		operator[](int i) const		{ return (T*)el[i]; }
  void		Set(T* it, int i)		{  el[i] = it; }
  
  void		Add(T* it) {Add_(it);}
  T*		New(int i = 1) {while (i-- > 0) Add_(new T); return (T*)el[size - 1];}
    // make a new T and add it; return last (or only) T made
  
  taList() {}
  explicit taList(int alloc) {Alloc(alloc);}
  ~taList() {SetSize(0);}
protected:
  void		Release_(int fm, int to) 
    {while (fm <= to) {T* el = FastEl(fm); if (el) delete el; ++fm;}}

};


template<class T>
void StatFill(T* el, int size, const T& it)
    {for (int i=0; i<size; ++i) el[i] = it;}
    
template<class T>
T* crealloc(T* el, int cur_size, int new_size) {
  T* rval = (T*)realloc(el, size_t(sizeof(T) * new_size));
  if (new_size > cur_size) {
    memset(&(rval[cur_size+1]), 0, sizeof(T) * (new_size - cur_size));
  }
  return rval;
}
  
class taArray_impl {
public:

  int 			alloc_size;	// allocation size
  int			size;		// number of elements in the 
  
  taArray_impl() {alloc_size = 0; size = 0;}
protected:
};

template<class T> 
class taArray : public taArray_impl { // #INSTANCE
public:
  static const T	no_el; // for SafeEl etc.
  
  T*		el;
  // element at index
  T*		Els() const		{ return el; }
  T&		FastEl(int i) const		{ return (T&)el[i]; }
  const T&	SafeEl(int i) const { 
    if ((i >= 0) && (i < size)) return el[i]; return no_el;}
  T*		AddrEl(int i) const { 
    if ((i >= 0) && (i < size)) return (T*)&(el[i]); return NULL;}
  const T&	operator[](int i) const	{ return el[i]; }
  T&		operator[](int i) 	{ return el[i]; }
  void		Set(const T& it, int i)		{el[i] = it; }
  void		Fill(const T& it) {StatFill(el, size, it);}
  
  void		SetSize(int i) {if (alloc_size < i) Alloc(i); size = i;}
  
  taArray() {el = NULL;}
  explicit taArray(int size_) {el = NULL; SetSize(size_);}
  ~taArray() {if (alloc_size) {delete[] el; el = NULL; alloc_size = size = 0;}}
protected:
  void		Alloc(int i) {el = new T[i]; alloc_size = i;}
};

typedef taArray<float>	float_Array;
typedef taArray<int>	int_Array;

class MinMax {
public:
  float		min;	// minimum value
  float		max;	// maximum value

  void 	ApplyMinLimit(float& wt)	{ if(wt < min) wt = min; }
  void 	ApplyMaxLimit(float& wt)	{ if(wt > max) wt = max; }
  
  inline float	Range()	const		{ return (max - min); }
  inline float	Scale()	const
  { float rval = Range(); if(rval != 0.0f) rval = 1.0f / rval; return rval; }

  MinMax() {min = max = 0.0f;} 
};

class MinMaxRange: public MinMax {
public:
  float		min;	// minimum value
  float		max;	// maximum value
  float		range;		// #HIDDEN distance between min and max
  float		scale;		// #HIDDEN scale (1.0 / range)

  float	Normalize(float val) const	{ return (val - min) * scale; }
  // normalize given value to 0-1 range given current in max
  float	Project(float val) const	{ return min + (val * range); }
  // project a normalized value into the current min-max range

  MinMaxRange() {range = scale = 0.0f;} 
  void	UpdateAfterEdit() { 
    range = Range(); if (range != 0.0f) scale = 1.0f / range; }
};


class FunLookup : public float_Array {
  // ##CAT_Math function lookup for non-computable functions and optimization
  INHERITED(float_Array)
public:
  MinMaxRange	x_range;	// range of the x axis
  float		res;		// resolution of the function
  float		res_inv;	// #READ_ONLY #NO_SAVE 1/res: speeds computation because multiplies are faster than divides

  inline float	Yval(float x) const	
    // get y value at given x value (no interpolation)
    { return SafeEl( (int) ((x - x_range.min) * res_inv)); }

  inline float	Xval(int idx)	// get x value for given index position within list
  { return x_range.min + ((float)idx * res); }

  float		Eval(float x);

  virtual void	AllocForRange(); // allocate values for given range and resolution

  FunLookup();
  virtual ~FunLookup() {}
  void	UpdateAfterEdit();
};


#endif
