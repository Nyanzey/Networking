#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>

std::string packL(std::vector<std::string> nick_list)
{
  // Format: L(1B)Size(3B)Clients(SizeB)
  std::string list;
  for (int i = 0; i < nick_list.size(); i++)
  {
    list += (nick_list[i] + ",");
  }

  int listSize = list.length();

  std::ostringstream ss1;
  ss1 << std::setw(3) << std::setfill('0') << listSize;
  std::string s1(ss1.str());

  std::string message = "L" + s1 + list;
  return message;
}

std::string packM(std::string msg, std::string origin)
{
  // M(1B)OriginSize(2B)Origin(OriginSizeB)MsgSize(3B)Msg(MsgSizeB)
  int msgSize = msg.length();
  int orgSize = origin.length();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << orgSize;
  std::string s1(ss1.str());

  std::ostringstream ss2;
  ss2 << std::setw(3) << std::setfill('0') << msgSize;
  std::string s2(ss2.str());

  std::string message = "M" + s1 + origin + s2 + msg;
  return message;
}

std::string packG(std::vector<std::vector<int>> board)
{
  //Format: G(1B)Xsize(2B)Xs(Xsize)Osize(2B)Os(Osize)
  std::string message = "G";

  int Xsize = board[0].size();
  int Osize = board[1].size();

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << Xsize;
  std::string s1(ss1.str());

  std::ostringstream ss2;
  ss2 << std::setw(2) << std::setfill('0') << Osize;
  std::string s2(ss2.str());

  message += s1;
  for (int i = 0; i < Xsize; i++)
  {
    message += (board[0][i] + '0');
  }

  message += s2;
  for (int i = 0; i < Osize; i++)
  {
    message += (board[1][i] + '0');
  }

  return message;
}

std::string packP(std::vector<std::string> players)
{
  // Format: L(1B)Size(3B)Clients(SizeB)
  std::string list;
  for (int i = 0; i < players.size(); i++)
  {
    list += (players[i] + ",");
  }

  int listSize = list.length();

  std::ostringstream ss1;
  ss1 << std::setw(3) << std::setfill('0') << listSize;
  std::string s1(ss1.str());

  std::string message = "P" + s1 + list;
  return message;
}

std::string packE(int err)
{
  //Format: E(1B)msgSize(2B)msg(msgSize)

  std::string message;

  if (err == 1)
  {
    message = "Invalid move";
  }
  else if (err == 2)
  {
    message = "Already occupied";
  }
  else
  {
    message = "Not your turn";
  }

  std::ostringstream ss1;
  ss1 << std::setw(2) << std::setfill('0') << message.length();
  std::string s1(ss1.str());

  std::string res = "E" + s1 + message;
  return res;
}
