#include <iostream>
int main() {
  int n;
  int k;
  std::cin >> n;
  int mas[n];
  for (int i = 0; i < n; ++i)
    std::cin >> mas[i];
  std::cin >> k;
  int sum =  0;
  for (int i = 0; i < k; ++i) {
    for (int j = i+1;j<k;++j){
      int sum = mas[i];
      while (sum){
        int l = i;
        int r = j;
      }
    }
  }
  return 0;
}

