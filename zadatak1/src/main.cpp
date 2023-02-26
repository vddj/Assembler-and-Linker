#include<iostream>
#include "../inc/assembler.hpp"
using namespace std;

int main(int argc, char *argv[]){
    Assembler as;
    if(argv[1][0] != '-' || argv[1][1] != 'o') cout << "pogresni argumenti" << endl;
    else if (argc != 4){
        cout << "pogresan broj argumenata" << endl;
    }
    else {
        as.firstPass(argv[3]);
        as.secondPass(argv[3], argv[2]);
    }
    return 0;
}

// ./assembler -o izlaz.o tests/interrupts.s
// g++ -o assembler ./src/*.cpp