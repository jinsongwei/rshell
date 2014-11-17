#include <sys/stat.h>
#include <fcntl.h>
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

//if there is more than one redirect call then spearate them with three 
//argv, first two execute, and the rest argv waiting.
void pipeSeperate(char ** argv, char **argvL, char **argvR, char *symbol);

void pipeHelp1(char ** argvL, char ** argvR);

void pipeHelp2(char ** argvL, char ** argvR);

void pipeHelp3(char ** argvL, char ** argvR);

bool isRedirect(char ** argv);
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

void pipeSeperate(char **argv, char **argvL, char **argvR, char *symbol)
{
	int i = 0;
	int j = 0;
	int index = 0;
	while(argv[i] != NULL)
		i++;
	i--;	
	while(i >= 0)
	{
		if(strcmp(argv[i],"<") == 0 || strcmp(argv[i],">") == 0 || strcmp(argv[i],"|") == 0)
		{
			strcpy(symbol, argv[i]);
			j = i;
			i++;
			while(argv[i] != NULL) 
			{
				//copy redirect symbol right side.
				argvR[index] = new char[strlen(argv[i])];
				strncpy(argvR[index],argv[i],strlen(argv[i]));
				index++;
				i++;
			}
			argvR[index] = NULL;
			break;
		}
		i--;
	}
	i = 0;
	while(i != j)
	{
		argvL[i] = new char[strlen(argv[i])];
		strncpy(argvL[i],argv[i], strlen(argv[i]));
		i++;
	}
	argvL[i] = NULL;
}

void pipeCall(char **argv)
{
//separate by '| < >', passing two argv
	char *argvL[50];
	char *argvR[50];
	char *symbolRedirect = new char[10];
	memset(symbolRedirect,'\0',10);
	
	pipeSeperate(argv, argvL,argvR,symbolRedirect);	
/*
	cout << " left side <<<<<" << endl;
	testArgv(argvL);
	cout << "right side >>>>>" << endl;
	testArgv(argvR);
	testString(symbolRedirect);
*/

	if(strcmp(symbolRedirect,"|") == 0)
		pipeHelp1(argvL,argvR);
	else if(strcmp(symbolRedirect,">") == 0)
		pipeHelp2(argvL,argvR);
	else if(strcmp(symbolRedirect,"<") == 0)
		pipeHelp3(argvL,argvR);	
	freeArgv(argvL);
	freeArgv(argvR);

}

void pipeHelp1(char ** argvL, char ** argvR)
{
//	testArgv(argvL);
//	testArgv(argvR);
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
	   if(isRedirect(argvL))
	   {
		pipeCall(argvL);
	   }
	   else if(-1 == execvp(argvL[0], argvL)) 
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


}

void pipeHelp2(char **argvL, char ** argvR)
{
	int fd[2];
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");
	bool isFile = false;
	
	int pidFile;
	pidFile = open(argvR[0], O_RDWR);
	if(pidFile != -1)	
		isFile = true;
	int pid = fork();
	if(pid == -1)
	{
	   perror("There was an error with fork(). ");
	   exit(1);
	}
//child:::
	else if(pid == 0)
	{
	   //write to the pipe
		if(-1 == dup2(fd[1],1))//make pidFile the write end of the pipe 
	   		perror("There was an error with dup2. ");
		if(-1 == close(fd[0]))
		   	perror("There was an error with close. ");
		if(isRedirect(argvL))
		{
			pipeCall(argvL);
		}
		else if(-1 == execvp(argvL[0], argvL)) 
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
		if(!isFile)
		{
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

		else	
		{
			char buf[BUFSIZ];
			memset(buf, '\0', BUFSIZ);
			if(-1 == read(fd[0],buf,BUFSIZ))
				perror("read");
			if(-1 == write(pidFile, buf, BUFSIZ))
				perror("write");
		}

	}
	if(-1 == dup2(pidFile,0))//restore stdin
	   perror("There is an error with dup2. ");
}

void pipeHelp3(char ** argvL, char ** argvR)
{
	int fd[2];
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");
	bool isFile = false;
	
	int pidFile;
	pidFile = open(argvR[0], O_RDWR);
	if(pidFile != -1)	
		isFile = true;
	int pid = fork();
	if(pid == -1)
	{
	   perror("There was an error with fork(). ");
	   exit(1);
	}
//child:::
	else if(pid == 0)
	{
	   //write to the pipe
		if(-1 == dup2(fd[1],1))//make pidFile the write end of the pipe 
			perror("There was an error with dup2. ");
		if(-1 == close(fd[0]))
			perror("There was an error with close. ");

			char buf[BUFSIZ];
			memset(buf, '\0', BUFSIZ);
			if(-1 == read(pidFile,buf,BUFSIZ))
				perror("read");
			if(-1 == write(fd[1], buf, BUFSIZ))
				perror("write");
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
	   if(isRedirect(argvL))
		pipeCall(argvL);		
	   else
	   {
		int fpid = fork();
		if(fpid == 0)
		{
			if(execvp(argvL[0], argvL) == -1)
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
 	}
//	if(-1 == dup2(pidFile,0))//restore stdin
//	   perror("There is an error with dup2. ");
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

bool isRedirect(char ** argv)
{
	int i = 0;
	while(argv[i] != NULL)
	{
		if(strcmp(argv[i],"<") == 0 ||
			strcmp(argv[i],">") == 0 ||
				strcmp(argv[i],"|") == 0)
			return true;
		i++;
	}
	return false;
	
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
	cout << "end of argv ...................." << endl;
}
//.......................................................


