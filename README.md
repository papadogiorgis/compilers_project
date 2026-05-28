# Alpha Compiler and Virtual Machine
*CS-340 Languages and Compilers - University of Crete*

## Authors
* **Andreas Kantilierakis (csd5411)**
* **Georgios Papadakis (csd4975)**

## 1. Execution Environment
This project was compiled, tested, and verified on the computer named `mango` located in the CSD labs.

## 2. How to Run
To compile and execute the compiler:
1. Open your terminal and navigate to the `compilers/` directory.
2. Build the executable by running `make`.
3. To run the compiler with a text file as an argument use the command `./compiler test.txt`. You can use the flag `-print-syntax` to print the syntax analysis and the scope table.
4. The intermediate code (quads) will be generated and saved to a file named `all_quads.txt` in the same directory. Scope lists and phase 2 outputs are printed to the console.

*To clean up the generated files run `make clean`.*

## 3. Implementation Status
- **What works great:** The entire standard specification for Phase C has been completed successfully.
- **Short-Circuit Evaluation is Implemented.**
- **What works partially:** None. All implemented features are fully functional.