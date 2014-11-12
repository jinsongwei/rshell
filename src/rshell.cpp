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

static bool stop = false;
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

//execute commands
void executeCmd(char ** argv);

//fork a new process call execvp
int execvpCall(char ** argv);

//check if contain symbols
bool isSymbol(char **argv);

//clean memory
void freeArgv(char ** temp);



int main()
{
          help();
          return 0;
}
 
void help()
{
		char * cmdString = new char[100];
	        char * argvNew[50];
		cmdString = inputCommand();
		cmdString = orgSymbol(cmdString);
		cmdString = orgSpaces(cmdString);
	       	parsingArgv(cmdString, argvNew);
		executeCmd(argvNew);

		delete [] cmdString;
		cmdString = NULL;
		freeArgv(argvNew);
		help();	
}

char * inputCommand()
{
	char *htname = new char[64];
	if(gethostname(htname,64) == -1)
	{
		perror("gethostname");
	}
	char *temp = new char[100];
	memset(temp, '\0', 100);
	char c;
	cout << "[rShell_"<< getlogin() <<"/"<< htname << "] $";
	while(c != EOF)
	{
		c = getchar();
		if(c == '\n')
		{
	        	if(temp[0] == '\0')
 				cout << "[rShell_"<< getlogin() << "/"<< htname <<"] $";
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
	memset(newString, '\0',100);
	int i = 0;
	while(temp[i] != '\0')
	{
		if(temp[i] == ';')
		{
			strncat(newString, " ; ", 3);
		}
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
		else if(temp[i] == '#')
		{
			strncat(newString, " # ", 3);
		}
		else 
			strncat(newString, &temp[i],1);
		i++;
	}
	strncat(newString,"\0",1);
	delete [] temp;
	temp = NULL;
	return newString;
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

void executeCmd(char **argv)
{
	if(isSymbol(argv))
	{	
		int i = 0;
		int j = 0;
		int wait_pid;
		char *subArgv[20];
		while(argv[i] != NULL)
		{
			if(strcmp(argv[i],";") == 0 || 
				strcmp(argv[i],"&&") == 0 ||
					strcmp(argv[i],"||") == 0)
			{
				subArgv[j] = NULL; 
				wait_pid = execvpCall(subArgv);	
				freeArgv(subArgv);
				if((strcmp(argv[i],";") == 0 ||
					strcmp(argv[i], "&&") == 0) && wait_pid != 0)
				{	
					freeArgv(subArgv);
					stop = true;
					break;	
				}
			//store left of commands
				if(argv[i+1] != NULL){
					int tempIndex = i + 1;
					char *leftArgv[50];
					int index = 0;
					while(argv[tempIndex] != NULL)
					{
						leftArgv[index] = new char[strlen(argv[tempIndex])+ 1];
						strcpy(leftArgv[index], argv[tempIndex]);
						strncat(leftArgv[index],"\0", 1);
						index++;
						tempIndex++;
					}	
					leftArgv[index] = NULL;	
					executeCmd(leftArgv);
					freeArgv(leftArgv);
					if(stop)
						break;
				}
				else
					break;
			}
			subArgv[j] = new char[strlen(argv[i]) + 1];
			strcpy(subArgv[j], argv[i]);
			strncat(subArgv[j], "\0", 1);
			j++;
			i++;
		}
	}	
	else{
		stop = true;
		execvpCall(argv);
	}
}

int execvpCall(char ** argv)
{
	if(argv[0] == NULL)
	{
		cerr << "wrong format of command "<< endl;
		help();
	}
	if(strcmp(argv[0], "exit") == 0)
		exit(0);
	int wait_pid = 0;
	int pid = fork();
	if(pid == 0){
		if(execvp(argv[0], argv) == -1)
		{
			perror(argv[0]);
			exit(1);
		}
	}		
	else if(pid == -1)
	{
		perror("fork");
		exit(1);
	}
	else
	{
		if(waitpid(pid,&wait_pid,0) == -1)
		{
			perror("wait");	
			exit(1);
		}
	}
	return wait_pid;
}

bool isSymbol(char **argv)
{
	int i = 0;
	while(argv[i] != NULL)
	{
		if(strcmp(argv[i], ";") == 0
			 || strcmp(argv[i],"&&") == 0
				 || strcmp(argv[i],"||") == 0)
			return true;
		i++;
	}
	return false;
}
	
void freeArgv(char ** temp)
{
	int i = 0;
	while(temp[i] != NULL)
	{
		delete [] temp[i];
		temp[i] = NULL;
		i++;
	}
}


//......................................................
//this is testing funciton nothing contribute to main program
void testString(char * test)
{
	cout << "string length = " << strlen(test) << endl;
	cout << "string = " << test << endl;
}

void testArgv(char * test[])
{
	cout << "testArgv....." << endl;
	int i = 0;
	while(test[i] != NULL){
		cout << i << " "<< strlen(test[i]) << " = " << test[i] << endl;
		i++;
	}
	if(i == 0)
		cout << "nothing in here" << endl;
}
//.......................................................


