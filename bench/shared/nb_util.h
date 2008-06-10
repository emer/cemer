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

class TimeUsedP;

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
# error not defined for Windows yet
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
  int		proc_id; // current proc being run
  TimeUsed	start_latency; // amount of time waiting to start (n/a for main thread)
  TimeUsed	run_time; // amount of time actually running jobs
  
  Task() {task_id = -1; proc_id = 0;}
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
  void*		SafeEl_(int i) 
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


class taArray_impl {
public:
  int 		alloc_size;	// #READ_ONLY #NO_SAVE allocation size
  int		size;		// #READ_ONLY #NO_SAVE #SHOW number of elements in the 
  
  taArray_impl() {alloc_size = 0; size = 0;}
};

template<class T> 
class taArray : public taArray_impl { // #INSTANCE
public:
  T*		el;
  // element at index
  T*		Els() const		{ return el; }
  T&		FastEl(int i) const		{ return (T*)el[i]; }
  const T&	operator[](int i) const	{ return el[i]; }
  T&		operator[](int i) 	{ return el[i]; }
  void		Set(const T& it, int i)		{el[i] = it; }
  
  void		SetSize(int i) {if (i < alloc_size) Alloc(i); size = i;}
  
  taArray() {el = NULL;}
  explicit taArray(int size_) {el = NULL; SetSize(size_);}
  ~taArray() {if (alloc_size) {delete[] el; el = NULL; alloc_size = size = 0;}}
protected:
  void		Alloc(int i) {el = new T[i]; alloc_size = i;}
};


#endif
