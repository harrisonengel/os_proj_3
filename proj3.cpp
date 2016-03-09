//*********************************************************
//
// Harrison Engel and Duy Dang
// Operating Systems
// Project #3: Producer/Consumer Problem with Faulty Threads
// Program 3: osp3.cpp
// 03/14/2016
// Dr. Michael C. Scherger
//
//*********************************************************

#include <iostream>
#include <cmath>
#include <vector>
#include <semaphore.h>
#include <pthread.h>

using namespace std;

sem_t semaphore;

//int total_consumed = 0; //only to be accessed between sem_wait() and sem_post()
//int total_to_be_consumed;

int items = -1;
int buf_len = -1 ;
int prod_threads = -1;
int fault_threads = -1;
int cons_threads = -1;
bool debug = false;

struct Buffer{
  int **data;
  int size;
  int cur_pos;
} buffer;

int get_rand(int start, int end);
bool is_prime(int n);
int insert(int data, Buffer buf);
void *prod(void *arg);
void *cons(void *arg);
void *fake_prod(void *arg);

int main(int args, char *argv[])  
{
  // Parse Args
  int i;
  string s;
  // Implement error catching
  for(i=1; i<args; i++)
    {
      s = string(argv[i]);
      if(!s.compare("-n")){
	items = atoi(argv[i+1]);
      }
      
      if(!s.compare("-l")){
	buf_len = atoi(argv[i+1]);
      }

      if(!s.compare("-p")){
	prod_threads = atoi(argv[i+1]);
      }

      if(!s.compare("-f")){
	fault_threads = atoi(argv[i+1]);
      }

      if(!s.compare("-c")){
	cons_threads = atoi(argv[i+1]);
      }

      if(!s.compare("-d")){
	debug = true;
      }
   }

  // Implement some sort of error solution
  if(items < 0 || buf_len < 0 || prod_threads < 0 || fault_threads < 0
     || cons_threads < 0){
    cout << "ERROR\n";
  }

  int *buf = new int[buf_len];
  buffer.data = &buf;
  buffer.size = buf_len;
  buffer.cur_pos = 0;

  total_to_be_consumed = (prod_threads + fault_threads) * items;

  //Initialize semaphore
  //int sem_init(sem_t* sem, int pshared, unsigned value);
  sem_init(&semaphore, 0, 1);


  pthread_t *producer_threads = new pthread_t[prod_threads];
  //pthread_t** producer_thread_array = &producer_threads;

  pthread_t *faulty_threads = new pthread_t[fault_threads];
  //pthread_t** faulty_thread_array = &faulty_threads;

  pthread_t *consumer_threads = new pthread_t[cons_threads];
  //pthread_t** consumer_thread_array = &consumer_threads;

  /* Create independent threads each of which will execute function */

  for(i=0; i<prod_threads; i++)
    {
      int *index = new int[1];
      int iret = pthread_create( &producer_threads[i], NULL, prod, index);
    }

  for(i=0; i<fault_threads; i++)
    {
      int *index = new int[1];
      int iret = pthread_create( &faulty_threads[i], NULL, fake_prod, index);
    }

  for(i=0; i<cons_threads; i++)
    {
      int *index = new int[1];
      int iret = pthread_create( &consumer_threads[i], NULL, cons, index);
    }


  for(i=0; i<prod_threads; i++)
    {
      pthread_t** temp = &producer_threads[i];
      pthread_join( **temp, NULL);
    }

  for(i=0; i<fault_threads; i++)
    {
      pthread_t** temp = &faulty_threads[i];
      pthread_join( **temp, NULL);
    }

  for(i=0; i<cons_threads; i++)
    {
      pthread_t** temp = &consumer_threads[i];
      pthread_join( **temp, NULL);
    }


  return 0;
}

void *prod(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int index = (int) *arg;
  int prime = 0;

  int insert_status = -2;

  for (int i=0; i < items; i++) {

    if (insert_status != -1) {
      prime = get_rand(2, 999999);

      while (!is_prime(prime)) {
        prime = get_rand(2, 999999);
      }
    }

    sem_wait(&semaphore);

    insert_status = insert(prime, buffer);

    sem_post(&semaphore);

  }
  


}

void *cons(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int index = (int) *arg;
  


}

void *fake_prod(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int index = (int) *arg;

  int even = 0;

  int insert_status = -2;

  for (int i=0; i < items; i++) {

    if (insert_status != -1) {
      even = get_rand(2, 999999);

      while (even % 2 != 0) {
        even = get_rand(2, 999999);
      }
    }

    sem_wait(&semaphore);

    insert_status = insert(even, buffer);

    sem_post(&semaphore);

  }

}

int insert(int data, Buffer buf)
{
  if(buf.size == buf.cur_pos)
    {
      return -1;
    }

  *buf.data[buf.cur_pos] = data;
  buf.cur_pos++;
  return buf.cur_pos;
}

bool is_prime(int n)
{
  int sqr = (int)sqrt(n);

  for(int i = 2; i < sqr; i++)
    {
      if(n % i == 0)
	return false;
    }

  return true;
}


int get_rand(int start, int end)
{
  int n = rand();
  return (n % (end - start)) + start;
  
}

int remove(Buffer buf)
{
  if(buf.cur_pos == 0){
    return -1;
    // Throw error instead;
  }else{
    buf.cur_pos--;
    return *buf.data[buf.cur_pos];
  }
}