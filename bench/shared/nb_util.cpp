#include "nb_util.h"

#if (defined(Q_OS_WIN))
# include <time.h>
# include <windows.h>
#else // Unix
# include <sys/time.h>
# include <sys/times.h>
# include <unistd.h>
#endif

#include <iostream>

using namespace std;

//////////////////////////
// 	TimeUsed 	//
//////////////////////////


#ifdef Q_OS_WIN

class TimeUsedP {
public:
  static LARGE_INTEGER freq;
  LARGE_INTEGER start;
  LARGE_INTEGER end;
  LARGE_INTEGER used;
  void		GetStartTime() { QueryPerformanceCounter(&start);}
  void		GetEndTime() { 
    QueryPerformanceCounter(&end);
    used.QuadPart = end.QuadPart - start.QuadPart;
    }
  double	GetTotSecs() {
    if (freq.QuadPart == 0) {
      QueryPerformanceFrequency(&freq);
    } 
    double rval = used.QuadPart / (double)(freq.QuadPart);
    return rval;}
};

LARGE_INTEGER TimeUsedHRd::freq;

#else // Unix
// computes a-b
#define ONE_MILLION 1000000
timeval timeval_diff(timeval a, timeval b) {
  a.tv_sec -= b.tv_sec;
  a.tv_usec -= b.tv_usec;
  if (a.tv_usec < 0) {
    a.tv_sec-- ;
    a.tv_usec += ONE_MILLION;
  }
  return a;
}

class TimeUsedP {
public:
  timeval	start;
  timeval	end;
  timeval	used;
  void		GetStartTime() { gettimeofday(&start, NULL);}
  void		GetEndTime() { 
    gettimeofday(&end, NULL);
    used = timeval_diff(end, start);}
  double	GetTotSecs() { 
    double rval = used.tv_usec / 1000000.0;
    if (used.tv_sec) rval += used.tv_sec;
    return rval;
    }
};
#endif // os

void TimeUsed::Initialize() {
  s_used = 0.0;
  n_used = 0;
  d = new TimeUsedP;
}

TimeUsed::~TimeUsed() {
  delete d;
  d = NULL;
}

void TimeUsed::Start(bool reset_used) {
  if (reset_used) ResetUsed();
  d->GetStartTime();
}

void TimeUsed::Stop() {
  d->GetEndTime();
  s_used += d->GetTotSecs();
  n_used++;
}

void TimeUsed::ResetUsed() {
  s_used = 0.0;
  n_used = 0;
}


// timing code
////////////////////////////////////////////////////////////////////////////////////

// core version 
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QCoreApplication>
#include <pthread.h>
#include <sys/signal.h>


QTaskThread::QTaskThread() {
  m_task = NULL;
  m_thread_id = 0;
  m_active = true;
  m_suspended = true;
}

void QTaskThread::suspend() {
  if (m_suspended) return;
  mutex.lock();
  m_suspended = true;
  mutex.unlock();
}

void QTaskThread::resume() {
  if (!m_suspended) return;
  mutex.lock();
  m_suspended = false;
  m_task->start_latency.Start(false);
  wc.wakeAll();;
  mutex.unlock();
}

void QTaskThread::run() {
  m_thread_id = currentThreadId();
  while (m_active) {
    mutex.lock();
    while (m_suspended)
      wc.wait(&mutex);
      
    if (m_task) {
      m_task->start_latency.Stop();
      m_task->run_time.Start(false);
      m_task->run();
      m_task->run_time.Stop();
    }
    m_suspended = true;
    if (!m_active) break;
    mutex.unlock();
  }
}

void QTaskThread::terminate() {
  if (!m_active) return;
  
  mutex.lock();
  m_active = false;
  m_suspended = false;
  setTask(NULL);
  wc.wakeAll();;
  mutex.unlock();
//  QThread::terminate(); //WARNING: including this causes the dude to hang
}

/*
void DeleteThreads() {
  for (int t = n_procs - 1; t >= 1; t--) {
    QTaskThread* th = (QTaskThread*)threads[t];
    if (th->isActive()) {
      th->terminate();
    }
    while (!th->isFinished());
    delete th;
  }
}
*/


void taPtrList_impl::Alloc(int new_alloc) {
  if (alloc_size >= new_alloc)	return;	// no need to increase..
  int old_alloc_sz = alloc_size;
  if (new_alloc == (alloc_size +1))
    new_alloc = max(16, (alloc_size * 2));
  alloc_size = new_alloc;
  if (!el) {
    el = (void**)malloc(alloc_size * sizeof(void*));
    if (!el) {
      cerr << "taPtrList_impl::Alloc -- malloc failed -- pointer list is too long! could be fatal.\n";
      alloc_size = 0;
    }
  }
  else {
    void** nwel = (void**)realloc((char *) el, alloc_size * sizeof(void*));
    if(!nwel) {
      cerr << "taPtrList_impl::Alloc -- realloc failed -- pointer list is too long! could be fatal.\n";
      alloc_size = old_alloc_sz;
    }
    el = nwel;
  }
}


