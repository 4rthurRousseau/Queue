#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <cassert>
#include <chrono>

using namespace std;
struct Queue {
  mutex mt;
  condition_variable_any producer_signal, consumer_signal;
  int begin, end;
  int array[100];
};

void push(Queue & q, int val) {
  q.mt.lock();
  while(q.end - q.begin == 99) {
    q.producer_signal.wait(q.mt);
  }
  int index = q.end % 100;
  q.array[index] = val;
  q.end = q.end + 1;
  assert(q.begin <= q.end);
  assert(q.end - q.begin <= 99);
  q.consumer_signal.notify_all();
  q.mt.unlock();
}
int pull(Queue & q) {
  q.mt.lock();
  
  while(q.begin == q.end) {
    q.consumer_signal.wait(q.mt);
  }
  int index = q.begin % 100;
  int res = q.array[index];
  q.begin = q.begin + 1;
  assert(q.begin <= q.end);
  assert(q.end - q.begin <= 99);
  if(q.end - q.begin <= 50) {
    q.producer_signal.notify_all();
  }
  q.mt.unlock();
return res;
}

int  size(Queue & q) {
  q.mt.lock();
  int res = q.end - q.begin;
  assert(q.begin <= q.end);
  assert(q.end - q.begin <= 99);
  q.mt.unlock();
  return res;
}

bool is_empty(Queue & q) {
  q.mt.lock();
  bool res = q.begin == q.end;
  assert(q.begin <= q.end);
  assert(q.end - q.begin <= 99);
  q.mt.unlock();
  return res;
}

void producer (Queue & q, int id) {
  this_thread::sleep_for(chrono::nanoseconds(100));
  for(int i = 1; i < 1001; i++) {
    this_thread::sleep_for(chrono::nanoseconds(10));
    push(q, i); 
  }
}

void calculer(int & n){
  if (n % 2 == 0){
    n = n / 2;
  } else {
    n = n * 3 + 1;
  }
}

void consumerQ1 (Queue & q, Queue & q2) {
  while(true){
    this_thread::sleep_for(chrono::nanoseconds(5));
    int res = pull(q);
    int index = res;
    int i = 0;
    while(res != 1){
      //cout << res << endl;
      calculer(res);
      i++;
    }
    //cout << "Number : " << index << " Itérations : " << i << " F : " << res << endl;
    push(q2, i);
  }
}
void consumerQ2 (Queue & q) {
  while(true) {
    this_thread::sleep_for(chrono::nanoseconds(5));
    int res = pull(q);
    cout << "Nombre d'itérations : " << res <<  endl;
  }
}

int main () {
  Queue q;
  q.begin = 0;
  q.end = 0;

  Queue q2;
  q2.begin = 0;
  q2.end = 0;

  thread p = thread(producer, ref(q), 0);

  thread a = thread(consumerQ1, ref(q), ref(q2));
  thread b = thread(consumerQ1, ref(q), ref(q2));
  thread c = thread(consumerQ1, ref(q), ref(q2));
  thread d = thread(consumerQ1, ref(q), ref(q2));

  thread m = thread(consumerQ2, ref(q2));

  p.join();
  d.join();
  m.join();
  return 0;
}
