#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ncurses.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>

constexpr int player1 = 0; // X, computer
constexpr int player2 = 1; // O, player
constexpr int boardsize = 3;
constexpr int Xpair = 1;
constexpr int Opair = 2;
constexpr int tilepairs = 3;
constexpr int slashpair = 4;

namespace ttt {
    class game {
    
    private:
        std::vector<char> ttt;
        int available_;
    public:
        //default constructor
        game() : available_(9) { ttt = { '/','/','/','/','/','/','/','/','/' }; }

        //build intro
        game(bool tmp) : available_(9) {ttt = { '1','2','3','4','5','6','7','8','9' };}
        
        game(const game&) = default;
        game(game&&) = default;
        ~game() = default;

        //a getter operator returning the value of the game at the given index
        char operator[](int i) const { return ttt[i]; }

        //a function returning the number of available moves left on board.
        int available() const { return available_; }

        //provides a vector of int pairs with sums across all rows, columns, diags.
        std::vector<std::pair<int,int>> getlistings() const {
            std::vector<std::pair<int,int>> vec;
            //d = diagonal, x for x's, o for o's, l = left, r = right
            int dtmpxl = 0, dtmpol = 0, dtmpxr = 0, dtmpor = 0;
            for (auto i = 0; i < 3; ++i) {
                int rtmpx = 0, rtmpo = 0, ctmpx = 0, ctmpo = 0;
                for (auto j = 0; j < boardsize; ++j) {
                    if (ttt[i * boardsize + j] != '/') (ttt[i * boardsize + j] == 'X') ? ++rtmpx : ++rtmpo;
                    if (ttt[j * boardsize + i] != '/') (ttt[j * boardsize + i] == 'X') ? ++ctmpx : ++ctmpo;
                    if (i == 0) {
                        if (ttt[j * boardsize + j] != '/') ttt[j * boardsize + j] == 'X' ? ++dtmpxl : ++dtmpol;
                        if (ttt[(j + 1) * 2] != '/') ttt[(j + 1) * 2] == 'X' ? ++dtmpxr : ++dtmpor;
                    }
                }
                vec.push_back(std::make_pair(rtmpx, rtmpo));
                vec.push_back(std::make_pair(ctmpx, ctmpo));
            }
            vec.push_back(std::make_pair(dtmpxl, dtmpol));
            vec.push_back(std::make_pair(dtmpxr, dtmpor));
            return vec;
        }

        //function that places the provided position based on which player is provided as input 
        void place(bool player, int position) {
            char place_val = player ? 'O' : 'X';
            if (ttt[position] == '/') {
                if(has_colors() == TRUE){
                    init_pair(1, COLOR_RED, COLOR_BLACK);
                } 
                ttt[position] = place_val;
                --available_;
            }
            else { 
                printw("Invalid placement in member function"); 
                refresh();
            }
        }

        //undoes a placement
        void unplace(int position) {
            ttt[position] = '/';
            ++available_;
        }

    };

    //prints the horizontal lines in the output of a game
    void printlineh(){
        attron(COLOR_PAIR(tilepairs));
        printw("\n-+-+-\n");
        attroff(COLOR_PAIR(tilepairs));
    } 

    //prints the vertical lines in the output of a game
    void printlinev(){
        attron(COLOR_PAIR(tilepairs));
        printw("|");
        attroff(COLOR_PAIR(tilepairs));
    }

    //prints an instance of a game
    void printer(game obj) {
        //colours are registered as colour pairs in ncurses
        start_color();
        init_pair(Xpair, COLOR_RED, COLOR_BLACK);
        init_pair(Opair, COLOR_BLUE, COLOR_BLACK);
        init_pair(tilepairs, COLOR_GREEN, COLOR_BLACK);
        //make slashes "invisible" using black on black
        init_pair(slashpair, COLOR_BLACK, COLOR_BLACK);

        for (auto i = 0; i < boardsize; ++i) {
            if (i == 1) printlineh();
            for (auto j = 0; j < boardsize; ++j) {
                if (j == 1) printlinev();
                    int cp = COLOR_PAIR(obj[i*boardsize+j] == '/' ? 
                            slashpair : (obj[i*boardsize+j] == 'X' ? Xpair : Opair));
                    attron(cp);
                    std::string str(1, obj[i*boardsize+j]);
                    printw(str.c_str());
                    attroff(cp);
                if (j == 1) printlinev();
            }
            if (i == 1) printlineh();
        }
        printw("\n");
        refresh();
    } 
 
    //function for getting the player's input and validating it.
    int getpchoice(ttt::game game_) {
        printw("Please enter an unoccupied tile from 1-9\n");
        refresh();
        int ch;
        while (1) {
            //48 is ascii value for 0, used as offset for representing other values with arithmetic.
            ch = getch()-48;
            printw("\n");
            if (ch > 0 && ch <= 9) {
                if (game_[ch - 1] == '/') {
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

    //if the adversary is in a position to win, it should seize that.
    bool win(ttt::game& game) {
        refresh();//refresh the screen
        std::vector<std::pair<int, int>> vec = game.getlistings();
        std::pair<int, int> dl = *(vec.end() - 2);//diag starting in top left.
        std::pair<int, int> dr = *(vec.end() - 1);//diag starting in top right.
        for (auto it = vec.begin(); it != (vec.end() - 2); ++it) {
            if ((*it).first == 2 && (*it).second == 0) {//win opportunity detected in rows/columns
                refresh();
                bool RorC = ((it - vec.begin()) % 2) == 0 ? true : false;
                int location;
                for (auto i = 0; i < boardsize; ++i) {
                    location = RorC ? (((it - vec.begin()) / 2) * boardsize + i) : (i * boardsize + ((it - vec.begin()) / 2));
                    if (game[location] == '/') {
                        game.place(player1, location);
                        return 0;
                    }
                }
            }
        }
        bool LorR;
        if (dl.first == 2 && dl.second == 0) LorR = true;
        else if (dr.first == 2 && dr.second == 0) LorR = false;
        else return 1;
        for (auto i = 0; i < boardsize; ++i) {
            int location = LorR ? (i * boardsize + i) : ((i + 1) * 2);
            if (game[location] == '/') {
                game.place(player1, location);
                return 0;
            }
        }
    }

    //if the player is in a position to win, it should prevent that.
    bool block(ttt::game& game) {
        std::vector<std::pair<int, int>> vec = game.getlistings();
        std::pair<int, int> dl = *(vec.end() - 2);//diag starting in top left.
        std::pair<int, int> dr = *(vec.end() - 1);//diag starting in top right.
        for (auto it = vec.begin(); it != (vec.end() - 2); ++it) {
            if ((*it).first == 0 && (*it).second == 2) {//win opportunity detected in rows/columns
                bool RorC = (((it - vec.begin()) % 2) == 0) ? true : false;
                int location;
                for (auto i = 0; i < boardsize; ++i) {
                    location = RorC ? (((it - vec.begin()) / 2) * boardsize + i) : (i * boardsize + ((it - vec.begin()) / 2));
                    if (game[location] == '/') {
                        game.place(player1, location);
                        return 0;
                    }
                }
            }
        }
        bool LorR;
        if (dl.first == 0 && dl.second == 2) LorR = true;
        else if (dr.first == 0 && dr.second == 2) LorR = false;
        else return 1;
        for (auto i = 0; i < boardsize; ++i) {
            int location = LorR ? (i * boardsize + i) : (i + 1) * 2;
            if (game[location] == '/') {
                game.place(player1, location);
                return 0;
            }
        }
    }

    //create a forking opportunity (2 unblocked rows)
    bool fork(ttt::game& game) {
        if (game.available() < 6) {
            for (int i = 0; i < boardsize * boardsize; ++i) {
                if(game[i] != 'O' && game[i] != 'X'){
                    if (game[i] == '/') game.place(player1, i);
                    std::vector<std::pair<int, int>> vec = game.getlistings();
                    int available_wins = 0;
                    for (auto it = vec.begin(); it != vec.end(); ++it) {
                        if ((*it).first == 2) ++available_wins;
                    }
                    if (available_wins >= 2) return 0;
                    else game.unplace(i);
                }
            }
        }
        return 1;
    }

    //prevent a forking opportunity for the player
    bool bl_fork(ttt::game& game) {
        if (game.available() < 6) {
            for (int i = 0; i < boardsize * boardsize; ++i) {
                if (game[i] != 'O' && game[i] != 'X') {
                    if (game[i] == '/') game.place(player2, i);
                    std::vector<std::pair<int, int>> vec = game.getlistings();
                    int available_wins = 0;
                    for (auto it = vec.begin(); it != vec.end(); ++it) {
                        if ((*it).second == 2) ++available_wins;
                    }
                    game.unplace(i);
                    if (available_wins == 2) {
                        game.place(player1, i);
                        return 0;
                    }
                }
            }
        }
        return 1;
    }

    //play the center if available
    bool center(ttt::game& game) {
        if (game[4] == '/') {
            game.place(player1, 4);
            return 0;
        }
        return 1;
    }

    //play the corner opposite the player
    bool opp_corner(ttt::game& game) {
        //if player owns a corner and the opposite is available, play it.
        int location = -1;
        if ((game[0] == '/' && game[8] == 'O') || (game[8] == '/' && game[0] == 'O')) location = game[8] == '/' ? 8 : 0;
        else if ((game[2] == '/' && game[6] == 'O') || (game[6] == '/' && game[2] == 'O')) location = game[6] == '/' ? 6 : 2;
        if (location == -1) return 1;
        else game.place(player1, location);
        return 0;
    }

    //control corners.
    bool empty_corner(ttt::game& game) {
        //place any available corner, center can't be played at this time (otherwise we would've)
        for (auto i = 0; i < 9; ++i) {
            if ((i % 2 == 0) && (game[i] == '/')) {
                game.place(player1, i);
                return 0;
            }
        }
        return 1;
    }
    
    //when in doubt play an available side
    bool empty_side(ttt::game& game) {
        for (auto i = 0; i < 9; ++i) {
            if ((i % 2 != 0) && (game[i] == '/')) {
                game.place(player1, i);
                return 0;
            }
        }
        return 1;
    }

    //OC: opposite corner, EC: empty corner, ES: empty side.
    enum class move { WIN, BLOCK, FORK, BL_FORK, CENTER, OC, EC, ES };

    //controller for the adversary.
    void reply(ttt::game& game) {
        move tmp = move::WIN;
        switch (tmp) {
        case move::WIN:
            if (win(game)) tmp = move::BLOCK;
            else break;
        case move::BLOCK:
            if (block(game)) tmp = move::CENTER;
            else break;
        case move::FORK:
            if (fork(game)) tmp = move::BL_FORK;
            else break;
        case move::BL_FORK:
            if (bl_fork(game)) tmp = move::CENTER;
            else break;
        case move::CENTER:
            if (center(game)) tmp = move::OC;
            else break;
        case move::OC:
            if (opp_corner(game)) tmp = move::EC;
            else break;
        case move::EC:
            if (empty_corner(game)) tmp = move::ES;
            else break;
        case move::ES:
            if (!empty_side(game)) break;
        default:
            printw("Switch default hit, big problem\n");
            refresh();
            break;
        }
    }

    //check for a winner or a draw condition
    void check(const ttt::game game, bool& done) {
        std::vector<std::pair<int, int>> vec = game.getlistings();
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if ((((*it).first) == 3) || ((*it).second == 3)) {
                printer(game);
                printw("\n");
                std::string winner = (*it).first == 3 ? "COMPUTER" : "PLAYER";
                printw("The winner is: ");
                addstr(winner.c_str());
                printw("\n");
        
                refresh();
                done = true;
                return;
            } 
        }
        if (game.available() == 0) {
            printw("The game has resulted in a draw\n");
            printer(game);
            printw("\n");
            refresh();
            done = true;
            return;
        }
    }

    //main game loop
    void run_game(std::vector<int> movesvec = std::vector<int>(0)) {
        printw("To make your move each turn, select a grid element by inputting the appropriate number from the following grid:\n\n");
        
        printer(game(1));
        printw("\n");
        refresh();
        
        //coin flip to pick who goes first.
        srand(time(NULL));
        int start = ((rand() % 2) == 0);

        ttt::game game_;
        bool done = 0;
        auto it = movesvec.begin();
        do {
            if ((start % 2) != 0) {
                printer(game_);
                refresh();
                int move = ((movesvec.size() > 0) ? (*it) : getpchoice(game_)) - 1;
                game_.place(player2, move);
                printw(std::to_string(move+1).c_str()); 
                printw(" was entered\n");
                movesvec.size() ? ++it : it;
                check(game_, done);
            } else {
                reply(game_);
                check(game_, done);
            }
            ++start;
            if (done)break;
        } while (!done);
        printw("\nGame over. Press any key to exit.\n");
        refresh();
    }
}


int main(int argc, char* argv[]) {
    /*std::vector<int> movesvec;
    if(argv[1]){
        std::ifstream file;
        file.open(argv[1]+2, std::ios::in);
        std::string tmp;
        while(std::getline(file,tmp)){
            movesvec.push_back(std::stoi(tmp));
        } 
    }
    */
    //ncurses settings / initialization.
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    //run the game loop
    ttt::run_game(movesvec);
    //if we're running the demo script don't wait for user input to close
    /*
    if(movesvec.size() != 0) {
        timeout(4000);
    } 
    */
    getch();
    endwin();
    exit(0);
}
