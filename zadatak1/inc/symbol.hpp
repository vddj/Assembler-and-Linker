#ifndef SYMBOL_H_
#define SYMBOL_H_

#include<string>
#include<list>
using namespace std;

class Symbol {
public:
    static std::list<Symbol> listSymbols;
    static int currID;
    int ID;
    string name;
    string section;
    int value;
    int size;
    char resolved; // 1 izvoz, 0 uvoz

    static void newSymbol(string name, string section, int value, int size, char resolved);
    static void writeTable();
    static void writeTableToString(string& outString);
    friend std::ostream& operator<<(std::ostream& os, const Symbol& sym);
    static char search(string name);
    static Symbol& getSymbol(string name);
    static char isInSection(Symbol& s, string section);
    static void changeSymbol(string name, string section, int value, int size, char resolved);

private:
    Symbol(string name, string section, int value, int size, char resolved);
};

#endif