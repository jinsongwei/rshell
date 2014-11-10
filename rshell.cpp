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

using namespace std;

//.........................................
//these two function is for testing
void testString(char * test);
void testArgv(char * test[]);
//........................................

void help();

//user input a whole string, terminate untill enter
char * inputCommand();

//check if command has ;,||,and && symbols. if has separate them.
char * orgSymbol(char *temp);

//organize spaces that only have one space between two argument.
char * orgSpaces(char * cmd);

//pass commands in temp to argvNew spearate by space.
void parsingArgv(char * temp, char ** argvTemp);

int main()
{
          help();
          return 0;
}
 
void help()
{
	char * cmdString;
        char * argvNew[50];
	cmdString = inputCommand();
	cmdString = orgSymbol(cmdString);
        cmdString = orgSpaces(cmdString);
        parsingArgv(cmdString, argvNew);
			
	testArgv(argvNew);
}

char * inputCommand()
{
	char *temp = new char[50];
	char c;
	cout << "[rShell_ls] $";
	while(c != EOF)
	{
		c = getchar();
		if(c == '\n')
		{
	        	if(temp[0] == '\0')
 				cout << "[rShell_ls] $";
			else
			{
                                strncat(temp, "\0", 1);
				break;
	                }
		}
		else
		strncat(temp, &c, 1);
	}
		return temp;
}

char * orgSymbol(char *temp)
{
	char *newString = new char[100];
	int i = 0;
	while(temp[i] != '\0')
	{
		if(temp[i] == ';')
			strncat(newString, " ; ", 3);
		else if(temp[i] == '&' && temp[i+1] == '&')
		{
			strncat(newString," && ", 4);
			i++;
		}
		else if(temp[i] == '|' && temp[i+1] == '|')
		{
			strncat(newString, " || ", 4);
			i++;
		}
		else 
			strncat(newString, &temp[i],1);
		i++;
	}
	strncat(newString,"\0",1);
	
	return newString;
}
char * orgSpaces(char * temp)
{
	char * cmd = new char[100];
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

void parsingArgv(char * temp, char * argvTemp[])
{
	int index = 0;
	char *tok;
	tok = strtok(temp, " ");
	while(tok != NULL)
	{
		argvTemp[index] = new char[strlen(tok) + 2];
		strncpy(argvTemp[index], tok, strlen(tok));
		strncat(argvTemp[index], "\0", 1);
		index++;
		tok = strtok(NULL, " ");
	}
	argvTemp[index] = NULL;
	argvTemp[index + 1] = NULL;
}

//......................................................
//this is testing funciton nothing contribute to main program
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


