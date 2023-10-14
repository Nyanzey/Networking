/* Server code in C */
#include <bits/stdc++.h>

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
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "server_libs/utils.h"
#include "server_libs/pack_ser.h"
#include "server_libs/unpack_ser.h"
#include "server_libs/tictactoe.h"

//Map for file descriptors
std::map<std::string, int> nicks;

//Map for nicknames
std::map<int, std::string> nicks_inv;

//List of clients connected
std::vector<std::string> nick_list;

//Waiting list
std::vector<std::string> waiting_TTT;

// M protocol
void procedureM(int socket, int n);

// W protocol
void procedureW(int socket, int n);

// L protocol
void procedureL(int socket, int n);

// F protocol
void procedureF(int socket, int n);

// R protocol
void procedureR(int socket, int n);

// Q protocol
void procedureQ(int socket, int n);

// B protocol
void procedureB(int socket, int n);

void procedurep(int socket, int n);

void procedurev(int socket, int n);

void read_t(int socket);

ActualTTTGame game;

int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  int n;
  srand(time(0));

  if (-1 == SocketFD)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(1100);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if (-1 == bind(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if (-1 == listen(SocketFD, 10))
  {
    perror("error listen failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  for (;;)
  {
    // Waiting for client connection
    std::cout << "accepting..." << std::endl;
    int ConnectFD = accept(SocketFD, NULL, NULL);
    if (0 > ConnectFD)
    {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }

    std::string nick_str;
    unpackN(ConnectFD, nick_str);

    // Addig new client connec  tion to maps
    
    nicks[nick_str] = ConnectFD;
    nicks_inv[ConnectFD] = nick_str;
    nick_list.push_back(nick_str);

    std::cout << "Accepted " << nick_str << " [" << nicks[nick_str] << "]" << std::endl;

    // Thread for forwarding messages
    std::thread(read_t, ConnectFD).detach();
  }
  close(SocketFD);
  return 0;
}

void read_t(int socket)
{
  // Buffers to unpack the message
  char *data;

  // Bytes read
  int n;

  for (;;)
  {
    // Waiting for message to arrive
    n = recv(socket, data, 1, 0);

    // Message read
    if (n > 0)
    {
      switch (*data)
      {
      case 'M':
        procedureM(socket, n);
        break;

      case 'W':
        procedureW(socket, n);
        break;

      case 'L':
        procedureL(socket, n);
        break;
      case 'F':
        procedureF(socket, n);
        break;
      case 'R':
        procedureR(socket, n);
        break;
      case 'B':
        procedureB(socket, n);
        break;
      case 'p':
        procedurep(socket, n);
        break;
      case 'v':
        procedurev(socket, n);
        break;
      //DO NOT copy this one for new protocol
      case 'Q':
        procedureQ(socket, n);
        //LOOK AT THIS RETURN MAN
        return;
      default:
        break;
      }
    }
  }
}

void procedurep(int socket, int n)
{
  std::string cur_player = nicks_inv[socket];
  char data[1];
  n = recv(socket, data, 1, 0);
  int move = *data - '0';
  int res;

  if (game.playername1 == cur_player)
  {
    res = game.move(move, game.p1);
  }
  else if (game.playername2 == cur_player)
  {
    res = game.move(move, game.p2);
  }
  else
  {
    res = -1;
  }

  if (res != 3)
  {
    std::string message = packE(res);
    send(socket, message.c_str(), message.length(), 0);
  }
  else
  {
    std::string message = packG(game.board.pack_board());

    send(nicks[game.playername1], message.c_str(), message.length(), 0);
    send(nicks[game.playername2], message.c_str(), message.length(), 0);

    for (int i = 0; i < game.viewers.size(); i++)
    {
      send(nicks[game.viewers[i]], message.c_str(), message.length(), 0);
    }

    if (game.board.finished())
    {
      std::string message;
      if(game.board.winner == game.p1)
      {
        message = "wTTT";
        send(nicks[game.playername1], message.c_str(), message.length(), 0);

        message = "lTTT";
        send(nicks[game.playername2], message.c_str(), message.length(), 0);

        std::ostringstream ss1;
        ss1 << std::setw(2) << std::setfill('0') << game.playername1.length();
        std::string s1(ss1.str());

        message = "z" + s1 + game.playername1;
        for (int i = 0; i < game.viewers.size(); i++)
        {
          send(nicks[game.viewers[i]], message.c_str(), message.length(), 0);
        }
      }
      else
      {
        message = "wTTT";
        send(nicks[game.playername2], message.c_str(), message.length(), 0);

        message = "lTTT";
        send(nicks[game.playername1], message.c_str(), message.length(), 0);

        std::ostringstream ss1;
        ss1 << std::setw(2) << std::setfill('0') << game.playername2.length();
        std::string s1(ss1.str());

        message = "z" + s1 + game.playername2;
        for (int i = 0; i < game.viewers.size(); i++)
        {
          send(nicks[game.viewers[i]], message.c_str(), message.length(), 0);
        }
      }
    }
  }
}

void procedurev(int socket, int n)
{
  game.viewers.push_back(nicks_inv[socket]);

  std::string message = packM("Registered as viewer.", "server");
  send(socket, message.c_str(), message.length(), 0);
}

void procedureB(int socket, int n)
{
  char supbuff[10];

  std::string game_name(3, '0');
  recv(socket, &game_name[0], 3, 0);

  if (game_name == "TTT")
  {
    if (!waiting_TTT.size())
    {
      std::string message = packM("Waiting for more players...", "server");
      waiting_TTT.push_back(nicks_inv[socket]);
      send(socket, message.c_str(), message.length(), 0);
    }
    else
    {
      char p1, p2;
      double randy = ((double)rand()) / INT_MAX;
      if (randy < 0.5)
      {
        p1 = 'X';
        p2 = 'O';
      }
      else
      {
        p1 = 'O';
        p2 = 'X';
      }

      std::string message1 = "S";
      message1 += p1;

      std::string message2 = "S";
      message2 += p2;

      //Player pieces
      game.set_params(p1, p2, waiting_TTT[0], nicks_inv[socket]);

      send(socket, message1.c_str(), message1.length(), 0);
      recv(socket, supbuff, 10, 0);

      send(nicks[waiting_TTT[0]], message2.c_str(), message2.length(), 0);
      recv(nicks[waiting_TTT[0]], supbuff, 10, 0);

      //Game board
      message1 = packG(game.board.pack_board());

      send(socket, message1.c_str(), message1.length(), 0);
      recv(socket, supbuff, 10, 0);

      send(nicks[waiting_TTT[0]], message1.c_str(), message1.length(), 0);
      recv(nicks[waiting_TTT[0]], supbuff, 10, 0);

      //Player list
      std::vector<std::string> players;
      players.push_back(game.playername1);
      players.push_back(game.playername2);

      message1 = packP(players);

      send(socket, message1.c_str(), message1.length(), 0);
      recv(socket, supbuff, 10, 0);

      send(nicks[waiting_TTT[0]], message1.c_str(), message1.length(), 0);
      recv(nicks[waiting_TTT[0]], supbuff, 10, 0);

      waiting_TTT.clear();
    }
  }
}

void procedureR(int socket, int n)
{
  std::string origin, hash;
  unpackR(socket, origin, hash);

  std::string destiny = nicks_inv[socket];
  int destiny_size = destiny.length();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << destiny_size;
  std::string s1(ss1.str());

  std::string message = "R" + s1 + destiny + hash;

  send(nicks[origin], message.c_str(), message.length(), 0);
}

void procedureM(int socket, int n)
{
  // Getting info from the message received
  std::string msg_str;
  std::string receiver_str;

  unpackM(socket, receiver_str, msg_str);
  
  // Sending message to corresponding client
  std::string message = packM(msg_str, nicks_inv[socket]);
  send(nicks[receiver_str], message.c_str(), message.length(), 0);
}

void procedureW(int socket, int n)
{
  // Getting info from the message received
  std::string msg_str;
  unpackW(socket, msg_str);

  // Sending message to corresponding client
  std::string message = packM(msg_str, nicks_inv[socket]);
  
  for (int i = 0; i < nick_list.size(); i++)
  {
    if (nick_list[i] != nicks_inv[socket])
    {
      send(nicks[nick_list[i]], message.c_str(), message.length(), 0);
    }
  }
}

void procedureL(int socket, int n)
{
  std::string message = packL(nick_list);

  send(socket, message.c_str(), message.length(), 0);
}

void procedureQ(int socket, int n)
{
  nick_list.erase(std::find(nick_list.begin(), nick_list.end(), nicks_inv[socket]));

  std::string tmp = nicks_inv[socket];
  nicks_inv.erase(socket);
  nicks.erase(tmp);

  shutdown(socket, SHUT_RDWR);
  close(socket);
}

void procedureF(int socket, int n)
{
  std::string filename, file, hash, timestamp, destiny;
  unpackF(socket, filename, file, hash, timestamp, destiny);

  std::string origin = nicks_inv[socket];

  int origin_size = origin.length();
  int filename_size = filename.size();
  int file_size = file.length();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << origin_size;
  std::string s1(ss1.str());

  std::ostringstream ss2;
  ss2 << std::setw(5) << std::setfill('0') << filename_size;
  std::string s2(ss2.str());

  std::ostringstream ss3;
  ss3 << std::setw(10) << std::setfill('0') << file_size;
  std::string s3(ss3.str());

  std::string message = "F" + s1 + origin + s2 + filename + s3 + file + hash + timestamp;
  std::cout << "message formatted: " << "F" + s1 + origin + s2 + filename + s3 + "FILE" + hash + timestamp << std::endl;
  send(nicks[destiny], message.c_str(), message.length(), 0);
}
