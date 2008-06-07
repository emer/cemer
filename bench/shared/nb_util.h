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


class Task {
public:
  int		task_id;
  int		proc_id; // current proc being run
  
  virtual void	run() = 0;
  
  Task() {task_id = -1; proc_id = 0;}
  virtual ~Task();
};

class QTaskThread: public QThread {
public:
  TimeUsed	start_latency; // amount of time waiting to start
  TimeUsed	run_time; // amount of time actually running jobs
  
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
  
  void		Alloc(int i) {el = new void*[i]; alloc_size = i;}
  taPtrList_impl() {el = NULL; alloc_size = 0; size = 0;}
  ~taPtrList_impl() {if (alloc_size) {delete[] el; el = NULL; alloc_size = size = 0;}}
};

template<class T> 
class taPtrList : public taPtrList_impl { // #INSTANCE
public:
  // element at index
  T**		Els() const		{ return (T**)el; }
  T*		FastEl(int i) const		{ return (T*)el[i]; }
  T*		operator[](int i) const		{ return (T*)el[i]; }
  void		Set(T* it, int i)		{  el[i] = it; }
  taPtrList() {}
  explicit taPtrList(int alloc) {Alloc(alloc);}
};


#endif
