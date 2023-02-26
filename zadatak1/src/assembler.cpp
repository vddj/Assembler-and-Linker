#include<regex>
#include<fstream>
#include<sstream>
#include <iomanip>
#include "../inc/assembler.hpp"
#include "../inc/symbol.hpp"
#include "../inc/relocation.hpp"
#include "../inc/section.hpp"
using namespace std;

void Assembler::firstPass(string filename){
    fstream newfile; 
    newfile.open(filename,ios::in);
    if (newfile.is_open()){  
        string oneLine;
        while(getline(newfile, oneLine) && !endOfAssembling){ 
            if(readOneLine(oneLine)) {lineNum++; continue;}
            if(oneLine.substr(0, 1) == ".") directiveFirstPass(oneLine); // direktiva
            else {
                string f = getInstruction(oneLine); // instrukcija
                int size = (f.size() + 1) / 3;
                this->cnt += size;
            }
            lineNum++;
        }
        newfile.close();
        //Symbol::writeTable();
        //Relocation::writeTable();
        //Section::writeTable();
    }
}

void Assembler::secondPass(string filename, string fileOut){
    fstream newfile; 
    this->endOfAssembling = 0;
    this->section = "und";
    this->cnt = 0;
    this->second = 1;
    this->lineNum = 1;
    newfile.open(filename,ios::in);
    if (newfile.is_open()){  
        string oneLine, sectionString = "";
        stringstream outputLines;
        while(getline(newfile, oneLine) && !endOfAssembling){ 
            if(readOneLine(oneLine)) continue;
            string f;
            if(oneLine.substr(0, 1) == ".") f = directiveSecondPass(oneLine); // direktiva
            else f = getInstruction(oneLine); // instrukcija
            
            if(f != "") {
                if(f[0] == '#') {
                    int width = 4;
                    int number = 0;
                    char fill = '0';
                    while(sectionString != ""){
                        outputLines << right << setw(width) << setfill(fill) << std::hex << number << ": ";
                        outputLines << sectionString.substr(0, 8*3) << " " << endl;
                        if(sectionString.size() < 8*3+1) break;
                        sectionString = sectionString.substr(8*3);
                        number += 0x8;
                    }
                    outputLines << f << endl;
                    sectionString = "";
                }
                else {
                    sectionString.append(f + " ");
                    int size = (f.size() + 1) / 3;
                    this->cnt += size;
                }
            }
            lineNum++;
        }
        newfile.close();
        writeOutputFile(fileOut, outputLines.str());
        //Symbol::writeTable();
        //Relocation::writeTable();
        //Section::writeTable();
    }
}

char Assembler::readOneLine(string& oneLine){
    int pozicija = oneLine.find("#");        // eliminacija komentara
    if(pozicija != string::npos) oneLine = oneLine.substr(0, pozicija-1);
    while(oneLine.substr(0, 1) == " ") oneLine = oneLine.substr(1);
    if(oneLine == "" || oneLine == " " || pozicija == 0) return 1;
    return 0;
}

void Assembler::writeOutputFile(string filename, string ss){
    ofstream myfile;
    myfile.open(filename,ios::out);

    string str0;
    Symbol::writeTableToString(str0);
    myfile << str0;

    string str1;
    Relocation::writeTableToString(str1);
    myfile << str1;
    myfile << endl;

    int width = 4;
    int number = 0;
    char fill = '0';

    myfile << ss;
    //cout << ss.str();
    myfile.close();
}

string Assembler::getInstruction(string oneLine){
    string temp = oneLine;
    stringstream s;
    int op = convertInstruction(getFirstWord(oneLine));
    if(op == -1) {
        if (!second) readLabel(oneLine);
        return "";
    }
    if(op >= 0x60 && op < 0xA0) {  // binarne operacije
        int pozicija;
        string registar, reg1, reg2;
        regex b("r[0-7], r[0-7]");
        if(!regex_match(oneLine, b)) {
            if (second) cout << "pogresni argumenti na liniji " << lineNum << endl;
            s << std::hex << op;
            string ss = s.str();
            return ss;
        }
        // citanje dva registra
        registar = getFirstWord(oneLine);
        reg1 = registar.substr(1, 1);

        registar = getFirstWord(oneLine);
        reg2 = registar.substr(1, 1);
        s << std::hex << op << " " << reg1 << reg2;
        string ss = s.str();
        return ss;
    }
    else if(op >= 0x50 && op < 0x60 || op == 0x30) {  // instrukcije skoka
        // citanje operanda
        string f = readOperandJump(oneLine);
        s << std::hex << op << " " << f;
        string ss = s.str();
        return ss;
    }
    else if(op == 0xA0 || op == 0xB0){
        string registar, reg;
        registar = getFirstWord(oneLine); // registar
        reg = registar.substr(1, 1);
        temp = getFirstWord(temp);
        if(temp == "push") {
            s << std::hex << op;
            return s.str() + " " + reg + "6 12";
        }
        if(temp == "pop") {
            s << std::hex << op;
            return s.str() + " " + reg + "6 42";
        }
        oneLine = getFirstWord(oneLine); // operand
        string f = readOperandMov(oneLine);
        f[0] = reg[0];
        s << std::hex << op << " " << f;
        string ss = s.str();
        return ss;
    }
    else if(op == 0x00) return "00";
    s << std::hex << op;
    string ss = s.str();
    return ss;
}

int Assembler::convertInstruction(string s){
    if(s == "halt") return 0x00;
    else if(s == "int") return 0x10;
    else if(s == "iret") return 0x20;
    else if(s == "call") return 0x30;
    else if(s == "ret") return 0x40;
    else if(s == "jmp") return 0x50;
    else if(s == "jeq") return 0x51;
    else if(s == "jne") return 0x52;
    else if(s == "jgt") return 0x53;
    else if(s == "push") return 0xB0;
    else if(s == "pop") return 0xA0;
    else if(s == "xchg") return 0x60;
    else if(s == "add") return 0x70;
    else if(s == "sub") return 0x71;
    else if(s == "mul") return 0x72;
    else if(s == "div") return 0x73;
    else if(s == "cmp") return 0x74;
    else if(s == "not") return 0x80;
    else if(s == "and") return 0x81;
    else if(s == "or") return 0x82;
    else if(s == "xor") return 0x83;
    else if(s == "test") return 0x84;
    else if(s == "shl") return 0x90;
    else if(s == "shr") return 0x91;
    else if(s == "ldr") return 0xA0;
    else if(s == "str") return 0xB0;
    else return -1;
}

string Assembler::directiveSecondPass(string oneLine){     // obrada direktiva
    string s = getFirstWord(oneLine);
    if(s == ".section"){
        // nadji u tabeli sekcija i zapisi velicinu
        string newSection = getFirstWord(oneLine);
        this->section = newSection;
        this->cnt = 0;
        return "#" + newSection;
    }
    else if(s == ".word"){
        char flag = 1;
        string ss;
        while(flag){
            s = getFirstWord(oneLine);
            if(s.substr(s.size()-1) != ",") flag = 0;
            else s = s.substr(0, s.size()-1);
            string novi;
            if(s.substr(0, 2) == "0x") { // literal
                int operand;
                stringstream ss1;
                ss1 << std::hex << s.substr(2);
                ss1 >> operand;
                ss1.clear();
                int bajt2 = operand & 0xFF;
                ss1 << std::hex << std::setfill('0') << std::setw(2) << bajt2;
                if(bajt2 == 0) novi = "00 00";
                else novi = ss1.str() + " 00";
            }
            else {
                if(Symbol::search(s)){  // simbol
                    Symbol symb = Symbol::getSymbol(s);
                    if(symb.size == 504) Relocation::newRelocation(this->section, this->cnt, "abs_data", s);
                    stringstream ss1;
                    int bajt2 = symb.value & 0xFF;
                    ss1 << std::hex << std::setfill('0') << std::setw(2) << bajt2;
                    if(bajt2 == 0) novi = "00 00";
                    else novi = ss1.str() + " 00";
                }
                else{
                    cout << "nerazresen simbol " << s << " na liniji "<< lineNum << endl;
                } 
            }
            ss = ss + novi + " ";
        }
        return ss.substr(0, ss.size()-1);
    }
    else if(s == ".skip"){   // .skip <literal>
        int value;
        s = getFirstWord(oneLine);
        stringstream ss0, ss1;
        ss0 << std::hex << s;
        ss0 >> value;
        for(int i = 0; i < value; i++){
            ss1 << "00";
            if(i != value-1) ss1 << " ";
        }
        return ss1.str();
    }
    else if(s == ".end"){
        endOfAssembling =1;
        return "#end";
    }
    return "";
}

void Assembler::directiveFirstPass(string oneLine){     // obrada direktiva
    string s = getFirstWord(oneLine);
    if(s == ".global" || s == ".extern"){  // .global <lista simbola>
        char flag = 1;
        while(flag){
            string tmp = s;
            s = getFirstWord(oneLine);
            if(s.substr(s.size()-1) != ",") flag = 0;
            else s = s.substr(0, s.size()-1);
            if (tmp == ".global"){
                Symbol::newSymbol(s, this->section, 0, 0, 0); // za sad nije rzresen ni jedan
            }
            else {
                Symbol::newSymbol(s, "und", 0, -1, 0); // ne znamo njenu sekciju
            }
        }
    }
    else if(s == ".section"){
        string newSection = getFirstWord(oneLine);
        if(this->section != "und"){
            Section sec = Section::getSection(this->section);
            sec.size = this->cnt;
        }
        this->section = newSection;
        this->cnt = 0;
        Section::newSection(this->section, 0);
    }
    else if(s == ".equ"){ // .equ <novi simbol>, <literal>
        string sub = getFirstWord(oneLine);
        sub = sub.substr(0, sub.size()-1);
        s = getFirstWord(oneLine);
        int value;   
        stringstream ss;
        ss << std::hex << s.substr(2);
        ss >> value;
        Symbol::newSymbol(sub, "allSec", value, 0, 1);
    }
    else if(s == ".end"){
        endOfAssembling =1;
        if(this->section != "und"){
            Section sec = Section::getSection(this->section);
            sec.size = this->cnt;
        }
    }
    else if(s == ".word"){
        char flag = 1;
        stringstream ss;
        while(flag){
            s = getFirstWord(oneLine);
            if(s.substr(s.size()-1) != ",") flag = 0;
            else s = s.substr(0, s.size()-1);
            this->cnt += 2;
        }
    }
    else if(s == ".skip"){   // .skip <literal>
        int value;
        stringstream ss;
        ss << std::hex << s.substr(2);
        ss >> value;
        this->cnt += value;
    }
}

string Assembler::getFirstWord(string& oneLine){
    int pozicija = oneLine.find(" ");
    string sub = oneLine.substr(0, pozicija);
    oneLine = oneLine.substr(pozicija+1);
    return sub;
}

string Assembler::readOperandJump(string s) {
    regex reg1("0x[0-9a-fA-F]+");   // <literal> == njegova vrednost
    regex reg2("[a-z]\\w+");    // <simbol> - vrednost <simbol> apsolutnim adresiranjem
    regex reg3("\\*0x[0-9a-fA-F]+");    // *<literal> - vrednost iz memorije na adresi <literal>
    regex reg4("%[a-z]\\w+");    // %<simbol> - vrednost <simbol> PC relativnim adresiranjem
    regex reg5("\\*[a-z]\\w+");    // *<simbol> - vrednost iz memorije na adresi <simbol>
    regex reg6("\\*r[0-9]");    // *<reg> - vrednost u registru reg
    regex reg7("\\*\\[r[0-9]\\]");    // *[<reg>] - vrednost iz memorije na adresi reg
    regex reg8("\\*\\[r[0-9] \\+ 0x[0-9]+\\]");    // *[reg + <literal>] - vrednost iz memorije na adresi reg + <literal>
    regex reg9("\\*\\[r[0-9] \\+ [a-z][a-z0-9]\\w+\\]");    // *[reg + <simbol>] - vrednost iz memorije na adresi reg + <simbol>

    return convertOperand(s, 1, reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9);
}

string Assembler::readOperandMov(string s){
    regex reg1("\\$0x[0-9a-fA-F]+");
    regex reg2("\\$[a-z][a-z0-9]\\w+");
    regex reg3("0x[0-9a-fA-F]+");
    regex reg4("[a-z][a-z0-9]\\w+");
    regex reg5("%[a-z]\\w+");
    regex reg6("r[0-9]");    // <reg> - vrednost u registru reg
    regex reg7("\\[r[0-9]\\]");    // [<reg>] - vrednost iz memorije na adresi reg
    regex reg8("\\[r[0-9] \\+ 0x[0-9]+\\]");    // [reg + <literal>] - vrednost iz memorije na adresi reg + <literal>
    regex reg9("\\[r[0-9] \\+ [a-z][a-z0-9]\\w+\\]");    // [reg + <simbol>] - vrednost iz memorije na adresi reg + <simbol>
    return convertOperand(s, 0, reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8, reg9);
}

// treba da se pozabavis ovom funkcijom gde su simboli da li ih ima u tabeli i ako ne onda greska

string Assembler::convertOperand(string s, char mode, regex r1, regex r2, regex r3, regex r4, regex r5, regex r6, regex r7, regex r8,regex r9){
    stringstream str;
    int poz = 0;
    if(regex_match(s, r8)){ // literal
        string rec = getFirstWord(s);
        poz = (mode) ? 3 : 2;
        int operand = stoi(rec.substr(poz, 1));
        rec = getFirstWord(s); // +
        rec = getFirstWord(s);
        rec = rec.substr(0, rec.size()-1);
        int literal = readLiteral(rec.substr(2));
        str << std::hex << "f" << operand << " 02 ";
        return str.str() + setOperand(literal);
    }
    else if(regex_match(s, r9)){
        string rec = getFirstWord(s);
        poz = (mode) ? 3 : 2;
        int operand = stoi(rec.substr(poz, 1));
        rec = getFirstWord(s); // +
        rec = getFirstWord(s);
        rec = rec.substr(0, rec.size()-1);
        if(Symbol::search(rec)){
            Symbol symb = Symbol::getSymbol(rec);
            if(Symbol::isInSection(symb, this->section)){
                string f = setOperand(symb.value);
                str << std::hex << "f" << operand << " 03 ";
                return str.str() + f;
            }
            else if(second) Relocation::newRelocation(this->section, this->cnt+3, "abs_data", rec);
        } 
        else {
            if (second) cout << "nerazresen simbol " << rec << " na liniji "<< lineNum << endl;
        }
        str << std::hex << "f" << operand << " 03 00 00";
        return str.str();
    }
    else if(regex_match(s, r6)){
        poz = (mode) ? 2 : 1;
        int operand = stoi(s.substr(poz, 1));
        str << std::hex << "f" << operand << " 01";
        return str.str();
    }
    else if(regex_match(s, r7)){
        poz = (mode) ? 3 : 2;
        int operand = stoi(s.substr(poz, 1));
        str << std::hex << "f" << operand << " 02 ";
        return str.str();
    }
    else if(regex_match(s, r1) || regex_match(s, r2)){
        if(s.substr(0,2) == "0x" || s.substr(0,3) == "$0x"){  // literal
            poz = (mode) ? 2 : 3;
            int operand = readLiteral(s.substr(poz));
            if(mode) return "f0 00 " + setOperand(operand); 
            return "00 00 " + setOperand(operand); 
        }
        else {                            // simbol
            if(!mode) s = s.substr(1);
            if(Symbol::search(s)){
                Symbol symb = Symbol::getSymbol(s);
                if(Symbol::isInSection(symb, this->section)){
                    string f = setOperand(symb.value);
                    if(mode) str << std::hex << "f0 00 ";
                    else str << std::hex << "00 00 ";
                    if(second && symb.size == 2) 
                        Relocation::newRelocation(this->section, this->cnt+3, "abs_data", s);
                    return str.str() + f;
                }
                else {
                    if(second) Relocation::newRelocation(this->section, this->cnt+3, "abs_data", s);
                    if(mode) return "f0 00 00 00"; 
                    return "00 00 00 00";
                }
            }
            else {
                if (second) cout << "nerazresen simbol " << s << " na liniji "<< lineNum <<endl;
                if(mode) return "f0 00 00 00"; 
                return "00 00 00 00";
            }
        }
    }
    else if(regex_match(s, r3)){  // literal
        poz = (mode) ? 1 : 0;
        int operand = readLiteral(s.substr(poz));
        if(mode) return "f0 04 " + setOperand(operand); 
        return "00 04 " + setOperand(operand);
    }
    else if(regex_match(s, r4)){
        if(mode) {   // jump // %<simbol> - vrednost <simbol> PC relativnim adresiranjem
            s = s.substr(1);
            if(Symbol::search(s)){
                Symbol symb = Symbol::getSymbol(s);
                if(symb.section == this->section){
                    string f = setOperand(symb.value);
                    str << std::hex << "00 00 ";
                    return str.str() + f;
                }
                else {
                    if(second) Relocation::newRelocation(this->section, this->cnt+3, "pc_rel", s);
                    return "f0 00 00 00";
                }
            }
            else { 
                if (second) cout << "nerazresen simbol " << s << " na liniji "<< lineNum <<endl;
                return "f0 00 00 00";
            }
        }
        else {   // mov vrednost iz memorije na adresi simbol
            if(Symbol::search(s)){
                Symbol symb = Symbol::getSymbol(s);
                if(Symbol::isInSection(symb, this->section)){
                    string f = setOperand(symb.value);
                    str << std::hex << "00 04 ";
                    return str.str() + f;
                }
                else if(second) Relocation::newRelocation(this->section, this->cnt+3, "memory", s);
            }
            else {
                if (second) cout << "nerazresen simbol " << s << " na liniji "<< lineNum <<endl;
            }
            return "00 04 00 00";
        }
        //int operand = stoi(s.substr(3));
        //str << std::hex << "ff 04 ";
        //return str.str() + setOperand(operand);
    }
    else if(regex_match(s, r5)){
        string pom;
        if(mode) pom = "mem"; else pom = "pc_rel";
        s = s.substr(1);
        if(Symbol::search(s)){
            Symbol symb = Symbol::getSymbol(s);
            if(Symbol::isInSection(symb, this->section)){
                string f = setOperand(symb.value);
                str << std::hex << "00 04 ";
                return str.str() + f;
            }
            else if(second) Relocation::newRelocation(this->section, this->cnt+3, pom, s);
        }
        else {
            if (second) cout << "nerazresen simbol " << s << " na liniji "<< lineNum <<endl;
        }
        if(mode) return "f0 03 00 00"; 
        return "07 03 00 00";
    }
    if(second) cout << "instrukcija neispravna na liniji " << lineNum << endl;
    return "";
}

void Assembler::readLabel(string line){
    regex b("[a-z]\\w+:"); 
    if (regex_match(line, b)){
        Symbol::newSymbol(line.substr(0, line.size()-1), this->section, this->cnt, 504, 1);
    }
    else cout << "pogresna instrukcija na liniji " << lineNum << endl;
}

string Assembler::setOperand(int operand){
    stringstream s;
    int bajt1 = operand>>8;
    int bajt2 = operand & 0xFF;
    s << std::hex << std::setfill('0') << std::setw(2) << bajt1 << " ";
    s << std::hex << std::setfill('0') << std::setw(2) << bajt2;
    return s.str();
}

int Assembler::readLiteral(string literal){
    stringstream str;
    str << std::hex << literal;
    int operand;
    str >> operand;
    if(operand > 0xFFFF){
        if(second) cout << "operand preveliki na liniji " << lineNum << endl;
        operand = operand & 0xFFFF;
    }
    return operand;
}