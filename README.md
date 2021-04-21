
![Screenshot from 2021-04-12 23-27-01](https://user-images.githubusercontent.com/22286436/114506441-a51f3000-9be6-11eb-8f45-c99132235fa9.png)

## Tic-Tac-Toe

Play Tic-Tac-Toe against a computer adversary implementing the optimal strategy.
A player implementing this optimal strategy can win, or at least draw, but cannot lose.


# Installation & compilation
Clone the github repository.
Execute `cmake -H. -Btmp` to compile.
Execute `cmake --build tmp` to build & link files.
The application can then be started with `./tmp/tictactoe`.
Note that the application does the require the ncurses library, https://invisible-island.net/ncurses/announce.html
Installation instructions for macOS, Ubuntu, are included below.
# macOS using brew
`brew install ncurses`

# Debian / Ubuntu using apt
`sudo apt-get install libncurses5-dev libncursesw5-dev`

# Fedora / centOS, etc. using yum
`sudo yum install ncurses-devel`

The application uses C-style random-number-generation, using srand & rand functions included in <stdio.h> to simulate a coin-flip to determine which player gets to go first.

The computer adversary, selects the first available move from the following list.
1) Win, if in a position to win, the adversary will seize that opportunity.
2) Block, if the player is in a position to win, the adversary will prevent it by playing the "winning" element.
3) Fork, Create an opportunity where the adversary has two ways to win (two non-blocked lines of 2).
4) Block, Prevent the player creating an opportunity with two ways to win.
5) Center, if the center is available, the adversary will play this element.
6) Opposite Corner, if the corner opposite to the player is available, the adversary will play there.
7) Empty Corner, the adversary will play in an available corner.
8) Empty Side, the adversary will take an open square in the middle on any side of the square.

The rules of tic-tac-toe are as follows:
    1) The board is a 3x3 grid.
    2) The player is O, and the adversary is X, each player plays a sequence of turns to attempt to create "three-in-a-row" within a 3x3 grid, a player that constructs such a sequence is deemed the winner.
    3) A move that has been previously made may not be remade, (an element containing an 'X' or an 'O' cannot be played again).
    4) The game is over when a player has won, or all possible moves have been consumed.
