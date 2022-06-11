clang++ -std=c++11 assembler.cpp -o AssemblerBasic
./AssemblerBasic add/Add.asm
./AssemblerBasic max/Max.asm
./AssemblerBasic max/MaxL.asm
./AssemblerBasic pong/Pong.asm
./AssemblerBasic pong/PongL.asm
./AssemblerBasic rect/Rect.asm
./AssemblerBasic rect/RectL.asm
mv add/Add.hack submission/Add.hack
mv max/Max.hack submission/Max.hack
mv max/MaxL.hack submission/MaxL.hack
mv pong/Pong.hack submission/Pong.hack
mv pong/PongL.hack submission/PongL.hack
mv rect/RectL.hack submission/RectL.hack
mv rect/Rect.hack submission/Rect.hack
