#include <ctime>
#include <iostream>
#include <thread>
#include <vector>

long seqSumArr(std::vector<int> p) {
  long sum = 0;
  for (int i = 0; i < p.size(); i++) {
    sum += p[i];
  }
  return sum;
}

long parSumArr(std::vector<int> p) {
  long sum = 0;
  for (int i = 0; i < p.size(); i++) {
    sum += p[i];
  }
  return sum;
}

int main(int argc, char **argv) {
  std::vector<int> obj{1, 2, 3, 4, 5, 6};
  std::cout << seqSumArr(obj);
  std::thread t(parSumArr, obj);
  t.join();
  return 0;
}
