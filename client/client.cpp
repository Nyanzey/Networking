#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <thread>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>

#include "client_libs/utils.h"
#include "client_libs/pack_cli.h"
#include "client_libs/unpack_cli.h"

//Function to read messages sent to client
void read_t(int socket);

void procedureF(char* data, int socket);

void procedureR(char* data, int socket);

std::string current_hash;std::string getTimestamp();

std::string hash(std::string str);

int getNum(char* p, int n);

char tictacP = '-';

int main(void)
{
  struct sockaddr_in stSockAddr;
  int Res;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n;
 
  if (-1 == SocketFD)
  {
    perror("cannot create socket");
    exit(EXIT_FAILURE);
  }
 
  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(1100);
  Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
  if (0 > Res)
  {
    perror("error: first parameter is not a valid address family");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  else if (0 == Res)
  {
    perror("char string (second parameter does not contain valid ipaddress");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
 
  if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("connect failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  std::string str;
  std::string message;
  int size;
  std::string nickname;

  //Getting nickname for current client
  std::cout << "Nickname: ";
  getline(std::cin, str);
  size = str.length();

  if (size < 10)
  {
    message = "N0" + std::to_string(size) + str;
  }
  else
  {
    message = "N" + std::to_string(size) + str;
  }
  
  //Sending nickname for current client to the server
  n = send(SocketFD, message.c_str(), message.length(), 0);

  //Thread for reading messages sent to client
  std::thread (read_t, SocketFD).detach();

  bool finish = false;
  while (1)
  {
    //Reading messages
    getline(std::cin, str);
    
    switch (str[0])
    {
    case 'M':
      {
        std::string message = packM(str.c_str());
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
      }
      break;
    case 'W':
      {
        std::string message = packW(str.c_str());
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
      }
      break;

    case 'L':
      {
        std::string message = "L00";
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
      }
      break;

    case 'F':
      {
        std::string message = packF(str.c_str(), current_hash);
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
      }
      break;

    case 'B':
      {
        std::string message = "BTTT";
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
      }
      break;

    case 'p':
      {
        std::string message = "p";
        message += str[2];
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
      }
      break;

    case 'v':
      {
        std::string message = "v";
        message += str[2] + str[3] + str[4];
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
      }
      break;

    case 'Q':
      {
        std::string message = "Q00";
        //std::cout << message << std::endl;
        send(SocketFD, message.c_str(), message.length(), 0);
        finish = true;
      }
      break;
    
    default:
      break;
    }
    
    if (finish){break;}
  }
 
  shutdown(SocketFD, SHUT_RDWR);
 
  close(SocketFD);
  return 0;
}

void read_t(int socket)
{
  //Bytes read
  int n;

  //Buffers to unpack the message received
  char data[5];

  while(1)
  {
    n = recv(socket, data, 1, 0);

    if (n>0)
    {
      if (*data == 'M')
      {
        //Unpacking message to get information
        unpackM(socket);
      }

      if (*data == 'L')
      {
        unpackL(socket);
      }

      if (*data == 'F')
      {
        procedureF(data, socket);
      }

      if (*data == 'R')
      {
        procedureR(data, socket);
      }

      if (*data == 'S')
      {
        char buff[10] = "OK";
        tictacP = data[1];
        send(socket, buff, 10, 0);
      }

      if (*data == 'G')
      {
        unpackG(socket);
      }

      if (*data == 'P')
      {
        unpackP(socket);
      }

      if (*data == 'E')
      {
        unpackE(socket);
      }

      if (*data == 'w')
      {
        unpackw(data);
      }

      if (*data == 'l')
      {
        unpackl(data);
      }

      if (*data == 'z')
      {
        unpackz(socket);
      }
    }
  }
}

void procedureF(char* data, int socket)
{
  std::string origin, hash;
  unpackF(socket, origin, hash);

  int origin_size = origin.length();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << origin_size;
  std::string s1(ss1.str());

  std::string message = "R" + s1 + origin + hash;
  send(socket, message.c_str(), message.length(), 0);
}

void procedureR(char* data, int socket)
{
  std::string origin, recv_hash;
  unpackR(socket, origin, recv_hash);

  if (recv_hash == current_hash)
  {
    std::cout << "Hash received from [" << origin << "] (" << recv_hash << ") is CORRECT." << std::endl; 
  }
  else
  {
    std::cout << "Hash received from [" << origin << "] (" << recv_hash << ") is INCORRECT." << std::endl; 
  }
}
