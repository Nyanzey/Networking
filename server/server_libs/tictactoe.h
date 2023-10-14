#include <vector>
#include <iostream>
#include <string>

class TictactoeGame
{
    public:
    char winner;
    char board[3][3];

    TictactoeGame()
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                board[i][j] = ' ';
            }
        }
        winner = '-';
    }

    void print()
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                std::cout << board[i][j] << "-";
            }
            std::cout << std::endl;
        }
    }

    // 1: range error
    // 2: occupied error
    // 3: success
    int move(int move, char player)
    {
        if (move < 1 || move > 9)
        {
            return 1;
        }
        move--;

        int x = move/3;
        int y = move%3;

        if (board[x][y] != ' ')
        {
            return 2;
        }
        
        board[x][y] = player;
        return 3;
    }

    bool check_win(int pos, char player)
    {
        int x = pos/3;
        int y = pos%3;

        winner = player;

        //check up
        int curx = x-1;
        int cury = y;
        
        int count = 1;
        while (curx >= 0 && board[curx][cury] == player)
        {
            count++;
            curx--;
        }
        if (count == 3){return true;}

        //check down
        curx = x+1;
        cury = y;
        
        count = 1;
        while (curx <= 2 && board[curx][cury] == player)
        {
            count++;
            curx++;
        }
        if (count == 3){return true;}

        //check right
        curx = x;
        cury = y+1;
        
        count = 1;
        while (cury <= 2 && board[curx][cury] == player)
        {
            count++;
            cury++;
        }
        if (count == 3){return true;}

        //check left
        curx = x;
        cury = y-1;
        
        count = 1;
        while (cury >= 0 && board[curx][cury] == player)
        {
            count++;
            cury--;
        }
        if (count == 3){return true;}

        //check left up
        curx = x-1;
        cury = y-1;
        
        count = 1;
        while (cury >= 0 && curx >= 0 && board[curx][cury] == player)
        {
            count++;
            cury--;
            curx--;
        }
        if (count == 3){return true;}

        //check left down
        curx = x+1;
        cury = y-1;
        
        count = 1;
        while (cury >= 0 && curx <= 2 && board[curx][cury] == player)
        {
            count++;
            cury--;
            curx++;
        }
        if (count == 3){return true;}

        //check rigth up
        curx = x-1;
        cury = y+1;
        
        count = 1;
        while (cury <= 2 && curx >= 0 && board[curx][cury] == player)
        {
            count++;
            cury++;
            curx--;
        }
        if (count == 3){return true;}

        //check right down
        curx = x+1;
        cury = y+1;
        
        count = 1;
        while (cury <= 2 && curx <= 2 && board[curx][cury] == player)
        {
            count++;
            cury++;
            curx++;
        }
        if (count == 3){return true;}

        winner = '-';
        return false;
    }

    bool finished()
    {
        bool res = false;
        for (int i = 0; i < 3 && !res; i++)
        {
            for (int j = 0; j < 3 && !res; j++)
            {
                if (board[i][j] != ' ')
                {
                    res = check_win(i*3 + j, board[i][j]);
                }
            }
        }
        return res;
    }

    //idx 0: X player
    //idx 1: O player
    std::vector<std::vector<int>> pack_board()
    {
        std::vector<std::vector<int>> res;
        std::vector<int> p1;
        std::vector<int> p2;

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (board[i][j] == 'X')
                {
                    p1.push_back(i*3 + j);
                }
                
                if (board[i][j] == 'O')
                {
                    p2.push_back(i*3 + j);
                }
            }
        }

        res.push_back(p1);
        res.push_back(p2);

        return res;
    }
};

class ActualTTTGame
{
    public:

    TictactoeGame board;
    char p1;
    char p2;
    std::string playername1;
    std::string playername2;
    bool turn;
    std::vector<std::string> viewers;

    ActualTTTGame(){}

    ActualTTTGame(char _p1, char _p2, std::string p1name, std::string p2name)
    {
        p1 = _p1;
        p2 = _p2;
        playername1 = p1name;
        playername2 = p2name;
        turn = true;
    }

    void set_params(char _p1, char _p2, std::string p1name, std::string p2name)
    {
        p1 = _p1;
        p2 = _p2;
        playername1 = p1name;
        playername2 = p2name;
    }

    // 0: not your turn
    // 1: range error
    // 2: occupied error
    // 3: success
    int move(int move, char player)
    {
        int opt;
        if (player == p1)
        {
            if (turn)
            {
                opt = board.move(move, player);
                if (opt == 3){turn = !turn;}
            }
            else
            {
                opt = 4;
            }
        }

        if (player == p2)
        {
            if (!turn)
            {
                opt = board.move(move, player);
                if (opt == 3){turn = !turn;}
            }
            else
            {
                opt = 4;
            }
        }
        return opt;
    }
};
