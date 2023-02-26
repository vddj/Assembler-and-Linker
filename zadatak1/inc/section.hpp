#ifndef SECT_H_
#define SECT_H_

#include<string>
#include<list>
#include<sstream>
using namespace std;

class Section{
public:
    static std::list<Section> listSection;
    string name;
    int size;
    string data;  
    
    Section(const Section&);
    static void newSection(string name, int size);
    static void writeTable();
    static Section& getSection(string name);
    friend std::ostream& operator<<(std::ostream& os, const Section& sym);

private:
    Section(string name, int size);
};

#endif