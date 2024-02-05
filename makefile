all: clean injector.exe injected.dll victim.exe

injector.exe: injector.cpp
	g++ injector.cpp -o injector.exe

victim.exe: victim.cpp
	g++ victim.cpp -o victim.exe

injected-code.o: 
	g++ -c injected-code.cpp

injected.dll: injected-code.o
	g++ -shared -o injected.dll injected-code.o

clean: 
	rm -f injected-code.o injected.dll injector.exe victim.exe