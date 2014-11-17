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

void pipeCall(char ** argv);
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
		
	//	testArgv(argvNew);	
		pipeCall(argvNew);
		
		delete [] cmdString;
		cmdString = NULL;
		freeArgv(argvNew);
	//	help();
}

char * inputCommand()
{
	char *temp = new char[100];
	memset(temp, '\0', 100);
	char c;
	cout << "[rShell_] $";
	while(c != EOF)
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
		else if(temp[i] == '|' && temp[i+1] != '|')
		{
			strncat(newString, " | ", 3);
		}
		else if(temp[i] == '>')
		{
			strncat(newString, " > ", 3);
		}
		else if(temp[i] == '<')
		{
			strncat(newString, " < ", 3);
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

void pipeCall(char **argv)
{
//separate by '|', passing two argv
	char *argvL[10];
	char *argvR[10];
	int i = 0;
	int index = 0;
	while(argv[i] != NULL && strcmp(argv[i],"|") != 0)	
	{
		argvL[i] = new char[strlen(argv[i])];
		strcpy(argvL[i],argv[i]);
		i++;
	}
	argvL[i] = NULL;
	i++;
	while(argv[i] != NULL)	
	{
		argvR[index] = new char[strlen(argv[i])];
		strcpy(argvR[index],argv[i]);
		index++;
		i++;
	}
	argvR[index] = NULL;	

// start to piping, 

	int fd[2];
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");

	int pid = fork();
	int savestdin;
	if(pid == -1)
	{
	   perror("There was an error with fork(). ");
	   exit(1);
	}
//child:::
	else if(pid == 0)
	{
	   //write to the pipe
	   if(-1 == dup2(fd[1],1))//make stdout the write end of the pipe 
	      perror("There was an error with dup2. ");
	   if(-1 == close(fd[0]))
	      perror("There was an error with close. ");
	   if(-1 == execvp(argvL[0], argvL)) 
	      perror("There was an error in execvp. ");


	   exit(1);  
	}
//parent:::
	else if(pid > 0) //parent function
	{
	   //read end of the pipe
//	   if(-1 == (savestdin = dup(0)))
//	     perror("There is an error with dup. ");
	   if(-1 == dup2(fd[0],0))//make stdin the read end of the pipe 
	      perror("There was an error with dup2. ");
	    if(-1 == close(fd[1]))
	      perror("There was an error with close. ");
	   if( -1 == wait(0)) 
	      perror("There was an error with wait().");
		int fpid = fork();
		if(fpid == 0)
		{
			if(execvp(argvR[0], argvR) == -1)
			{	
				perror("execvp inside doesn't work properly");
				exit(1);
			}
			exit(1);
		}
		else if(fpid == -1)
		{
			perror("fork fail");
		}
		else
		{
			if(-1 == wait(NULL))
				perror("wait");
		}
	}
	if(-1 == dup2(savestdin,0))//restore stdin
	   perror("There is an error with dup2. ");

	freeArgv(argvL);
	freeArgv(argvR);

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


