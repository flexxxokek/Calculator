FLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Wno-pointer-arith -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

LIBS= ./libs/Calc/calc.cpp ./libs/Io/io.cpp ./libs/Data/data.cpp -Llibs/Stack -lStack

all: 
	g++ $(FLAGS) $(LIBS) ./src/main.cpp -o calc.exe

assembler:
	g++ $(FLAGS) $(LIBS) ./libs/String/*.cpp ./libs/Assembler/*.cpp -o assembler.exe

disassembler:
	g++ $(FLAGS) $(LIBS) ./libs/String/*.cpp ./libs/Disassembler/*.cpp -o disassembler.exe
