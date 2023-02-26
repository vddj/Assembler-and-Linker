#ifndef ASSEMB_H_
#define ASSEMB_H_

#include<string>
#include<iostream>
#include<regex>
using namespace std;

class Assembler{

public:
    long long cnt = 0;
    char endOfAssembling = 0;
    char second = 0;
    int lineNum = 1;
    string section = "und";

    void firstPass(string filename);
    void secondPass(string filename, string fileOut);
    char readOneLine(string& oneLine);
    void writeOutputFile(string filename, string ss);
    string getInstruction(string oneLine);
    int convertInstruction(string s);
    void directiveFirstPass(string s);
    string directiveSecondPass(string s);
    string getFirstWord(string& oneLine);
    string readOperandJump(string s);
    string readOperandMov(string s);
    string convertOperand(string, char, regex, regex, regex, regex, regex, regex, regex, regex,regex);
    void readLabel(string line);
    string setOperand(int operand);
    int readLiteral(string literal);
};

#endif