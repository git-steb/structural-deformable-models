#include "vuThread.h"
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <fstream>

using namespace std;

vuMutex logmut;

vuMutex& getLogMut() {return logmut;}

#ifndef WIN32

#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

void* _kickoff(void* ptr)
{
    vuThread *th = (vuThread*)ptr;
    int   whatsup = th->m_Whatsup;
    void* data    = th->m_AdditionalData;
    th->m_WhatsupMutex.unlock();

    th->run(whatsup, data);

    th->stopThread();
    return NULL;
}

/*
  void* _ret_kickoff(void *ptr)
  {
  vuThread *th = (vuThread*)ptr;
  int whatsup = th->m_Whatsup;
  th->m_WhatsupMutex.unlock();

  return th->retrun(whatsup);

//  return NULL;
}*/

bool vuThread::startThread(int whatsup, void* data)

{
    pthread_t *thread = new pthread_t;

    m_WhatsupMutex.lock();
    m_Whatsup        = whatsup;
    m_AdditionalData = data;

    pthread_attr_t tattr;

//  cout << "here" << endl;

    pthread_attr_setdetachstate (&tattr, PTHREAD_CREATE_DETACHED);

//  cout << "s1" << endl;

    int t = pthread_create(thread, NULL, &_kickoff, (void*) this);

    if (t != 0)

    {
//      cout << "s2.5" << endl;
        cout << strerror (errno) << endl;
        if (t == EAGAIN)
            cout << "eagain" << endl;

        return false;
    }

    pthread_detach (*thread);

//  cout << "s3" << endl;

    return true;
}

/*bool vuThread::retStartThread(int whatsup)

  {
  pthread_t *thread = new pthread_t [1];

  m_WhatsupMutex.lock();
  m_Whatsup = whatsup;

  int t = pthread_create(thread, NULL, &_ret_kickoff, (void*) this);

  if (t != 0)

  {
  cout << strerror (errno) << endl;
  if (t == EAGAIN)
  cout << "eagain" << endl;

  return false;
  }

  pthread_detach (*thread);

  return true;
  }*/

void vuThread::stopThread()
{
    pthread_exit(NULL);
}

void vuThread::usleep(unsigned long ms)
{
    ::usleep(ms);
}

vuMutex::vuMutex(bool recursive)
{
    mutex = (void*) new pthread_mutex_t;
    if(recursive) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        // other: PTHREAD_MUTEX_FAST_NP (default), PTHREAD_MUTEX_ERRORCHECK_NP
        pthread_mutex_init((pthread_mutex_t*)mutex, &attr);
        pthread_mutexattr_destroy(&attr);
    } else {
        pthread_mutex_init((pthread_mutex_t*)mutex, NULL);
    }
}

vuMutex::~vuMutex()
{
    pthread_mutex_destroy((pthread_mutex_t*) mutex);
    delete (pthread_mutex_t*)mutex;
}

void vuMutex::lock()
{
    pthread_mutex_lock( (pthread_mutex_t*) mutex );
}

bool vuMutex::trylock()
{
    return pthread_mutex_trylock( (pthread_mutex_t*) mutex ) != EBUSY;
}

void vuMutex::unlock()
{
    pthread_mutex_unlock( (pthread_mutex_t*) mutex );
}

#else // WIN32 defined

#include <windows.h>
#include <process.h>
#include <sys/time.h>

vuMutex::vuMutex(bool recursive)
{
    // one of these should be usable as recursive mutex, but right now,
    // I don't know which... so we just have fast mutexes
    //mutex = (void*)CreateMutex( NULL, FALSE, NULL );

    //mutex = (void*)new CRITICAL_SECTION;
    //InitializeCriticalSection((CRITICAL_SECTION*)mutex);

    //mutex = (void*)CreateSemaphore(NULL, 1, 0x7fffffff, NULL);

    mutex = (void*)CreateEvent(NULL,FALSE,TRUE,NULL);
}

vuMutex::~vuMutex()
{
    if(mutex) {
        //DeleteCriticalSection((CRITICAL_SECTION*)mutex);
        //delete (CRITICAL_SECTION*)mutex;
        CloseHandle((HANDLE)mutex);
        mutex = NULL;
    }
}

void vuMutex::lock()
{
    WaitForSingleObject( (HANDLE)mutex, INFINITE );
    //EnterCriticalSection((CRITICAL_SECTION*)mutex);
}

bool vuMutex::trylock()
{
    return WaitForSingleObject( (HANDLE)mutex, 1) != WAIT_TIMEOUT;
//  return false;
}

void vuMutex::unlock()
{
    //ReleaseSemaphore( (HANDLE)mutex, 1, NULL );
    //LeaveCriticalSection((CRITICAL_SECTION*)mutex);
    SetEvent((HANDLE)mutex);
}

void _kickoff(void* ptr)
{
    vuThread *th = reinterpret_cast<vuThread*>(ptr);
    int   whatsup = th->m_Whatsup;
    void* data    = th->m_AdditionalData;
    th->m_Whatsup = -1;
    th->m_WhatsupMutex.unlock();
    th->run(whatsup, data);
    return;
}

bool vuThread::startThread(int whatsup, void* data)
{
    m_WhatsupMutex.lock();
    m_Whatsup        = whatsup;
    m_AdditionalData = data;

    unsigned long thread = _beginthread(_kickoff, 0, (void*)this);
    //HANDLE thread = CreateThread(NULL, 0, _kickoff, this, 0, NULL);
    while(m_Whatsup == -1);

    m_WhatsupMutex.lock();
    m_WhatsupMutex.unlock();
    if (thread == (unsigned long)(-1))
    {
//      cout << "s2.5" << endl;
        cout << strerror (errno) << endl;
        if (errno == EAGAIN)
            cout << "eagain" << endl;

        return false;
    }

    return true;
}

void vuThread::stopThread()
{
    //_endthread();
}

void vuThread::usleep(unsigned long ms) {
    if (ms > 500)
        Sleep ((ms+500)/1000);
    else if (ms > 0)
        Sleep (1);
    else
        Sleep (0);
}

#endif
