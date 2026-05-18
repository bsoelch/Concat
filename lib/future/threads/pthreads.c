#include <inttypes.h>
#include <pthread.h>
typedef void* Thread_fun(void*);
typedef uint64_t Thread_id;
int cct_helper_createThread(Thread_id* thread_id,Thread_fun f,void* arg) {
  pthread_t pthread_id;
  int res = pthread_create(&pthread_id,NULL,f,arg);
  *thread_id = pthread_id;
  return res;
}
int cct_helper_joinThread(Thread_id thread_id) {
  return pthread_join(thread_id,NULL);
}
void cct_helper_exitThread() {
  pthread_exit(NULL);
}
