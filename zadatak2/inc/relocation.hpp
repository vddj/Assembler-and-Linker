#ifndef REL_H_
#define REL_H_

#include<string>
#include<list>
using namespace std;

class Relocation{
public:
    static std::list<Relocation> listRelocation;
    string section;
    int offset;
    string type;
    string symbol;

    static void writeTable();
    static void writeTableToString(string& outString);
    static void newRelocation(string section, int offs,string type, string symbol);
    friend std::ostream& operator<<(std::ostream& os, const Relocation& sym);

private:
    Relocation(string s, int o,string t, string sim);
};

#endif