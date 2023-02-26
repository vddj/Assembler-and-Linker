#include<iostream>
#include<regex>
#include "../inc/linker.hpp"
#include<list>
using namespace std;

int main(int argc, char *argv[]){
    string filename;
    if(argc < 5) {
        cout << "pogresan broj parametara" << endl;
        return 0;
    }
    string param = argv[1];
    if(param.compare("-hex") != 0 && param.compare("-linkable") != 0) {
        cout << "pogresan parametar" << endl;
        return 0;
    }
    int i = 2;
    int uslov = 0;
    //cout << " broj " << param << endl;
    regex reg("-place=[a-zA-Z0-9]+@0x[0-9A-Za-z]+");
    while(i < argc){
        string s = argv[i];
        //cout << " broj " << i << " " << s << endl;
        if(s.compare("-o") == 0){
            filename = argv[++i];
            uslov = 1;
            i++;
        }
        if(uslov){
            Linker::datoteke.push_back(argv[i]);
            i++;
        }
        else{
            if(regex_match(s, reg)){
                string naziv;
                int mesto;
                //cout << " regex ok " << endl;
                int pozicija = s.find("@");
                naziv = s.substr(7, pozicija-7);
                stringstream w;
                pozicija = s.find("x");
                w << std::hex << s.substr(pozicija+1);
                w >> mesto; 
                Linker::newSekcija(mesto, naziv);
                i++;
            }
            else {
                cout << "pogresan argument broj " << i+1 << endl;
                return 0;
            }
        }
    }

    if(param.compare("-hex") == 0) {
        Linker l;
        l.readFile(filename);
    }
    else if(param.compare("-linkable") == 0) {
        Linker l;
        l.readFileLinkable(filename);
    }
    return 0;
}

// g++ -o linker ./src/*.cpp

// ./linker -hex -place=myData@0x3e -place=isr@0x0 -o ime1.hex tests/izlaz1.o tests/izlaz.o
// ./linker -hex -place=ivt@0x0 -place=myCode@0x50 -place=isr@0x10 -place=myData@0x70 -o ime1.hex tests/izlaz1.o tests/izlaz.o