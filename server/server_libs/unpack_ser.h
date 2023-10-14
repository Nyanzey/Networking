#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <algorithm>
#include <fstream>

void unpackN(int socket, std::string &name)
{
  char data[3];
  recv(socket, data, 1, 0);

  recv(socket, data+1, 2, 0);
  int size = getNum(data, 2);

  std::string datastr(size, '0');
  
  for (int i = 0; i < size; i++)
  {
    recv(socket, &datastr[i], 1, 0);
  }

  name = datastr;
}

void unpackR(int socket, std::string &origin, std::string &hash)
{
  char data[3];
  recv(socket, data+1, 2, 0);
  int origin_size = getNum(data, 2);

  std::string originbuf(origin_size, '0');
  for (int i = 0; i < origin_size; i++)
  {
    recv(socket, &originbuf[i], 1, 0);
  }

  std::string hashbuf(10, '0');
  for (int i = 0; i < 10; i++)
  {
    recv(socket, &hashbuf[i], 1, 0);
  }

  origin = originbuf;
  hash = hashbuf;
}

void unpackM(int socket, std::string &destiny, std::string &msg)
{
  // Format: M(1B)DestinySize(2B)Destiny(DestinySizeB)MsgSize(3B)Msg(MsgSizeB)

  char data[4];
  recv(socket, data+1, 2, 0);
  int destiny_size = getNum(data, 2);

  std::string destbuf(destiny_size, '0');
  for (int i = 0; i < destiny_size; i++)
  {
    recv(socket, &destbuf[i], 1, 0);
  }

  recv(socket, data+1, 3, 0);
  int msg_size = getNum(data, 3);

  std::string msgbuf(msg_size, '0');
  for (int i = 0; i < msg_size; i++)
  {
    recv(socket, &msgbuf[i], 1, 0);
  }
  
  destiny = destbuf;
  msg = msgbuf;
}

void unpackW(int socket, std::string &msg)
{
  // Format: W(1B)Size(2B)Msg(SizeB)

  char data[3];
  recv(socket, data+1, 2, 0);
  int msg_size = getNum(data, 2);

  std::string msgbuf(msg_size, '0');
  for (int i = 0; i < msg_size; i++)
  {
    recv(socket, &msgbuf[i], 1, 0);
  }

  msg = msgbuf;
}

void unpackF(int socket, std::string &filename, std::string &file, std::string &hash, std::string &timestamp, std::string &destiny)
{
  //Format: F(1B)destinySize(2B)destiny(destinySize)nameSize(5B)
  //filename(nameSizeB)fileSize(10B)file(fileSize)hash(10B)timestamp(14B?)

  char data[11];

  //destiny
  recv(socket, data+1, 2, 0);
  int destiny_size = getNum(data, 2);

  std::string destinybuf(destiny_size, '0');
  for (int i = 0; i < destiny_size; i++)
  {
    recv(socket, &destinybuf[i], 1, 0);
  }

  //filename
  recv(socket, data+1, 5, 0);
  int filename_size = getNum(data, 5);

  std::string filenamebuf(filename_size, '0');
  for (int i = 0; i < filename_size; i++)
  {
    recv(socket, &filenamebuf[i], 1, 0);
  }

  //file
  recv(socket, data+1, 10, 0);
  int file_size = getNum(data, 10);

  std::string filebuf(file_size, '0');
  for (int i = 0; i < file_size; i++)
  {
    recv(socket, &filebuf[i], 1, 0);
  }

  //hash

  std::string hashbuf(10, '0');
  for (int i = 0; i < 10; i++)
  {
    recv(socket, &hashbuf[i], 1, 0);
  }

  //timestamp

  std::string timestampbuf(14, '0');
  for (int i = 0; i < 14; i++)
  {
    recv(socket, &timestampbuf[i], 1, 0);
  }

  destiny = destinybuf;
  filename = filenamebuf;
  file = filebuf;
  hash = hashbuf;
  timestamp = timestampbuf;
}
