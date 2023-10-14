#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <fcntl.h>

std::string packM(const char* data)
{
  //Format: M(1B)DestinySize(2B)Destiny(DestinySizeB)MsgSize(3B)Msg(MsgSizeB)
  char destiny[100];
  char msg[1000];

  const char* p = data+2;
  int i = 0;
  while (*p != ',')
  {
    destiny[i] = *p;
    p++;
    i++;
  }
  p++;
  destiny[i] = '\0';

  i = 0;
  while (*p != '\0')
  {
    msg[i] = *p;
    p++;
    i++;
  }
  msg[i] = 0;

  std::string destiny_str = destiny;
  std::string msg_str = msg;

  int dest_size = destiny_str.length();
  int msg_size = msg_str.length();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << dest_size;
  std::string s1(ss1.str());

  std::ostringstream ss2;
  ss2 << std::setw(3) << std::setfill('0') << msg_size;
  std::string s2(ss2.str());

  std::string message = "M" + s1 + destiny_str + s2 + msg_str;
  return message;
}

std::string packW(const char* data)
{
  //Format: W(1B)Size(2B)Msg(SizeB)
  char msg[1000];

  const char* p = data+2;
  int i = 0;
  while (*p != '\0')
  {
    msg[i] = *p;
    p++;
    i++;
  }
  msg[i] = '\0';

  std::string msg_str = msg;
  int msgSize = msg_str.length();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << msgSize;
  std::string s1(ss1.str());

  std::string message = "W" + s1 + msg_str;
  return message;
}

//Change format to send file
std::string packF(const char* data, std::string &current_hash)
{
  //Format: F(1B)destinySize(2B)destiny(destinySize)nameSize(5B)
  //filename(nameSizeB)fileSize(10B)file(fileSize)hash(10B)timestamp(14B?)

  std::string destiny = "";
  std::string filename = "";
  std::string timestamp = getTimestamp();

  unsigned char file_buff[200000];

  const char* p = data+2;
  while (*p != ',')
  {
    destiny += *p;
    p++;
  }
  p++;
  
  while (*p != '\0')
  {
    filename += *p;
    p++;
  }

  
  FILE * fd = fopen(filename.c_str(), "rb");
  int n = fread(file_buff, 1, sizeof(file_buff), fd);
  std::string file_str(file_buff, file_buff + n);

  std::string hsh = hash(file_str+timestamp);
  
  int destiny_size = destiny.size();
  int filename_size = filename.size();
  int file_size = file_str.length();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << destiny_size;
  std::string s1(ss1.str());

  std::ostringstream ss2;
  ss2 << std::setw(5) << std::setfill('0') << filename_size;
  std::string s2(ss2.str());

  std::ostringstream ss3;
  ss3 << std::setw(10) << std::setfill('0') << file_size;
  std::string s3(ss3.str());

  current_hash = hsh;
  std::string message = "F" + s1 + destiny + s2 + filename + s3 + file_str + hsh + timestamp;
  return message;
}
