#include <time.h>
#include <sys/stat.h>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <fstream>
#include <fcntl.h>
#include "Timer.h"

#define MODE(z) ((statbuf.st_mode & (z)) == (z))

using namespace std;

//.........................................
//these two function is for testing
void testString(char * test);
void testArgv(char * test[]);
//........................................

//get commands from the userinput.
void help();

//user input a whole string, terminate untill enter
char * inputCommand();

//organize spaces that only have one space between two argument.
char * orgSpaces(char * cmd);

//pass commands in temp to argvNew spearate by space.
void parsingArgv(char * temp, char ** argvTemp);

//using std::istream::get and std::ostream::put mehtod to implement cp
void getput(char ** argv);

//using read and write one character at a time, to copy data to another
void readWriteOne(char ** argv);

//using read and write by passing buffer copy data
void readWriteBuf(char ** argv);

// clean memory
void freeArray(char ** array);

int main()
{
	help();
	return 0;
}

void help()
{
	char * cmdString = new char[100];
	char * argv[50];
	cmdString = inputCommand();
	cmdString = orgSpaces(cmdString);
	parsingArgv(cmdString, argv);
	
	if(strcmp(argv[0],"cp") != 0 && argv[5] != NULL)
	{
		cerr << "wrong command " << argv[0] << endl;
		freeArray(argv);
		help();
	}
	else if(argv[1] == NULL || argv[2] == NULL) 
			
	{
		cerr << "missing arguments" << endl;
		help();
	}

	Timer t;
	double eTime;
	//using fstream method 	
//	t.start();
//	getput(argv);
//	t.elapsedUserTime(eTime);
//	cout << eTime << endl;

	//using read and write one character at a time
	t.start();
	readWriteBuf(argv);
	t.elapsedUserTime(eTime);
	cout << eTime << endl;

	//using read and write by buffer 
//	t.start();
//	readWriteBuf(argv);
//	t.elapsedUserTime(eTime);
//	cout << eTime << endl;

//    



		
}

char * inputCommand()
{
	int i = 0;
	char *temp= new char[50];
	memset(temp,'\0',50);
	char c = ' ';
	cout << "[rShell_] $";
	while(c != '\0')
	{
		c = getchar();
		if(c == '\n')
		{
	        	if(temp[0] == '\0')
 				cout << "[rShell_] $";
			else
			{
                                strncat(temp, "\0", 1);
				break;
	                }
		}
		else
		{
			strncat(temp, &c, 1);
		}
		i++;
	}
		return temp;
}


char * orgSpaces(char * temp)
{
	char * cmd = new char[100];
	memset(cmd, '\0',100);
	int i = 0;
	while(temp[i] != '\0'){
		if(temp[i] == ' '){
			while(temp[i+1] == ' '){
				if(temp[i+1] == '\0')
					break;
				else
					i++;
			}
		}
		strncat(cmd, &temp[i],1);
		i++;
	}
	strncat(cmd, "\0", 1);

	delete temp;
	temp = NULL;
	return cmd;
}



void parsingArgv(char * temp, char ** argvTemp)
{
	int index = 0;
	char *tok;
	tok = strtok(temp, " ");
	while(tok != NULL && (strcmp(tok, "#") != 0))
	{
		argvTemp[index] = new char[strlen(tok) + 1];
		strcpy(argvTemp[index], tok);
		strncat(argvTemp[index], "\0", 1);
		index++;
		tok = strtok(NULL, " ");
	}
	argvTemp[index] = NULL;
}

void getput(char ** argv)
{
	ifstream infile(argv[1]);
	ofstream outfile(argv[2]);
	char c;
	while(infile.get(c))
	{
		outfile.put(c);
	}
	
	infile.close();
	outfile.close();
		
}

void readWriteOne(char ** argv)
{

	int fdi = open(argv[1], O_RDWR | O_APPEND);
	char c[1];
	memset(c,'\0',1);
	int offset = -11;
	int fdo = open(argv[2], O_APPEND | O_RDWR);

	while(offset != 0)
	{
		offset = read(fdi, c, 1);
		if(offset == -1)
		{
			perror("read");
			exit(1);
		}	
		
		if(write(fdo, c, 1) == -1)
		{
			perror("write");
			exit(1);
		}
	}
	
	if(close(fdi) == -1)
	{
		perror("close");
		exit(1);
	}
	
	if(close(fdo) == -1)
	{
		perror("close");
		exit(1);
	}
}

void readWriteBuf(char ** argv)
{

	int fdi = open(argv[1], O_RDWR | O_APPEND);
	char c[BUFSIZ];
	memset(c,'\0',1);
	int offset = -11;
	int fdo = open(argv[2], O_APPEND | O_RDWR);

	while(offset != 0)
	{
		offset = read(fdi, c, BUFSIZ);
		if(offset == -1)
		{
			perror("read");
			exit(1);
		}	
		
		if(write(fdo, c, BUFSIZ) == -1)
		{
			perror("write");
			exit(1);
		}
	}
	
	if(close(fdi) == -1)
	{
		perror("close");
		exit(1);
	}
	
	if(close(fdo) == -1)
	{
		perror("close");
		exit(1);
	}
}

void freeArray(char ** array)
{
        int i = 0;
        while(array[i] != NULL)
        {
                delete [] array[i];
                array[i] = NULL;
                i++;
        }
}


//......................................................
void testString(char * test)
{
	cout << "stirng length = " << strlen(test) << endl;
	cout << "string = " << test << endl;
}

void testArgv(char * test[])
{
	cout << "testArgv......................" << endl;
	int i = 0;
	while(test[i] != NULL){
		cout << i << " = " << test[i] << endl;
		i++;
	}
}
//.......................................................


