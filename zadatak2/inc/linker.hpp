#ifndef LINKER_H_
#define LINKER_H_

#include<fstream>
#include<sstream>
#include<iomanip>
#include<iostream>
#include<list>
#include<string>
using namespace std;

typedef struct {
    int mesto;
    int velicina;
    string naziv;
    string podaci;
} Sekcija;

class Linker{
public:
    char endOfFile = 0;
    int cnt = 0, velikiBroj = 1001;
    stringstream ss, ss1;
    fstream newfile;
    string currSection = "";
    static std::list<Sekcija> lista;
    static std::list<string> datoteke;

    void readFile(string filename);
    void readFileLinkable(string filename);
    void readCode(string filename);
    void readCodeLinkable(string filename);
    void readTables(string filename);
    void readRelocation(string oneLine);
    void readSection(string s);
    void readSymbol(string oneLine);
    string getFirstWord(string& oneLine);
    void razresiSimbole();
    void writeFile(string filename, string s);
    void writeFileLinkable(string filename, string s);
    string setOperand(int operand);
    static string newSekcija(int velicina);
    static void newSekcija(int mesto, string naziv);
    static void newSekcija(int mesto, string naziv, string text);
    static void ispis();
    char search(string name);
    void addText(string name, string text);
    void raspodelaSekcija();
};

#endif