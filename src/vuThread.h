/*	Modified March 2002
	By Christopher Steinbach
	Modified to make the threads detachable so that
	my program will stop crashing on my animations...
*/

#ifndef _VUTHREAD_H_
#define _VUTHREAD_H_

#include <stddef.h>

/* \todo implement vuThread and vuMutex for windows.

   information on thread wrapper classes for windows
   http://www.codeguru.com/system/OOThreadWrapper.html
   http://www.codeproject.com/threads/cthread.asp?print=true

*/

class vuLock;

/** Wrapper class for POSIX mutex.
    A mutex can be locked and unlocked. If a locked mutex is tried to be locked againg
    (e.g. by a parallel running thread) the thread is blocked until the mutex is free again.
    This can be used to manage access to shared resources or synchronize between different
    threads.
 */
class vuMutex
{
	friend class vuLock;
 public:
  /** Default constructor.
      Calls pthread_mutex_init() */
  vuMutex(bool recursive=false);
  /** Destructor.
      Calls pthread_mutex_destroy() */
  ~vuMutex();

  /** Locks the mutex.
      If the mutex has already been locked by another process this function waits until
      the mutex is free. This is the recommended method to synchronize between threads.
      While waiting for the mutex to get free no cpu time is waisted. */
  void lock();
  /** Try to lock the mutex.
      Same as lock() but if mutex is busy the calling thread is not blocked.
      \return true if mutex was successfully locked. false if mutex busy. */
  bool trylock();
  /** Unlock the mutex.
      Allow other threads to lock the mutex. If you forget this the program will get
      stuck... */
  void unlock();

 private:
  void * mutex;
};

/** Using this class to trigger a mutex prevents from forgetting unlock().
	Usage: { vuLock l(mutex);  ... do stuff ...; }
 */
class vuLock {
public:
	/** locks vuMutex m. */
	vuLock(vuMutex &m) : lmutex(&m) {
		lmutex->lock();
	}
	/** unlocks the mutex upon destruction */
	~vuLock() {
		lmutex->unlock();
	}
private:
	vuMutex *lmutex;
};

/** class providing basic multithreading functions

   NB - These threads are created as detached threads so that they will deallocate properely
   as the vuThread class is currently used.
*/
class vuThread
{
 public:
  //! Virtual destructor (doing nothing)
  virtual ~vuThread() {};

  /** This function is called by the user to start a new thread.
      It calls the virtual run() with parameter whatsup to allow different behaviour
      of the threads. */
  bool startThread(int whatsup, void* data=NULL);

//  void* retStartThread (int whatsup);

  /** This function is called when a new thread is started by startThread.
      The function is virtual. Override it to run your own code. The whatsup argument
      is for free use to customize behaviour of different threads. */
  virtual void run(int whatsup, void* data) = 0;

//  virtual void* retrun (int whatsup) {};

  void stopThread();

  //! sleep for n microseconds (!!not yet implemented for win32)
  static void usleep(unsigned long ms);

 protected:

 private:

#ifdef WIN32
  friend void _kickoff(void *ptr);
#else
  friend void* _kickoff(void *ptr);
#endif
  //friend void* _ret_kickoff(void *ptr);

  /** This is needed to avoid confusion in whatsup argument handling.
      This solution is not elegant, but if we want to hand over an argument to run()
      we need it. Maybe someone has a better idea... */
  vuMutex	m_WhatsupMutex;

  /** holds the argument passed to the thread started */
  int		m_Whatsup;
  /** holds the additional data passed to the thread started */
  void*         m_AdditionalData;

};

#endif
