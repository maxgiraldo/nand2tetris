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
    {"MD", "011"},
    {"A", "100"},
    {"AM", "101"},
    {"MA", "101"},
    {"AD", "110"},
    {"DA", "110"},
    {"ADM", "111"},
    {"AMD", "111"},
    {"DAM", "111"},
    {"DMA", "111"},
    {"MDA", "111"},
    {"MAD", "111"},
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
    Instr.erase(remove_if(Instr.begin(), Instr.end(), ::isspace), Instr.end());
    int CommentIdx = Instr.find("/");
    if (CommentIdx != string::npos)
        return Instr.substr(0, CommentIdx);
    else
        return Instr;
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
    int LastCharIdx = Instr.length() - 1;
    int StartIdx = 1;

    if (IsLabel(Instr))
    {
        StartIdx = Instr.find("(") + 1;
        return Instr.substr(StartIdx, Instr.length() - 2);
    }

    StartIdx = Instr.find("@") + 1;
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
    if (idx != string::npos) return Instr.substr(idx+1, Instr.length()-1);

    idx = Instr.find(";");
    if (idx != string::npos) return Instr.substr(0, idx);

    return "null";
}

string
Jump(string Instr)
{
    int idx = Instr.find(";");

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
    ofstream HackFile;
    string OutputFileName = "";
    OutputFileName.append(FileName.substr(0, FileName.length()-4));
    OutputFileName.append(".hack");
    HackFile.open(OutputFileName);
    string Instr;
    string NoSpaceInstr;
    int RamIdx = 16;
    int LineNum = -1;

    unordered_map<string, int> SymTable = {
        {"R0", 0},
        {"R1", 1},
        {"R2", 2},
        {"R3", 3},
        {"R4", 4},
        {"R5", 5},
        {"R6", 6},
        {"R7", 7},
        {"R8", 8},
        {"R9", 9},
        {"R10", 10},
        {"R11", 11},
        {"R12", 21},
        {"R13", 13},
        {"R14", 14},
        {"R15", 15},
        {"SP", 0},
        {"LCL", 1},
        {"ARG", 2},
        {"THIS", 3},
        {"THAT", 4},
        {"SCREEN", 16384},
        {"KBD", 24576},
    };

    if (AsmFile.is_open())
    {
        while (AsmFile)
        {
            getline(AsmFile, Instr);
            if (IsComment(Instr)) continue;
            if (IsEmptyLine(Instr)) continue;

            NoSpaceInstr = NoWhiteSpaceStr(Instr);
            switch (InstructionType(NoSpaceInstr)) {
                case C_INSTRUCTION:
                    LineNum++;
                    break;
                case A_INSTRUCTION:
                    LineNum++;
                    break;
                case L_INSTRUCTION:
                    if (!SymTable[Symbol(NoSpaceInstr)])
                        SymTable[Symbol(NoSpaceInstr)] = LineNum + 1;
                    break;
            }
        }
    }
    if (AsmFile2.is_open())
    {
        while (AsmFile2)
        {
            getline(AsmFile2, Instr);
            if (IsComment(Instr)) continue;
            if (IsEmptyLine(Instr)) continue;

            NoSpaceInstr = NoWhiteSpaceStr(Instr);
            switch (InstructionType(NoSpaceInstr))
            {
                case C_INSTRUCTION:
                    HackFile << CInstrToBinary(NoSpaceInstr) << endl;
                    break;
                case A_INSTRUCTION:
                    if (!IsOnlyDigits(Symbol(NoSpaceInstr)) && !SymTable[Symbol(NoSpaceInstr)])
                    {
                        SymTable[Symbol(NoSpaceInstr)] = RamIdx++;
                        HackFile << IntToBinary(SymTable[Symbol(NoSpaceInstr)]) << endl;
                    }
                    else if (SymTable[Symbol(NoSpaceInstr)])
                    {
                        HackFile << IntToBinary(SymTable[Symbol(NoSpaceInstr)]) << endl;
                    }
                    else
                    {
                        HackFile << IntToBinary(stoi(Symbol(NoSpaceInstr))) << endl;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    HackFile.close();
}
