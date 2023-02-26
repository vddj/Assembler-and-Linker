#include "../inc/symbol.hpp"
#include<sstream>
#include<iostream>
#include <iomanip>
using namespace std;

int Symbol::currID = 1;
std::list<Symbol> Symbol::listSymbols;

const char separator = ' ';
const int width = 7;
const int stringWidth = 14;

Symbol::Symbol(string name, string section, int value, string size, char resolved){
    this->ID = currID++;
    this->name = name;
    this->section = section;
    this->value = value;
    this->size = size;
    this->resolved = resolved;
}

void Symbol::newSymbol(string name, string section, int value, string size, char resolved){
    if(Symbol::search(name)){
        if(resolved == 1) Symbol::changeSymbol(name, section, value, size, 1);
        return;
    }
    Symbol symb(name, section, value, size, resolved);
    Symbol::listSymbols.push_back(symb);
}

char Symbol::search(string name){
    std::list<Symbol>::iterator it;
    for (it = listSymbols.begin(); it != listSymbols.end(); ++it){
        if(it->name == name) return 1;
    }
    return 0;
}
Symbol& Symbol::getSymbol(string name){
    std::list<Symbol>::iterator it;
    for (it = listSymbols.begin(); it != listSymbols.end(); ++it){
        if(it->name == name) return *it;
    }
    return *it;
}

void Symbol::changeSymbol(string name, string section, int value, string size, char resolved){
    std::list<Symbol>::iterator it;
    for (it = listSymbols.begin(); it != listSymbols.end(); ++it){
        if(it->name == name) {
            it->section = section;
            it->value = value;
            it->resolved = 1;
            return;
        }
    }
}


void Symbol::writeTable(){
    std::list<Symbol>::iterator it;
    cout << "\n Symbol table\n";
    cout << left << setw(width) << setfill(separator) << "ID";
    cout << left << setw(stringWidth) << setfill(separator) << "name";
    cout << left << setw(stringWidth) << setfill(separator) << "section";
    cout << left << setw(width) << setfill(separator) << "value";
    cout << left << setw(width) << setfill(separator) << "resolved";
    cout << endl;
    for (it = listSymbols.begin(); it != listSymbols.end(); ++it){
        cout << *it;
    }
}

void Symbol::writeTableToString(string& outString){
    stringstream ss;
    std::list<Symbol>::iterator it;
    ss << "\n Symbol table\n";
    ss << left << setw(width) << setfill(separator) << "ID";
    ss << left << setw(stringWidth) << setfill(separator) << "name";
    ss << left << setw(stringWidth) << setfill(separator) << "section";
    ss << left << setw(width) << setfill(separator) << "value";
    ss << left << setw(width) << setfill(separator) << "resolved";
    ss << endl;
    for (it = listSymbols.begin(); it != listSymbols.end(); ++it){
        ss << *it;
    }
    outString = ss.str();
}

ostream& operator<<(ostream& os, const Symbol& sym){
    os << left << setw(width) << setfill(separator) << std::hex << sym.ID;
    os << left << setw(stringWidth) << setfill(separator) << sym.name;
    os << left << setw(stringWidth) << setfill(separator) << sym.section;
    os << left << setw(width) << setfill(separator) << std::hex << sym.value;
    os << left << setw(width) << setfill(separator) << (int)sym.resolved;
    os << left << setw(width) << setfill(separator) << sym.size;
    os << endl;
    return os;
}

char Symbol::isInSection(Symbol& s, string section){
    if(s.section == section) return 1;
    if(s.section == "allSec") return 1;
    return 0;
}