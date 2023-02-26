#include "../inc/section.hpp"
#include<sstream>
#include<iostream>
#include <iomanip>
using namespace std;

std::list<Section> Section::listSection;

const char separator = ' ';
const int width = 7;
const int stringWidth = 10;

Section::Section(string name, int size){
    this->name = name;
    this->size = size;
}

Section::Section(const Section& s){
    this->name = s.name;
    this->size = s.size;
}

void Section::newSection(string name, int size){
    Section sec(name, size);
    listSection.push_back(sec);
}

Section& Section::getSection(string name){
    std::list<Section>::iterator it;
    for (it = listSection.begin(); it != listSection.end(); ++it){
        if(it->name == name) return *it;
    }
    return *it;
}

void Section::writeTable(){
    std::list<Section>::iterator it;
    cout << "\n Section table\n";
    cout << left << setw(stringWidth) << setfill(separator) << "name";
    cout << left << setw(width) << setfill(separator) << "size";
    cout << endl;
    for (it = listSection.begin(); it != listSection.end(); ++it){
        cout << *it;
    }
}

std::ostream& operator<<(std::ostream& os, const Section& sym){
    os << left << setw(stringWidth) << setfill(separator) << sym.name;
    os << left << setw(width) << setfill(separator) << sym.size;
    os << endl;
    return os;
}
