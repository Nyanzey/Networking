#include <fstream>
#include <algorithm>

void unpackF(int socket, std::string &origin, std::string &local_hash)
{
  //Format: F(1B)originSize(2B)origin(originSize)nameSize(5B)
  //filename(nameSizeB)fileSize(10B)file(fileSize)hash(10B)timestamp(14B?)

  char data[11];

  //origin
  recv(socket, data+1, 2, 0);
  int origin_size = getNum(data, 2);

  std::string originbuf(origin_size, '0');
  for (int i = 0; i < origin_size; i++)
  {
    recv(socket, &originbuf[i], 1, 0);
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

  origin = originbuf;
  local_hash = hashbuf;

  std::ofstream out(filenamebuf);
  out << filebuf;
  out.close();

  std::cout << "Received file: [" << filenamebuf << "] from [" << origin << "]" << std::endl;

  local_hash = hash(filebuf+timestampbuf);
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

void unpackM(int socket)
{
  //Format: M(1B)OriginSize(2B)Origin(OriginSizeB)MsgSize(3B)Msg(MsgSizeB)
  char data[4];
  recv(socket, data+1, 2, 0);
  int origin_size = getNum(data, 2);

  std::string originbuf(origin_size, '0');
  for (int i = 0; i < origin_size; i++)
  {
    recv(socket, &originbuf[i], 1, 0);
  }

  recv(socket, data+1, 3, 0);
  int msg_size = getNum(data, 3);

  std::string msgbuf(msg_size, '0');
  for (int i = 0; i < msg_size; i++)
  {
    recv(socket, &msgbuf[i], 1, 0);
  }

  std::cout << msgbuf << " : from [" << originbuf << "]" << std::endl;  
}

void unpackL(int socket)
{
  //Format: L(1B)Size(3B)Clients(SizeB)
  std::string buff;

  char data[4];
  recv(socket, data+1, 3, 0);
  int list_size = getNum(data, 3);

  std::cout << "Client list: " << std::endl;
  char p[1];
  for (int i = 0; i < list_size; i++)
  {
    recv(socket, p, 1, 0);
    if (*p == ',')
    {
      std::cout << "- " << buff << std::endl;
      buff = "";
      continue;
    }

    buff += *p;
  }
}

void unpackG(int socket)
{
  //Format: G(1B)Xsize(2B)Xs(Xsize)Osize(2B)Os(Osize)
  std::vector<int> Xs;
  std::vector<int> Os;

  char data[3];
  recv(socket, data+1, 2, 0);
  int Xsize = getNum(data, 2);

  char px[1];
  for (int i = 0; i < Xsize; i++)
  {
    recv(socket, px, 1, 0);
    Xs.push_back(*px - '0');
  }

  recv(socket, data+1, 2, 0);
  int Osize = getNum(data, 2);

  char po[1];
  for (int i = 0; i < Osize; i++)
  {
    recv(socket, po, 1, 0);
    Os.push_back(*po - '0');
  }

  std::cout << "-- Game updated --" << std::endl;
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      int num = i*3 + j;

      if (std::find(Xs.begin(), Xs.end(), num) != Xs.end())
      {
        std::cout << "X-";
      }
      else if (std::find(Os.begin(), Os.end(), num) != Os.end())
      {
        std::cout << "O-";
      }
      else
      {
        std::cout << " -";
      }
    }
    std::cout << std::endl;
  }
  std::cout << "-----------------------" << std::endl;

  char buff[10] = "OK";
  send(socket, buff, 10, 0);
}

void unpackP(int socket)
{
  //Format: L(1B)Size(3B)Clients(SizeB)
  std::string buff;

  char data[4];
  recv(socket, data+1, 3, 0);
  int list_size = getNum(data, 3);

  std::cout << "Players: " << std::endl;
  char p[1];
  for (int i = 0; i < list_size; i++)
  {
    recv(socket, p, 1, 0);
    if (*p == ',')
    {
      std::cout << "- " << buff << std::endl;
      buff = "";
      continue;
    }

    buff += *p;
  }

  char buffer[10] = "OK";
  send(socket, buffer, 10, 0);
}

void unpackE(int socket)
{
  //Format: E(1B)msgSize(2B)msg(msgSize)
  char data[3];
  recv(socket, data+1, 2, 0);
  int msgSize = getNum(data, 2);

  std::string message(msgSize, '0');
  for (int i = 0; i < msgSize; i++)
  {
    recv(socket, &message[i], 1, 0);
  }

  std::cout << "ERROR: " << message << std::endl;
}

void unpackw(char* data)
{
  std::cout << "YOU WIN!!" << std::endl;
}

void unpackl(char* data)
{
  std::cout << "YOU LOSE!!" << std::endl;
}

void unpackz(int socket)
{
  //Format: z(1B)winnerSize(2B)winner(winnerSizeB)
  char data[3];
  recv(socket, data+1, 2, 0);
  int winnerSize = getNum(data, 2);

  std::string winner(winnerSize, '0');
  for (int i = 0; i < winnerSize; i++)
  {
    recv(socket, &winner[i], 1, 0);
  }

  std::cout << "Winner is: " << winner << std::endl;
}
