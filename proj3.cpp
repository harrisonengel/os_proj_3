#include <iostream>
#include <cmath>

using namespace std;

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
}

void *prod(void *arg)
{
  Buffer *buf = (struct Buffer*)arg;
  

}

void *cons(void *arg)
{
  Buffer *buf = (struct Buffer*)arg;
  
}

void *fake_prod(void *arg)
{
  Buffer *buf = (struct Buffer*)arg;


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
		       


