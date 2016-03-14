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
#include <sys/time.h>
#include <cstdlib>
#include <cstring>

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

static vector<int> individual_consumptions;
/* 
   Buffer Structure:
   Maintains the buffer, its size
   and the current position.
 */
struct Buffer{
  vector<int> data;
  int size;
  int cur_pos;
} buffer;

int get_rand(int start, int end);
bool is_prime(int n);
int insert(int data);
int remove();
void print_buffer();
void *prod(void *arg);
void *cons(void *arg);
void *fake_prod(void *arg);

/*******************************
     Main Function

 The main function of the program.
 Its algorithm is:
     1) Read in Parameters and check
       for errors.
     2) Initialize Semaphores
     3) Spawn threads
     4) Join threads
     5) Print Results

 Return Value
 ------------
 int       Process return value.

 Parameters
 ----------
 args      The number of arguments
 argv      The parameter arguments

 Local Variables
 ---------------
 int:
  i           Loop counter
  start       Start time return value.
 int[]:
  prod_ids    Producer thread IDs
  fault_ids   Fautly thread IDs
  cons_ids    Consumer thread IDs
 double:
  simulation_time_in_seconds 
              The simulation time
 pthread_t[]:
  producer_threads
              Array of producer threads
  faulty_threads
              Array of faulty threads
  consumer_threads
              Array of consumer threads
 timeval:
  start_time  The start time of the process
  end_time    The end time of the process

********************************/
int main(int args, char *argv[])  
{
  // Parse Args
  int i;
  string s;
  // Implement error catching
  for(i=1; i<args; i++)
    {
      s = string(argv[i]);
      if(i+1 != args || (!s.compare("-d") && (i+1 == args))){
	if(!s.compare("-n")){
	  items = atoi(argv[i+1]);
	  i++;
	} else if(!s.compare("-l")){
	  buf_len = atoi(argv[i+1]);
	  i++;
	} else if(!s.compare("-p")){
	  prod_threads = atoi(argv[i+1]);
	  i++;
	} else if(!s.compare("-f")){
	  fault_threads = atoi(argv[i+1]);
	  i++;
	} else if(!s.compare("-c")){
	  cons_threads = atoi(argv[i+1]);
	  i++;
	} else if(!s.compare("-d")){
	  debug = true;
	}else
	  {
	    cout << "ERROR: Incorrect parameter format.\n";
	    return(-1);
	  }
      }
      else
	{
	  cout << "ERROR: Incorrect parameter format.\n";
	  return(-1);
	}
    }

  // Implement some sort of error solution
  if(items < 0 || buf_len < 0 || prod_threads < 0 || fault_threads < 0
     || cons_threads < 0){
    cout << "ERROR: Missing parameter(s). \n";
    return(-1);
  }

  for (i=0; i < buf_len; i++) {
    buffer.data.push_back(-1);
  }

  buffer.size = buf_len;
  buffer.cur_pos = 0;

  total_to_be_consumed = (prod_threads + fault_threads) * items;

  //Initialize array to keep track of consumption of each consumer
  for (i=0; i< cons_threads; i++) {

    individual_consumptions.push_back(0);

  }

  //Initialize semaphore
  //int sem_init(sem_t* sem, int pshared, unsigned value);
  sem_init(&semaphore, 0, 1);


  //pthread_t *producer_threads = new pthread_t[prod_threads];
  pthread_t producer_threads [prod_threads];

  //pthread_t *faulty_threads = new pthread_t[fault_threads];
  pthread_t faulty_threads [fault_threads];

  //pthread_t *consumer_threads = new pthread_t[cons_threads];
  pthread_t consumer_threads [cons_threads];



  //Start keeping track of time:
  timeval start_time, end_time;
  int start = gettimeofday(&start_time, NULL);

  /* Create independent threads each of which will execute function */

  int prod_ids [prod_threads];
  for(i=0; i<prod_threads; i++)
    {
      prod_ids[i] = i;

      int iret = pthread_create( &(producer_threads[i]), NULL, prod, &(prod_ids[i]));
    }

  int fault_ids [fault_threads];
  for(i=0; i<fault_threads; i++)
    {
      fault_ids[i] = i;

      int iret = pthread_create( &(faulty_threads[i]), NULL, fake_prod, &(fault_ids[i]));
    }

  int cons_ids [cons_threads];
  for(i=0; i<cons_threads; i++)
    {
      cons_ids[i] = i;

      int iret = pthread_create( &(consumer_threads[i]), NULL, cons, &(cons_ids[i]));
    }


  //Join all pthreads created
  for(i=0; i<prod_threads; i++)
    {
      pthread_join( producer_threads[i], NULL);
    }

  for(i=0; i<fault_threads; i++)
    {
      pthread_join( faulty_threads[i], NULL);
    }

  for(i=0; i<cons_threads; i++)
    {
      pthread_join( consumer_threads[i], NULL);
    }

  //double simulation_time_in_seconds = difftime(time(NULL), start);
  int end = gettimeofday(&end_time, NULL);
  double simulation_time_in_seconds = (long long)(end_time.tv_usec - start_time.tv_usec) / 1000000.0;
  simulation_time_in_seconds += (long long)(end_time.tv_sec - start_time.tv_sec) / 1.0;
  //Print statistics
  cout << "\nPRODUCER / CONSUMER SIMULATION COMPLETE";

  cout << "\n";

  cout << "=======================================";

  cout << "\n";

  cout << "Number of Items Per Producer Thread: " << items ;

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

    cout << "  Thread " << i+1 << ": " << individual_consumptions[i] << "\n";

  }

  cout << "\n";
  
  cout << "Total Simulation Time: " << to_string(simulation_time_in_seconds) << " seconds\n";

  return 0;
}

/*******************************
     Producer Function
********************************/
void *prod(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int* id_container = (int*) arg;
  int id = id_container[0];
  int prime = 0;
  int number_of_items_inserted = 0;

  int insert_status = -2;

  while(number_of_items_inserted < items) {

    if (insert_status != -1) {
      prime = get_rand(2, 999999);

      while (!is_prime(prime)) {
        prime = get_rand(2, 999999);
      }
    }

    sem_wait(&semaphore);

    insert_status = insert(prime);

    if (insert_status != -1) {

      number_of_items_inserted++;

      if(debug)
	{
	  cout << "(PRODUCER  " << id+1 << " writes   " << number_of_items_inserted << "/" << items << "   " << prime << "):";
	  print_buffer();
	}
      if(buffer.size == buffer.cur_pos) {
	if(debug)
	  cout << " *BUFFER NOW FULL*";

        time_full++;

      }

      if(debug)
	cout << "\n";

    }

    sem_post(&semaphore);

  }
  

}

/*******************************
     Fake Producer Function
********************************/
void *fake_prod(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int* id_container = (int*) arg;
  int id = id_container[0];

  int number = 0;

  int number_of_items_inserted = 0;

  int insert_status = -2;

  while (number_of_items_inserted < items) {

    if (insert_status != -1) {
      number = get_rand(2, 999999);
    }

    sem_wait(&semaphore);

    insert_status = insert(number);

    if (insert_status != -1) {

      number_of_items_inserted++;
      if(debug)
	{
	  cout << "(PR*D*C*R  " << id+1 << " writes   " << number_of_items_inserted << "/" << items << "   " << number << "):";
	  print_buffer();
	}
      
      if(buffer.size == buffer.cur_pos) {
	if(debug)
	  cout << " *BUFFER NOW FULL*";

        time_full++;

      }
      if(debug)
	cout << "\n";

    }

    sem_post(&semaphore);

  }

}

/*******************************
     Consumer Function
********************************/
void *cons(void *arg)
{
  //Buffer *buf = (struct Buffer*)arg;
  int* id_container = (int*) arg;
  int id = id_container[0];

  int remove_status;
  int number_of_items_removed = 0;

  while (total_consumed != total_to_be_consumed) {

    sem_wait(&semaphore);

    remove_status = remove();

    if (remove_status != -1) {

      total_consumed++;

      individual_consumptions[id]++;

      number_of_items_removed++;
      if(debug)
	{
	  cout << "(CONSUMER  " << id+1 << " reads   " << number_of_items_removed << "       " << remove_status << "):";
	  print_buffer();
	}
      
      if (!is_prime(remove_status)) {
	if(debug)
	  cout << " *NOT PRIME*";

        time_non_prime++;
      }

      if(buffer.cur_pos == 0){
        if(debug)
	  cout << " *BUFFER NOW EMPTY*";

        time_empty++;

      }
      if(debug)
	cout << "\n";

    }

    sem_post(&semaphore);

  }
  
}

/*******************************
     Insert Function
********************************/
int insert(int input)
{
  if(buffer.size == buffer.cur_pos)
    {
      return -1;
    }

  buffer.data[buffer.cur_pos] = input;
  buffer.cur_pos++;
  return buffer.cur_pos;
}

/*******************************
     Is Prime Function
********************************/
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


/*******************************
     Get Random Function
********************************/
int get_rand(int start, int end)
{
  int n = rand();
  return (n % (end - start)) + start;
  
}

/*******************************
     Remove Function
********************************/
int remove()
{
  if(buffer.cur_pos == 0){
    return -1;
    // Throw error instead;
  }else{
    buffer.cur_pos--;
    return buffer.data[buffer.cur_pos];
  }
}

/*******************************
     Print Buffer Function
********************************/
void print_buffer()
{

  cout << "  (" << buffer.cur_pos << "): [  ";

  for(int i = buffer.cur_pos-1; i > -1; i--)
  {
    cout << buffer.data[i] << "  ";
  }

  cout << "]";
}
