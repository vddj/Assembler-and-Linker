#include "../inc/relocation.hpp"
#include<sstream>
#include<iostream>
#include <iomanip>
using namespace std;

std::list<Relocation> Relocation::listRelocation;

const char separator = ' ';
const int width = 7;
const int stringWidth = 10;

Relocation::Relocation(string section, int offs,string type, string symbol){
    this->section = section;
    this->offset = offs;
    this->type = type;
    this->symbol = symbol;
}

void Relocation::newRelocation(string s, int o,string t, string sim){
    Relocation rel(s, o, t, sim);
    listRelocation.push_back(rel);
}

void Relocation::writeTable(){
    std::list<Relocation>::iterator it;
    cout << "\n Relocation table\n";
    cout << left << setw(stringWidth) << setfill(separator) << "section";
    cout << left << setw(width) << setfill(separator) << "offset";
    cout << left << setw(stringWidth) << setfill(separator) << "type";
    cout << left << setw(width) << setfill(separator) << "symbol";
    cout << endl;
    for (it = listRelocation.begin(); it != listRelocation.end(); ++it){
        cout << *it;
    }
}

void Relocation::writeTableToString(string& outString){
    std::list<Relocation>::iterator it;
    stringstream ss;
    ss << "\n Relocation table\n";
    ss << left << setw(stringWidth) << setfill(separator) << "section";
    ss << left << setw(width) << setfill(separator) << std::hex << "offset";
    ss << left << setw(stringWidth) << setfill(separator) << "type";
    ss << left << setw(width) << setfill(separator) << "symbol";
    ss << endl;
    for (it = listRelocation.begin(); it != listRelocation.end(); ++it){
        ss << *it;
    }
    outString = ss.str();
}

std::ostream& operator<<(std::ostream& os, const Relocation& sym){
    os << left << setw(stringWidth) << setfill(separator) << sym.section;
    os << left << setw(width) << setfill(separator) << sym.offset;
    os << left << setw(stringWidth) << setfill(separator) << sym.type;
    os << left << setw(width) << setfill(separator) << sym.symbol;
    os << endl;
    return os;
}