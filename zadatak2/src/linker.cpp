#include<fstream>
#include<sstream>
#include<iomanip>
#include<iostream>
#include "../inc/relocation.hpp"
#include "../inc/symbol.hpp"
#include "../inc/linker.hpp"
using namespace std;

std::list<Sekcija> Linker::lista;
std::list<string> Linker::datoteke;

void Linker::readFile(string filename){
    std::list<string>::iterator it1;
    for (it1 = datoteke.begin(); it1 != datoteke.end(); ++it1){
        newfile.open(*it1,ios::in);
        readTables(*it1);
        readCode(*it1);
        newfile.close();
        endOfFile = 0;   
    }
    raspodelaSekcija();
    razresiSimbole();
    std::list<Sekcija>::iterator it;
    for (it = lista.begin(); it != lista.end(); ++it){
        ss << it->podaci << " ";
    }
    writeFile(filename, ss.str());
    //Relocation::writeTable();
}

void Linker::raspodelaSekcija(){
    int trenutna = 0;
    std::list<Sekcija>::iterator it;
    lista.sort( []( const Sekcija &a, const Sekcija &b ) 
        { return a.mesto < b.mesto; } );
    for (it = lista.begin(); it != lista.end(); ++it){
        if(it->naziv == "pomocna") break;
        if(it->mesto > 1000) it->mesto =trenutna;
        if(trenutna > it->mesto){
            cout << "adrese se poklapaju " << it->naziv << " " << trenutna << endl;
            return;
        }
        if(trenutna < it->mesto){
            string t = newSekcija(it->mesto-trenutna);
            newSekcija(trenutna, "pomocna", t);
        }
        trenutna = it->mesto + it->velicina;
    }
    
    lista.sort( []( const Sekcija &a, const Sekcija &b ) 
        { return a.mesto < b.mesto; } );
}

void Linker::readCode(string filename){
    if (newfile.is_open()){  
        string oneLine;
        this->cnt = 0;
        ss1.str("");
        currSection = "";
        while(getline(newfile, oneLine) && !endOfFile){ 
            int pozicija = oneLine.find("#");  
            if(pozicija != string::npos) readSection(oneLine.substr(1));
            else {
                oneLine = oneLine.substr(6, oneLine.size()-8);
                int size = (oneLine.size() + 1) / 3;
                this->cnt += size;
                //ss << oneLine << " ";
                ss1 << oneLine << " ";
            }
        }
    }
}

void Linker::readSection(string s){
    if(s == "end") endOfFile = 1;
    if(currSection != ""){
        if(search(currSection)) addText(currSection, ss1.str());
        else newSekcija(velikiBroj++, currSection, ss1.str());
    }
    currSection = s;
    ss1.str("");
    this->cnt = 0;
}

void Linker::readTables(string filename){
    if (newfile.is_open()){  
        string oneLine;
        getline(newfile, oneLine); // prazan red 
        getline(newfile, oneLine); // Symbol table
        getline(newfile, oneLine); // ID     name      section   value  resolved
        // Symbol table
        while(getline(newfile, oneLine)){ 
            if(oneLine == "") break;
            readSymbol(oneLine);
        }
        getline(newfile, oneLine); // Relocation table
        getline(newfile, oneLine); // section   offset type      symbol 
        // Relocation table
        while(getline(newfile, oneLine)){ 
            if(oneLine == "") break;
            readRelocation(oneLine);
        }
    }
}

string Linker::getFirstWord(string& oneLine){
    int pozicija = oneLine.find(" ");
    string sub = oneLine.substr(0, pozicija);
    oneLine = oneLine.substr(pozicija+1);
    while(oneLine.substr(0, 1) == " ") oneLine = oneLine.substr(1);
    //cout << "first word: " << sub << endl;
    return sub;
}

void Linker::readRelocation(string oneLine){
    string section, symbol, type;
    int offset;
    section = getFirstWord(oneLine);// section
    stringstream ss1;
    string str = getFirstWord(oneLine);
    ss1 << std::hex << str;//offset
    ss1 >> offset;
    type = getFirstWord(oneLine);//type
    symbol = getFirstWord(oneLine);// symbol
    Relocation::newRelocation(section, offset, type, symbol);
}

void Linker::readSymbol(string oneLine){
    string name, section, size;
    int value;
    char resolved;
    string s = getFirstWord(oneLine);
    name = getFirstWord(oneLine);
    section = getFirstWord(oneLine);
    stringstream sss;
    string sop = getFirstWord(oneLine);
    sss << std::hex << sop;
    sss >> value;
    resolved = stoi(getFirstWord(oneLine));
    size = getFirstWord(oneLine);
    Symbol::newSymbol(name, section, value, size, resolved);
}

void Linker::razresiSimbole(){
// prepravi labele iz tabele simbola
    std::list<Symbol>::iterator it;
    for (it = Symbol::listSymbols.begin(); it != Symbol::listSymbols.end(); ++it){
        if(it->size == "label"){
            std::list<Sekcija>::iterator it1;
            for (it1 = lista.begin(); it1 != lista.end(); ++it1){
                if(it1->naziv == it->section){
                    it->value = it->value + it1->mesto;
                    break;
                }
            }
        }
    }
// razresi relokacionu tabelu
    std::list<Relocation>::iterator it1;
    for (it1 = Relocation::listRelocation.begin(); it1 != Relocation::listRelocation.end(); ++it1){  
        std::list<Sekcija>::iterator it2;
        for (it2 = lista.begin(); it2 != lista.end(); ++it2){
            if(it2->naziv == it1->section){
                //cout << it2->naziv << " " << it1->section << endl;
                std::list<Symbol>::iterator it3;
                for(it3 = Symbol::listSymbols.begin(); it3 != Symbol::listSymbols.end(); ++it3){
                    if(it3->name == it1->symbol){
                        string vr;
                        if(it1->type == "memory" || it1->type == "abs_data")
                            vr = setOperand(it3->value);
                        else vr = setOperand(it3->value-it1->offset-2);
                        
                        if(it1->section == "ivt"){
                            it2->podaci[it1->offset*3+0] = vr[3];
                            it2->podaci[it1->offset*3+1] = vr[4];
                            it2->podaci[it1->offset*3+2] = vr[2];
                            it2->podaci[it1->offset*3+3] = vr[0];
                            it2->podaci[it1->offset*3+4] = vr[1];
                        }
                        else {
                            it2->podaci[it1->offset*3+0] = vr[0];
                            it2->podaci[it1->offset*3+1] = vr[1];
                            it2->podaci[it1->offset*3+2] = vr[2];
                            it2->podaci[it1->offset*3+3] = vr[3];
                            it2->podaci[it1->offset*3+4] = vr[4];
                        }
                        
                        break;
                    }
                }
                break;
            }
        }
        
    }
}

void Linker::writeFile(string filename, string s){
    ofstream myfile;
    myfile.open(filename,ios::out);
    /*string pom0;
    Symbol::writeTableToString(pom0);
    myfile << pom0;
    string pom;
    Relocation::writeTableToString(pom);
    myfile << pom << endl;*/
    int width = 4;
    int number = 0;
    char fill = '0';
    stringstream ss;
    while(s != ""){
        ss << right << setw(width) << setfill(fill) << std::hex << number << ":";
        ss << " " << s.substr(0, 8*3) << endl;
        if(s.size() < 8*3+1) break;
        s = s.substr(8*3);
        number += 0x8;
    }
    myfile << ss.str();
    myfile.close();
}

string Linker::setOperand(int operand){
    stringstream s;
    int bajt1 = operand>>8;
    int bajt2 = operand & 0xFF;
    s << std::hex << std::setfill('0') << std::setw(2) << bajt1 << " ";
    s << std::hex << std::setfill('0') << std::setw(2) << bajt2;
    return s.str();
}

string Linker::newSekcija(int velicina){
    string s;
    for(int i = 0; i < velicina; i++){
        s = s + "00";
        if(i != velicina-1) s = s + " ";
    }
    return s;
}

void Linker::newSekcija(int mesto, string naziv){
    Sekcija sek;
    sek.mesto = mesto;
    sek.naziv = naziv;
    Linker::lista.push_back(sek);
}

void Linker::newSekcija(int mesto, string naziv, string text){
    if(text.substr(text.size()-1) == " ") text = text.substr(0, text.size()-1);
    Sekcija sek;
    sek.mesto = mesto;
    sek.naziv = naziv;
    sek.podaci = text;
    sek.velicina = (text.size() + 1 ) / 3;
    Linker::lista.push_back(sek);
}

void Linker::ispis(){
    std::list<Sekcija>::iterator it;
    for (it = lista.begin(); it != lista.end(); ++it){
        cout << it->naziv << " mesto " << it->mesto << " velicina " << it->velicina << endl;
        cout << it->podaci.substr(0, 20) << endl;
    }

    std::list<string>::iterator it1;
    for (it1 = datoteke.begin(); it1 != datoteke.end(); ++it1){
        cout << *it1 << endl;
    }
}

char Linker::search(string name){
    std::list<Sekcija>::iterator it;
    for (it = lista.begin(); it != lista.end(); ++it){
        if(it->naziv == name) return 1;
    }
    return 0;
}

void Linker::addText(string name, string text){
    std::list<Sekcija>::iterator it;
    for (it = lista.begin(); it != lista.end(); ++it){
        if(it->naziv == name) {
            if(text.substr(text.size()-1) == " ") text = text.substr(0, text.size()-1);
            it->podaci = text;
            it->velicina = (text.size() + 1 ) / 3;
            return;
        }
    }
}

void Linker::readFileLinkable(string filename){
    std::list<string>::iterator it1;
    ss1.str("");
    for (it1 = datoteke.begin(); it1 != datoteke.end(); ++it1){
        newfile.open(*it1,ios::in);
        readTables(*it1);
        readCodeLinkable(*it1);
        newfile.close();
        endOfFile = 0;   
    }
    writeFileLinkable(filename, ss1.str());
    //Relocation::writeTable();
}

void Linker::readCodeLinkable(string filename){
    if (newfile.is_open()){  
            string oneLine;
            while(getline(newfile, oneLine) && !endOfFile){
                if(oneLine == "#end") break;
                ss1 << oneLine << endl;
            }
        }
}

void Linker::writeFileLinkable(string filename, string s){
    ofstream myfile;
    myfile.open(filename,ios::out);
    string pom0;
    Symbol::writeTableToString(pom0);
    myfile << pom0;
    string pom;
    Relocation::writeTableToString(pom);
    myfile << pom << endl;
    myfile << s << "#end";
    myfile.close();
}