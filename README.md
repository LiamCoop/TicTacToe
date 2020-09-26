# Update
Imported and updated my project from UVic's SENG475-Robust and Efficient Computing in C++

# to-do:
-Change #defines to constexpr
-Implement method for the user to specify a difficulty when they choose to start the game.

# Tic-Tac-Toe

Play Tic-Tac-Toe against a computer adversary implementing the optimal strategy.
A player implementing this optimal strategy can win, or at least draw, but cannot lose.
Should you be unfamiliar with the rules of the game they are included at the end of this file.

The application uses C-style random-number-generation, using srand & rand functions included in <stdio.h> to simulate a coin-flip to determine whether the player or the adversary starts.

The computer adversary, henceforth referred to as "the adversary", will select the first available move from the following list.
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

