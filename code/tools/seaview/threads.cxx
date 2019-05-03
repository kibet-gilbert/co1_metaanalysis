/* default thread stack sizes
 Mac OS X  512 KB (8 MB for main thread)
 Linux       2 MB
 WIN32       1 MB
 */

// included function
int fl_create_thread(void (*f) (void *), void* p, unsigned stack_size_KB);
// if stack_size_KB == 0, the thread is created with its default size
// returns 0 iff OK

#ifdef NO_THREADS

int fl_create_thread(void (*f) (void *), void* p, unsigned stack_size_KB)
{
  f(p);
  return 0;
}

#else

#ifndef HAVE_PTHREAD
#ifdef WIN32
#define HAVE_PTHREAD 0
#else
#define HAVE_PTHREAD 1
#endif
#endif

#if HAVE_PTHREAD
// Use POSIX threading...

#include <pthread.h>

int fl_create_thread(void (*f) (void *), void* p, unsigned stack_size_KB)
{
  pthread_t t;
  int 			err = 0;
  pthread_attr_t 	threadAttribute;
  size_t		stackSize = 0;
  int			retval;
  
  err = pthread_attr_init(&threadAttribute);
  if (err) return err;
  pthread_attr_setdetachstate(&threadAttribute, PTHREAD_CREATE_DETACHED);

  if (stack_size_KB > 0) {
    err = pthread_attr_getstacksize(&threadAttribute, &stackSize); 
    if (err) return err;
    if (stackSize < stack_size_KB * 1024)
    {
      err = pthread_attr_setstacksize(&threadAttribute, stack_size_KB * 1024);
      if (err) return err;
    }
  }
  retval = pthread_create(&t, &threadAttribute, (void* (*) (void *))f, p);
  pthread_attr_destroy(&threadAttribute);
  return retval;
}

#else // Use Windows threading...

#include <process.h>

int fl_create_thread(void (*f) (void *), void* p, unsigned stack_size_KB) 
// returns 0 iff OK
{
  unsigned long t;
  t = _beginthread(f, stack_size_KB * 1024, p);
  return t == -1L;
}
#endif // HAVE_PTHREAD
#endif // NO_THREADS
