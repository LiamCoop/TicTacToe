#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ncurses.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

constexpr int Xpair = 1;
constexpr int Opair = 2;
constexpr int tilepairs = 3;
constexpr int slashpair = 4;

namespace ttt {

    //prints an instance of a game
    void printer(const char game[3][3]) {
        //colours are registered as colour pairs in ncurses
        start_color();
        init_pair(Xpair, COLOR_RED, COLOR_BLACK);
        init_pair(Opair, COLOR_BLUE, COLOR_BLACK);
        init_pair(tilepairs, COLOR_GREEN, COLOR_BLACK);
        //make slashes "invisible" using black on black
        init_pair(slashpair, COLOR_BLACK, COLOR_BLACK);

        for (auto i = 0; i < 3; ++i) {
            if (i == 1) {
              attron(COLOR_PAIR(tilepairs));
              printw("\n-+-+-\n");
              attroff(COLOR_PAIR(tilepairs));
            }
            for (auto j = 0; j < 3; ++j) {
                if (j == 1) {
                  attron(COLOR_PAIR(tilepairs));
                  printw("|");
                  attroff(COLOR_PAIR(tilepairs));
                }
                int cp = COLOR_PAIR(game[i][j] == '/' ? 
                        slashpair : (game[i][j] == 'X' ? Xpair : Opair));
                attron(cp);
                std::string str(1, game[i][j]);
                printw(str.c_str());
                attroff(cp);
                if (j == 1) {
                  attron(COLOR_PAIR(tilepairs));
                  printw("|");
                  attroff(COLOR_PAIR(tilepairs));
                }
            }
            if (i == 1) {
              attron(COLOR_PAIR(tilepairs));
              printw("\n-+-+-\n");
              attroff(COLOR_PAIR(tilepairs));
            }
        }
        printw("\n");
        refresh();
    } 
 
    //function for getting the player's input and validating it.
    int getpchoice(char game[3][3]) {
        printw("Please enter an unoccupied tile from 1-9\n");
        refresh();
        int ch;
        while (1) {
            //subtract 48, ascii value for 0
            ch = getch()-48;
            printw("\n");
            if (ch > 0 && ch <= 9) {
                if (game[(ch-1) / 3][(ch-1) % 3] == '/') {
                    break;
                } else {
                    printw("\nPlease enter an unoccupied tile\n");
                }
            } else {
                printw("\nThe tile entered must be a value between 1 and 9, and not have been previously played\n");
            }
            refresh();
        }
        return ch;
    }

    bool allEqual(char a, char b, char c) {
      return (a == b && a == c && a != '/');
    }

    char check(char game[3][3]) {
      bool spotsAvailable = false;
      for(auto i=0; i < 3; ++i) {
        if(game[i][0] == '/' || game[i][1] == '/' || game[i][2] == '/') spotsAvailable = true;
        if(allEqual(game[i][0], game[i][1], game[i][2])) return game[i][0];
        if(allEqual(game[0][i], game[1][i], game[2][i])) return game[0][i];
      }
      if(allEqual(game[0][0], game[1][1], game[2][2])) return game[1][1];
      if(allEqual(game[0][2], game[1][1], game[2][0])) return game[1][1];

      return spotsAvailable ? ' ' : '/';
    }

    //if the adversary is in a position to win, it should seize that.
    bool win(char (&game)[3][3]) {
        for(auto i = 0; i < 3; ++i) {
        for(auto j = 0; j < 3; ++j) {
          if(game[i][j] == '/') {
            game[i][j] = 'X';
            if(check(game) == 'X') {
              return 1;
            } else game[i][j] = '/';
          }
        }
      } 
      return 0;
    }

    //if the player is in a position to win, it should prevent that.
    bool block(char (&game)[3][3]) {
      for(auto i = 0; i < 3; ++i) {
        for(auto j = 0; j < 3; ++j) {
          if(game[i][j] == '/') {
            game[i][j] = 'O';
            if(check(game) == 'O') {
              game[i][j] = 'X';
              return 1;
            } else game[i][j] = '/';
          }
        }
      } 
      return 0;
    }

    bool is_forked(char game[3][3]){
      std::vector<std::pair<int,int>> vec;
      // x for x's, o for o's, l = left, r = right, d = diagonal
      int dtmpxl = 0, dtmpol = 0, dtmpxr = 0, dtmpor = 0;
      for (auto i = 0; i < 3; ++i) {
          int rtmpx = 0, rtmpo = 0, ctmpx = 0, ctmpo = 0;
          for (auto j = 0; j < 3; ++j) {
              if (game[i][j] != '/') (game[i][j] == 'X') ? ++rtmpx : ++rtmpo;
              if (game[j][i] != '/') (game[j][i] == 'X') ? ++ctmpx : ++ctmpo;
              if (i == 0) {
                  if (game[j][j] != '/') 
                    game[j][j] == 'X' ? ++dtmpxl : ++dtmpol;
                  if (game[(j+1) * 2 / 3][(j+1) * 2 %3] != '/') 
                    game[(j+1) * 2 / 3][(j+1) * 2 %3] == 'X' ? ++dtmpxr : ++dtmpor;
              }
          }
          vec.push_back(std::make_pair(rtmpx, rtmpo));
          vec.push_back(std::make_pair(ctmpx, ctmpo));
      }
      vec.push_back(std::make_pair(dtmpxl, dtmpol));
      vec.push_back(std::make_pair(dtmpxr, dtmpor));

      int winOps = 0;
      for(auto it=vec.begin(); it != vec.end(); ++it) {
        if( ((*it).first == 0 && (*it).second == 2) || ((*it).first == 2 && (*it).second == 2)) {
          ++winOps;
        }
      }
      return (winOps >= 2);
    }
    
    //create a forking opportunity (2 unblocked rows)
    bool fork(char (&game)[3][3]) {
      for(auto i=0; i < 3; ++i){
        for(auto j=0; j < 3; ++j){
          if(game[i][j] == '/') {
            game[i][j] = 'X';
            if(is_forked(game)) return 1;
            else game[i][j] = '/';
          }
        }
      }
      return 0;
    }

    //prevent a forking opportunity for the player
    bool bl_fork(char (&game)[3][3]) {
      for(auto i=0; i < 3; ++i){
        for(auto j=0; j < 3; ++j){
          if(game[i][j] == '/') {
            game[i][j] = 'O';
            if(is_forked(game)) {
              game[i][j] = 'X'; 
              return 1;
            } else game[i][j] = '/';
          }
        }
      }
      return 0;
    }

    //play the center if available
    bool center(char (&game)[3][3]) {
        if (game[1][1] == '/') {
            game[1][1] = 'X';
            return 1;
        }
        return 0;
    }

    //play the corner opposite the player
    bool opp_corner(char (&game)[3][3]) {
      if(game[0][0] == 'O' && game[2][2] == '/') {
        game[2][2] = 'X';
        return 1;
      }
      if(game[2][2] == 'O' && game[0][0] == '/') {
        game[0][0] = 'X';
        return 1;
      }
      if(game[0][2] == 'O' && game[2][0] == '/') {
        game[2][0] = 'X';
        return 1;
      }
      if(game[2][0] == 'O' && game[0][2] == '/'){
        game[0][2] = 'X';
        return 1;
      }
      return 0;
    }

    //control corners.
    bool empty_corner(char (&game)[3][3]) {
      //place any available corner, center can't be played at this time (otherwise we would've)
      for (auto i = 0; i < 3; ++i) {
        if(i==1) continue;
        for (auto j = 0; j < 3; ++j) {
          if(j == 1) continue;
          if(game[i][j] == '/') {
            game[i][j] = 'X';
            return 1;
          }
        }
      }
      return 0;
    }
    
    //when in doubt play an available side
    bool empty_side(char (&game)[3][3]) {
      for (auto i = 0; i < 3; ++i) {
        for (auto j = 0; j < 3; ++j) {
          if((i*3+j % 2 == 0) || (i*3+j == 4)) continue;
          if(game[i][j] == '/') {
            game[i][j] = 'X';
            return 1;
          }
        }
      }
      return 0;
    }

    //OC: opposite corner, EC: empty corner, ES: empty side.
    enum class move { WIN, BLOCK, FORK, BL_FORK, CENTER, OC, EC, ES };

    //controller for the adversary.
    void reply(char (&game)[3][3]) {
        move tmp = move::WIN;
        switch (tmp) {
        case move::WIN:
            if (win(game)) break;
            tmp = move::BLOCK;
        case move::BLOCK:
            if (block(game)) break;
            tmp = move::FORK;
        case move::FORK:
            if (fork(game)) break;
            tmp = move::BL_FORK;
        case move::BL_FORK:
            if (bl_fork(game)) break;
            tmp = move::CENTER;
        case move::CENTER:
            if (center(game)) break;
            tmp = move::OC;
        case move::OC:
            if (opp_corner(game)) break;
            tmp = move::EC;
        case move::EC:
            if (empty_corner(game)) break;
            tmp = move::ES;
        case move::ES: empty_side(game);
            break;
        default:
            refresh();
            break;
        }
    }
    
    //main game loop
    void run_game(int diffNum) {
        printw("To make your move, select the grid element using a number from the following grid:\n\n");
        char gameIntro[3][3] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

        printer(gameIntro);
        printw("\n");
        refresh();
        
        //coin flip to pick who goes first.
        srand(time(NULL));
        int start = ((rand() % 2) == 0);

        char game[3][3] = {'/', '/', '/', '/', '/', '/', '/', '/', '/'};
        char winner = ' ';
        do {
            if ((start % 2)!= 0) {
                //player turn
                printer(game);
                refresh();
                int pchoice = getpchoice(game) - 1;
                int i = pchoice / 3;
                int j = pchoice % 3;
                game[i][j] = 'O';
                printw("%s was entered\n", std::to_string(i*3+j+1).c_str()); 
                winner = check(game);
            } else {
              reply(game);
              winner = check(game);
            }
            ++start;
        } while (winner == ' ');
        printer(game);
        printw("\n");
        if(winner == '/') printw("Game was a draw.");
        else printw("Winner is: %s", winner == 'X' ? "AI":"Player");
        refresh();
        printw("\nGame over. Press any key to exit.\n");
    }
}


int main(int argc, char* argv[]) {
    //default difficulty to medium
    int diffNum = 1;
    if(argv[1]){ 
      std::string diffArr [4] = {"easy", "medium", "hard", "unbeatable"};
      for(auto i = 0; i < 4; ++i) {
        if(argv[1] == diffArr[i]) diffNum = i;
      }
    }
    
    //ncurses settings / initialization.
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);

    //run the game loop, passing difficulty
    ttt::run_game(diffNum);

    // get character to close window
    getch();
    endwin();

    exit(0);
}
