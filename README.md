# CS-340 Compilers Project - Phase C

## Authors
* **Andreas Kantilierakis (csd5411)**
* **Georgios Papadakis (csd4975)**

## 1. Execution Environment
This project was compiled, tested, and verified on the computer named `mango` located in the CSD labs.

## 2. How to Run
To compile and execute the intermediate code generator:
1. Open your terminal and navigate to the `compilers/` directory.
2. Build the executable by running `make`.
3. Run the parser and pass the input text file as an argument: `./parse test.txt`
4. The intermediate code (quads) will be generated and saved to a file named `all_quads.txt` in the same directory. Scope lists and phase 2 outputs are printed to the console.

*To clean up the generated files run `make clean`.*

## 3. Implementation Status
- **What works great:** The entire standard specification for Phase C has been completed successfully.
- **Short-Circuit Evaluation is Implemented.**
- **What works partially:** None. All implemented features are fully functional.