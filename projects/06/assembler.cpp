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

int RamIdx = 16;

unordered_map<string, int> SymMap = {
    {"SCREEN", 16384},
    {"KBD", 24576},
};

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

string
NoWhiteSpaceStr(string Instr)
{
    char CurrentChar = Instr[0];
    int CharIdx = 1;
    while (CurrentChar == ' ' || CurrentChar == '\t')
    {
        CurrentChar = Instr[CharIdx++];
    }

    return Instr.substr(CharIdx-1, Instr.length() - 2);
}

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
    return Instr.find("@") != string::npos;
}

bool
IsLabel(string Instr)
{
    return Instr.find("(") != string::npos;
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
    int StartIdx = 1;

    if (IsLabel(Instr))
    {
        StartIdx = Instr.find("(") + 1;
        if (Instr.find("\r") != string::npos) return Instr.substr(StartIdx, Instr.length() - 3);

        return Instr.substr(StartIdx, Instr.length() - 2);
    }

    StartIdx = Instr.find("@") + 1;
    if (Instr.find("\r") != string::npos) return Instr.substr(StartIdx, Instr.length() - 2);
    return Instr.substr(StartIdx, Instr.length() - 1);
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
    if (idx != string::npos) return Instr.substr(idx+1, Instr.length()-3);

    idx = Instr.find(";");
    if (idx != string::npos) return Instr.substr(0, idx);

    return "null";
}

string
Jump(string Instr)
{
    int idx = Instr.find(";");
    if (Instr[Instr.length() - 1] == '\n') return Instr.substr(idx+1, Instr.length()-2);

    return Instr.substr(idx+1, Instr.length()-1);
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

string
CInstrToBinary(string Instr)
{
    string Dst = "";
    string Jmp = "";
    string Comparator = Comp(Instr);

    if (IsJump(Instr))
    {
        Dst = "null";
        Jmp = Jump(Instr);
    }
    else
    {
        Dst = Dest(Instr);
        Jmp = "null";
    }

    string CompBinary = CompSymToBinary[Comp(Instr)];
    string DestBinary = DestSymToBinary[Dst];
    string JmpBinary = JumpSymToBinary[Jmp];
    bool A = (Comp(Instr).find("M") != string::npos);

    string Result = "111";
    if (A) Result.append("1");
    else Result.append("0");
    Result.append(CompBinary);
    Result.append(DestBinary);
    Result.append(JmpBinary);

    return Result;
}

bool
IsOnlyDigits(string Instr)
{
    for (int i=0; i < Instr.length(); i++)
    {
        if (!isdigit(Instr[i])) return false;
    }

    return true;
}

int
main(int argc, char *argv[])
{
    string FileName = argv[1];
    ifstream AsmFile(FileName);
    ifstream AsmFile2(FileName);
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
                    cout << CInstrToBinary(Instr) << endl;
                    break;
                case A_INSTRUCTION:
                    if (!IsOnlyDigits(Symbol(Instr)) && !SymMap[Symbol(Instr)])
                        SymMap[Symbol(Instr)] = RamIdx++;
                    break;
                case L_INSTRUCTION:
                    if (!SymMap[Symbol(Instr)])
                        SymMap[Symbol(Instr)] = RamIdx++;
                    break;
            }
        }
    }
    //if (AsmFile2.is_open())
    //{
        //while (AsmFile2)
        //{
            //getline(AsmFile2, Instr);
        //}
    //}
}
