FLAGS=-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Wno-pointer-arith -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

LIBS= ./libs/Stack/*.cpp ./libs/String/*.cpp ./libs/Calc/*.cpp

all: 
	g++ $(FLAGS) $(LIBS) main.cpp -o calc.exe

compiller:
	g++ $(FLAGS) $(LIBS) ./libs/Compiller/*.cpp -o compiller.exe

decompiller:
	g++ $(FLAGS) $(LIBS) ./libs/Decompiller/*.cpp -o decompiller.exe
