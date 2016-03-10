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
#include <time.h>

using namespace std;

sem_t semaphore;

int total_consumed = 0; //only to be accessed between sem_wait() and sem_post()
int total_to_be_consumed;

int time_full = 0;
int time_empty = 0;
int time_non_prime = 0;

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

  //Declare and initialize array to keep track of consumption of each consumer
  int *individual_consumptions = new int[cons_threads];

  for (i=0; i< cons_threads; i++) {

    &individual_consumptions[i] = 0;

  }

  //Initialize semaphore
  //int sem_init(sem_t* sem, int pshared, unsigned value);
  sem_init(&semaphore, 0, 1);


  pthread_t *producer_threads = new pthread_t[prod_threads];
  //pthread_t** producer_thread_array = &producer_threads;

  pthread_t *faulty_threads = new pthread_t[fault_threads];
  //pthread_t** faulty_thread_array = &faulty_threads;

  pthread_t *consumer_threads = new pthread_t[cons_threads];
  //pthread_t** consumer_thread_array = &consumer_threads;




  //Start keeping track of time:
  time_t start = time(0);

  /* Create independent threads each of which will execute function */

  for(i=0; i<prod_threads; i++)
    {
      int *id = new int[1];
      &id[0] = i;

      int iret = pthread_create( &producer_threads[i], NULL, prod, id);
    }

  for(i=0; i<fault_threads; i++)
    {
      int *id = new int[1];
      *id = i;

      int iret = pthread_create( &faulty_threads[i], NULL, fake_prod, id);
    }

  for(i=0; i<cons_threads; i++)
    {
      int *id = new int[1];
      *id = i;

      int iret = pthread_create( &consumer_threads[i], NULL, cons, id);
    }


  //Join all pthreads created
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

  double simulation_time_in_seconds = difftime(time(0), start);

  //Print statistics
  cout << "PRODUCER / CONSUMER SIMULATION COMPLETE";

  cout << "\n";

  cout << "=======================================";

  cout << "\n";

  cout << "Number of Items Per Producer Thread:" << items ;

  cout << "\n";

  cout << "Size of Buffer: " << buf_len;

  cout << "\n";

  cout << "Number of Producer Threads: " << prod_threads;

  cout << "\n";

  cout << "Number of Faulty Producer Threads: " << fault_threads;

  cout << "\n";

  cout << "Number of Consumer Threads: " << cons_threads;

  cout << "\n\n";

  cout << "Number of Times Buffer Became Full: " << time_full;

  cout << "\n";

  cout << "Number of Times Buffer Became Empty: " << time_empty;

  cout << "\n\n";

  cout << "Number of Non-primes Detected: " << time_non_prime;

  cout << "\n";

  cout << "Total Number of Items Consumed: " << total_consumed;

  cout << "\n";

  for (int i=0; i < cons_threads; i++) {

    cout << "  Thread " << i+1 << ": " << &individual_consumptions[i] << "\n";

  }

  cout << "\n";
  
  cout << "Total Simulation Time: " << simulation_time_in_seconds << " seconds\n";

  return 0;
}

void *prod(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int id = (int) *arg;
  int prime = 0;
  int number_of_items_inserted = 0;

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

    if (insert_status != -1) {

      number_of_items_inserted++;

      cout << "(PRODUCER  " << id << " writes   " << number_of_items_inserted << "/" << items << "   " << prime << "):";

      print_buffer(buffer);

    }

    if(buffer.size == buffer.cur_pos) {
        
        cout << " *BUFFER NOW FULL*";

        time_full++;

    }

    cout << "\n";

    sem_post(&semaphore);

  }
  

}

void *fake_prod(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int id = (int) *arg;

  int number = 0;

  int number_of_items_inserted = 0;

  int insert_status = -2;

  for (int i=0; i < items; i++) {

    if (insert_status != -1) {
      number = get_rand(2, 999999);
    }

    sem_wait(&semaphore);

    insert_status = insert(number, buffer);

    if (insert_status != -1) {

      number_of_items_inserted++;

      cout << "(PR*D*C*R  " << id << " writes   " << number_of_items_inserted << "/" << items << "   " << prime << "):";

      print_buffer(buffer);

    }

    if(buffer.size == buffer.cur_pos) {
        
        cout << " *BUFFER NOW FULL*";

        time_full++;

    }

    cout << "\n";

    sem_post(&semaphore);

  }

}

void *cons(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int id = (int) *arg;
  int remove_status;
  int number_of_items_removed = 0;

  while (total_consumed != total_to_be_consumed) {

    sem_wait(&semaphore);

    remove_status = remove(buffer);

    if (remove_status != -1) {

      total_consumed++;

      &individual_consumptions[id]++;

      number_of_items_removed++;

      cout << "(CONSUMER  " << id << " reads   " << number_of_items_removed << "       " << remove_status << "):";

      print_buffer(buffer);

      if (!is_prime(remove_status)) {
        cout << " *NOT PRIME*";

        time_non_prime++;
      }

      if(buffer.cur_pos == 0){
        
        cout << " *BUFFER NOW EMPTY*";

        time_empty++;

      }

    }

    cout << "\n";

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
    return *buf.data[0];
  }
}

void print_buffer(Buffer buf)
{

  cout << "  (" << buf.cur_pos << "): [  ";

  for(int i = 0; i < buf.cur_pos; i++)
  {
    cout << *buf.data[i] << "  ";
  }

  cout << "]";
}