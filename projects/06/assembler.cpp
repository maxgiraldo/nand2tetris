#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

enum instr_type { A_INSTRUCTION, C_INSTRUCTION, L_INSTRUCTION };

// A-instruction
// 0vvvvvvvvvvvvvvv
//
// C-instruction
// 111accccccdddjjj

unordered_map<string, string> CompSymToBinary =
{
    {"0", "101010"},
    {"1", "111111"},
    {"-1", "111010"},
    {"D", "001100"},
    {"A", "110000"},
    {"M", "110000"},
    {"!D", "001101"},
    {"!A", "110001"},
    {"!M", "110001"},
    {"-D", "001111"},
    {"-A", "110011"},
    {"-M", "110011"},
    {"D+1", "011111"},
    {"A+1", "110111"},
    {"M+1", "110111"},
    {"D-1", "001110"},
    {"A-1", "110010"},
    {"M-1", "110010"},
    {"D+A", "000010"},
    {"D+M", "000010"},
    {"D-A", "010011"},
    {"D-M", "010011"},
    {"A-D", "000111"},
    {"M-D", "000111"},
    {"D&A", "000000"},
    {"D&M", "000000"},
    {"D|A", "010101"},
    {"D|M", "010101"},
};

unordered_map<string, string> DestSymToBinary =
{
    {"null", "000"},
    {"M", "001"},
    {"D", "010"},
    {"DM", "011"},
    {"A", "100"},
    {"AM", "101"},
    {"AD", "110"},
    {"ADM", "111"},
};

unordered_map<string, string> JumpSymToBinary =
{
    {"null", "000"},
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"},
};

// Instr types

bool
IsComment(string Instr)
{
    return Instr.substr(0,2) == "//";
}

bool
IsEmptyLine(string Instr)
{
    return Instr == "\r" || Instr == "\0";
}

bool
IsAddress(string Instr)
{
    return Instr.substr(0,1) == "@";
}

bool
IsLabel(string Instr)
{
    return Instr.substr(0,1) == "(";
}

bool
IsJump(string Instr)
{
    return Instr.find(";") != string::npos;
}

bool
IsComp(string Instr)
{
    return Instr.find("=") != string::npos;
}

instr_type
InstructionType(string Instr)
{
    if (IsAddress(Instr)) return A_INSTRUCTION;
    if (IsLabel(Instr)) return L_INSTRUCTION;

    return C_INSTRUCTION;
}

// Get values

string
Symbol(string Instr)
{
    int LastCharIdx = Instr.length() - 3;
    return Instr.substr(1, LastCharIdx);
}

string
Address(string Instr)
{
    int LastIdx = Instr.length() - 2;
    return Instr.substr(1, LastIdx);
}

string
Dest(string Instr)
{
    int idx = Instr.find("=");
    return Instr.substr(0, idx);
}

string
Comp(string Instr)
{
    int idx = Instr.find("=");
    return Instr.substr(idx+1, Instr.length()-2);
}

string
Jump(string Instr)
{
    int idx = Instr.find(";");
    return Instr.substr(idx+1, Instr.length()-2);
}

// Conversion

string
IntToBinary(int Decimal)
{
    vector<int> BinaryNum(16);
    int Idx;

    for (Idx=0; Decimal>0; Idx++)
    {
        BinaryNum[Idx] = Decimal % 2;
        Decimal = Decimal / 2;
    }

    string Binary = "";
    for (int j = 15; j >= 0; j--)
        Binary.append(to_string(BinaryNum[j]));

    return Binary;
}

//string
//AddressToBinary(string Instr)
//{
    //return IntToBinary(stoi(AddressSymbol(Instr)));
//}
//
//

string
CInstrToBinary(string Instr)
{
    inline
    string
    CompToBinary(string Instr) return CompSymToBinary[Comp(Instr)];

    inline
    string
    DestToBinary(string Instr) return DestSymToBinary[Dest(Instr)];

    inline
    string
    JumpToBinary(string Instr) return JumpSymToBinary[Jump(Instr)];

    return CompToBinary(Instr
}

int
main(int argc, char *argv[])
{
    auto FileName = argv[1];
    ifstream AsmFile(FileName);
    string Instr;

    if (AsmFile.is_open())
    {
        while (AsmFile)
        {
            getline(AsmFile, Instr);
            if (IsComment(Instr)) continue;
            if (IsEmptyLine(Instr)) continue;
            switch (InstructionType(Instr)) {
                case C_INSTRUCTION:
                    if (IsComp(Instr)) {
                        cout << "DEST: " << DestToBinary(Instr) << endl;
                        cout << "COMP: " << CompToBinary(Instr) << endl;
                    }
                    if (IsJump(Instr)) cout << "JMP: " << JumpToBinary(Instr) << endl;
                    break;
                case A_INSTRUCTION:
                    cout << 
                    break;
                case L_INSTRUCTION:
                    break;
            }
        }
    }
}
