all: rshell

rshell: bin rshell.o ls.o
	
bin:
	mkdir bin

rshell.o: 
	g++ -Wall -Werror -ansi -pedantic ./src/rshell.cpp -o ./bin/rshell 

ls.o:
	g++ -Wall -Werror -ansi -pedantic ./src/ls.cpp -o ./bin/ls 

clean: 
	rm -rf ./bin
