#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>

std::string getTimestamp()
{
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%d%m%Y%H%M%S");
  auto str = oss.str();

  return str;
}

std::string hash(std::string s)
{
    const int p = 31;
    const int m = 1e9 + 9;
    long long hash_value = 0;
    long long p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + (c)) % m;
        p_pow = (p_pow * p) % m;
    }

    if (hash_value < 0){hash_value = -hash_value;}
    
    std::ostringstream ss1;
    ss1 << std::setw(10) << std::setfill('0') << hash_value;
    std::string s1(ss1.str());

    return s1;
}

int getNum(char* p, int n)
{
  int total = 0;
  int mul = 1;
  while (n>0)
  {
    total += mul*(*(p+n) - '0');
    mul *= 10;
    n--;
  }
  return total;
}