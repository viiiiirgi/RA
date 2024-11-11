# Assignment 2 RA-MIRI

## Compilation
1. Install gnuplot via the package manager
2. `gcc assignment2.c`

## Running the program
`assignment2 <m-bins> <n-balls> <one-choice|two-choice|three-choice|four-choice|ten-choice> <d> [repeat-T-times] [batch (integer >0)] [k-choices]`

batch and k-choices are optional but are positional argument. For this reason, when using k-choices, `batch` must be set to 0 (SEE EXAMPLES)

## EXAMPLES
`./out 300 90000 three-choice 3`

`./out 300 90000 three-choice 3 10` with 10 runs

`./out 300 300 three-choice 3 10 300` b-batch mode

`./out 300 90000 three-choice 3 10 0 1` k-choice = 1 (see assignment). Note that in this case b-batch is set to 0
