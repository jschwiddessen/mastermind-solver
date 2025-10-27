# mastermind-solver
A parallel C program that plays Mastermind for you.

This is **not** a full implementation of the board game _Mastermind_; it's an engine that helps you win it in as few guesses as possible.

The computations are parallelized with _OpenMP_. You can choose between two strategies: optimize the _average_ case or the _worst_ case.

## Settings
Before compilation, adjust three constants at the top of the source file:

- `NUM_COLORS`: number of available colors (default: `6`)
- `CODE_LENGTH`: length of the secret code (default: `4`)
- `WORST_CASE`: optimize worst case if `true`, average case if `false`

## Compilation
After changing the constants, compile with:
```bash
gcc -O3 -fopenmp mastermind-solver.c -o mastermind
```

## Running the solver
Example run with `CODE_LENGTH=4`, `NUM_COLORS=6`, and `WORST_CASE=false`:
```bash
./mastermind yellow red blue green pink white
```
If the secret code were **[green yellow yellow pink]**, the interaction could look like this:
```
SETTINGS:

Code length: 4
Number of colors: 6
Maximum number of guesses: 12
Mode: average case
Colors: yellow red blue green pink white

********************************************************************************

There are still 1296 valid combinations.
The expected number of valid combinations after the next reply is 185.27.

My proposed combination for guess 1 is:

   white     pink   yellow    white 

My proposed guess is valid and could be the solution.

Please enter the number of black pins: 1
Please enter the number of white pins: 1

********************************************************************************

There are still 230 valid combinations.
The expected number of valid combinations after the next reply is 27.20.

My proposed combination for guess 2 is:

    blue    white    green    white 

My proposed guess is valid and could be the solution.

Please enter the number of black pins: 0
Please enter the number of white pins: 1

********************************************************************************

There are still 41 valid combinations.
The expected number of valid combinations after the next reply is 4.05.

My proposed combination for guess 3 is:

    pink   yellow   yellow     blue 

My proposed guess is valid and could be the solution.

Please enter the number of black pins: 2
Please enter the number of white pins: 1

********************************************************************************

There are still 4 valid combinations.
The expected number of valid combinations after the next reply is 0.75.

My proposed combination for guess 4 is:

   green   yellow   yellow     pink 

My proposed guess is valid and could be the solution.

Please enter the number of black pins: 4
Please enter the number of white pins: 0

Done! Solved in 4 guesses!
```

## What the code does
The solver is randomized and will usually give different suggestions even with the same input.

To propose the next guess, it evaluates all possible guesses and assigns a score. It assumes every currently feasible secret code is equally likely, which gives probabilities for each reply (counts of black and white pins). Using this, the solver minimizes the number of potential solutions after the next reply: with `WORST_CASE=false`, it minimizes the **expected** number; with `WORST_CASE=true`, it minimizes the **maximum** number across all replies.
