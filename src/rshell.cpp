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
#include <pwd.h>
#include <signal.h>
using namespace std;

static bool stop = false;
int childPid = 1;
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

//fork a new process call execv
int execvCall(char ** argv);

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//start pipe call if there is redirection
void pipeCall(char ** argv);

//if there is more than one redirect call then spearate them with three 
//argv, first two execute, and the rest argv waiting.
void pipeSeperate(char ** argv, char **argvL, char **argvR, char *symbol);

//for |
void pipeHelp1(char ** argvL, char ** argvR);

//for >
void pipeHelp2(char ** argvL, char ** argvR);

//for <
void pipeHelp3(char ** argvL, char ** argvR);

//for <<< 
void pipeHelp4(char ** argvL, char ** argvR);

//for 2>
void pipeHelp5(char ** argvL, char ** argvR);

bool isRedirect(char ** argv);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//check if contain symbols
bool isSymbol(char **argv);

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//PATH value 
//return NULL if call fail or doesn't have command user input
//else return the path that the command located.
char * checkPath(char * cmd);
int checkTarget(char *path, char * target);

//add cd command
int callCd(char ** argv);

//signal ^C
void handle_signal(int sig);

//clean memory
void freeArgv(char ** temp);


int main()
{	
//	char * temp = "bg";
//	char * path = checkPath(temp);
//	cout << path << endl;
        help();
          return 0;
}
 
void help()
{
	signal(SIGINT, handle_signal);

		int pid = fork();
		if(pid == 0){
			
			while(1){
				childPid = getpid();
				char * cmdString;
				char * argvNew[50];
				cmdString = inputCommand();
				//path = checkPath(cmdString);
				//testString(path);
				cmdString = orgSymbol(cmdString);
				cmdString = orgSpaces(cmdString);
				parsingArgv(cmdString, argvNew);
				if(strcmp(argvNew[0],"exit") == 0)
				{
				//	delete [] cmdString;
				//	cmdString =NULL;
					freeArgv(argvNew);
					if(-1 == kill(getppid(),SIGKILL))
						perror("kill");
				}
			
				executeCmd(argvNew);
			}
			//	freeArgv(argvNew);
			exit(0);
		}
		else if(pid ==-1)
		{
			perror("fork");
		}
		else{
			wait(0);
			help();
		}
//		help();
	
}

char * inputCommand()
{
	char *htname = new char[128];
	char *usrname = new char[128];
	memset(htname,'\0',128);
	memset(usrname,'\0',128);
	if(gethostname(htname,128) == -1)
	{
		perror("gethostname");
	}
	
	if((getlogin_r(usrname,128)) == -1)
		perror("getlogin");
	
	char *temp = new char[100];
	memset(temp, '\0', 100);
	char c;
	cout << "[rShell_"<< usrname <<"/"<< htname
				<<  get_current_dir_name()<<"] $" ;
	while(c != EOF)
	{
		c = getchar();
		if(c == '\n')
		{
	        	if(temp[0] == '\0')
				cout << "[rShell_"<< usrname <<"/"<< htname
				<<  get_current_dir_name()<<"] $" ;
			else
			{
                                strncat(temp, "\0", 1);
				break;
	                }
		}
		else
		strncat(temp, &c, 1);
	}
	
//	delete [] htname;
//	htname = NULL;
//	delete [] usrname;
//	usrname = NULL;

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
		else if(temp[i] == '>' )
		{
			if(temp[i+1] == '>')
			{
				strncat(newString, " > ",3);
				i++;
			}
			else
				strncat(newString, " > ", 3);
		}
		else if(temp[i] == '<')
		{
			if(temp[i+1] != '<')
				strncat(newString, " < ", 3);
			else if(temp[i+1] == '<' && temp[i+2] == '<')
			{
				 	strncat(newString, " <<< ", 5);
					i++;
					i++;
			}
		}
		else if(temp[i] == '#')
		{
			strncat(newString, " # ", 3);
		}
		else if(temp[i] == '2')
		{
			if( temp[i+1] != '\0' && temp[i+1] == '>')
			{
				if(temp[i+2] != '\0' && temp[i+2] == '>')
				{
					strncat(newString, " 2> ",4);
					i += 2;
				}
				else 
				{
					strncat(newString, " 2> ",4);
					i++;
				}
			}
			else
				strncat(newString, &temp[i],1);
				
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
		memset(argvTemp[index], '\0',strlen(argvTemp[index]));
		strcpy(argvTemp[index], tok);
		strncat(argvTemp[index], "\0", 1);
		index++;
		tok = strtok(NULL, " ");
	}
	while(index != 50){
		argvTemp[index] = NULL;
		index++;
	}
	delete [] temp;
	temp = NULL;
	
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
				wait_pid = execvCall(subArgv);	
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
						memset(leftArgv[index],'\0',strlen(leftArgv[index]));
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
			memset(subArgv[j],'\0',strlen(subArgv[j]));
			strcpy(subArgv[j], argv[i]);
			strncat(subArgv[j], "\0", 1);
			j++;
			i++;
		}
	}	
	else{
		stop = true;
		execvCall(argv);
	}
}

int execvCall(char ** argv)
{
	int wait_pid = 0;
	if(isRedirect(argv))
	{
		pipeCall(argv);
		return wait_pid;
	}
	else
	{
		if(strcmp(argv[0],"cd") == 0)
		{
			if(-1 == callCd(argv))
			{
				cerr << ("wrong call cd") << endl;
				return -1;
			}
		}
		else
		{
			if(strcmp(argv[0], "exit") == 0)
			{
				freeArgv(argv);
				exit(0);
			}
			int pid = fork();
			if(pid == 0){
				char * cmdPath = checkPath(argv[0]);
				
				if(cmdPath == NULL){
					cerr << "there is no such command" << endl;
					exit(1);
				}
				else{ 
					strcat(cmdPath,argv[0]); 
					strncat(cmdPath,"\0",1);
					memset(argv[0],'\0',strlen(argv[0]));
					strcpy(argv[0],cmdPath);
					strncat(argv[0],"\0",1);
					if(execv(cmdPath, argv) == -1)
					{
						perror(argv[0]);
						exit(1);
					}
				}
			}		
			else if(pid == -1)
			{
				perror("fork");
				freeArgv(argv);
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
		return wait_pid;
	}
}

//start piping >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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
		if(strcmp(argv[i],"<") == 0 || 
			strcmp(argv[i],">") == 0 || 
				strcmp(argv[i],"|") == 0 || 
				strcmp(argv[i],"<<<") == 0 ||
				strcmp(argv[i],"2>") == 0)
		{
			strcpy(symbol, argv[i]);
			j = i;
			i++;
			while(argv[i] != NULL) 
			{
				//copy redirect symbol right side.
				argvR[index] = new char[strlen(argv[i])];
				memset(argvR[index],'\0',strlen(argvR[index]));
				strcpy(argvR[index],argv[i]);
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
		memset(argvL[i],'\0',strlen(argvL[i]));
		strcpy(argvL[i],argv[i]);
		i++;
	}
	argvL[i] = NULL;
}

void pipeCall(char **argv)
{
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
	else if(strcmp(symbolRedirect,"<<<") == 0)
		pipeHelp4(argvL,argvR);	
	else if(strcmp(symbolRedirect,"2>") == 0)
		pipeHelp5(argvL,argvR);

//	freeArgv(argvL);
//	freeArgv(argvR);
	delete [] symbolRedirect;
	symbolRedirect = NULL;

}

void pipeHelp1(char ** argvL, char ** argvR)
{
//	testArgv(argvL);
//	testArgv(argvR);
	int fd[2];
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");

	int pid = fork();
	if(pid == -1)
	{
	   perror("There was an error with fork(). ");
	}
//child:::
	else if(pid == 0)
	{
	   if(-1 == dup2(fd[1],1))
	      perror("There was an error with dup2. ");
	   if(-1 == close(fd[0]))
	      perror("There was an error with close. ");
	   if(isRedirect(argvL))
	   {
		pipeCall(argvL);
	   }
	   else
	   {
		char * cmdPath = checkPath(argvL[0]);
		if(cmdPath == NULL){
			cerr << "there is no such command" << endl;
			exit(1);
		}
		else{ 
			strcat(cmdPath,argvL[0]); 
			strncat(cmdPath,"\0",1);
			//const char * cmd = cmdPath;	
			if(execv(cmdPath, argvL) == -1)
			{
				perror(argvL[0]);
				exit(1);
			}
	   	    }
	   }
	   exit(0);  
	}
//parent:::
	else if(pid > 0) //parent function
	{
	   int savestdin;
	   if(-1 == (savestdin = dup(0)))
		perror("wrong with calling dup");
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
			
			char * cmdPath = checkPath(argvR[0]);
			if(cmdPath == NULL){
				cerr << "there is no such command" << endl;
				exit(1);
			}
			else{ 
				strcat(cmdPath,argvR[0]); 
				strncat(cmdPath,"\0",1);
				//const char * cmd = cmdPath;	
				if(execv(cmdPath, argvR) == -1)
				{
					perror(argvR[0]);
					exit(1);
				}
	   		}
			exit(0);
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
	   if(-1 == dup2(savestdin,0))//restore stdin
	   perror("There is an error with dup2. ");

	}
	
}

void pipeHelp2(char **argvL, char ** argvR)
{
	int fd[2];
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");
	
	int pidFile;
	pidFile = open(argvR[0], O_RDWR | O_CREAT,S_IRWXU);
	if(pidFile == -1)	
		perror("open");
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
		else
		{
			char *cmdPath;
			cmdPath = checkPath(argvL[0]);
			if(cmdPath == NULL)
			{
				cerr << argvL[0] << " command does not exsit" << endl;
				exit(1);
			}
			else{
				strcat(cmdPath, argvL[0]);
				strncat(cmdPath, "\0", 1);
				if(-1 == execv(cmdPath, argvL)) {
					perror("There was an error in execv. ");
					exit(1);
				}	
			}
		}
	   exit(0);  
	}
//parent:::
	else if(pid > 0) //parent function
	{
	   int savestdin;
	   if(-1 == (savestdin = dup(0)))
		perror("wrong with dup");
	   //read end of the pipe
//	   if(-1 == (savestdin = dup(0)))
//	     perror("There is an error with dup. ");
	   if(-1 == dup2(fd[0],0))//make stdin the read end of the pipe 
	      perror("There was an error with dup2. ");
	    if(-1 == close(fd[1]))
	      perror("There was an error with close. ");
	   if( -1 == wait(0)) 
	      perror("There was an error with wait().");

		char buf[BUFSIZ];
		memset(buf, '\0', BUFSIZ);
		if(-1 == read(fd[0],buf,BUFSIZ))
			perror("read");
		if(-1 == write(pidFile, buf, BUFSIZ))
			perror("write");

	   	if(-1 == dup2(savestdin,0))
			perror("wrong with calling dup");
	}
	if(-1 == close(pidFile))
	   perror("there was wrong closing pidFile");
}

void pipeHelp3(char ** argvL, char ** argvR)
{
	int fd[2];
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");
	
	int pidFile;
	pidFile = open(argvR[0], O_RDWR | O_CREAT, S_IRWXU);
	if(pidFile == -1)
		perror("open");
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
	   exit(0);  
	}
//parent:::
	else if(pid > 0) //parent function
	{
	   int savestdin;
	   if(-1 == (savestdin = dup(0)))
		perror("worng with dup");
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
			char * cmdPath = checkPath(argvL[0]);
			if(cmdPath == NULL)
			{
				cerr << argvL[0] <<" command doesn't exist" << endl;
				exit(1);
			}
			else{
				strcat(cmdPath, argvL[0]);
				strncat(cmdPath,"\0",1);
				if(execv(cmdPath, argvL) == -1)
				{	
					perror("execv inside doesn't work properly");
					exit(1);
				}
			}
			exit(0);
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
		if(-1 == dup2(savestdin,0))
			perror("wrong with calling dup");
 	}
}


void pipeHelp4(char ** argvL, char ** argvR)
{
	int fd[2];
	
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");
	int pid = fork();
	if(pid == -1)
	{
	   perror("There was an error with fork(). ");
	}
	
//child:::
	else if(pid == 0)
	{
	   //write to the pipe
		if(-1 == dup2(fd[1],1))  //write stdout
			perror("There was an error with dup2. ");
		if(-1 == close(fd[0]))
			perror("There was an error with close. ");

		int i = 0;
		int size = 0;
		while(argvR[i] != NULL)
		{
			size += strlen(argvR[i]) + 1;
			i++;
		}
		size++;
		size++;
		char *buf = new char[size];
		memset(buf, '\0', size);
		i = 0;	
		while(argvR[i] != NULL)
		{
			strcat(buf, argvR[i]);
			strncat(buf, " ",1);
			i++;
		}
		strncat(buf, "\n", 1);
		strncat(buf, "\0", 1);
		if(-1 == write(fd[1], buf, size))
			perror("write");
		delete [] buf;
		buf = NULL;
	   exit(0);  
	}
//parent:::
	else if(pid > 0) //parent function
	{
	   int savestdin;
	   if(-1 == (savestdin = dup(0)))
		perror("call dup fail");
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
			char * cmdPath = checkPath(argvL[0]);
			if(cmdPath == NULL)
			{
				cerr << argvL[0] << " command does not exist"<< endl;
				exit(1);
			}
			else
			{
				strcat(cmdPath, argvL[0]);
				strncat(cmdPath, "\0",1);
				if(execv(cmdPath, argvL) == -1)
				{	
					perror("execv inside doesn't work properly");
					exit(1);
				}
			}
			exit(0);
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
	    if(-1 == dup2(savestdin,0))
		perror("call dup fail");
 	}
}


void pipeHelp5(char ** argvL, char ** argvR)
{

	int fd[2];
	if(pipe(fd) == -1)
	   perror("There was an error with pipe(). ");
	
	int pidFile;
	pidFile = open(argvR[0], O_RDWR | O_CREAT, S_IRWXU);
	if(pidFile == -1)	
		perror("open");
	int pid = fork();
	if(pid == -1)
	{
	   perror("There was an error with fork(). ");
	}
//child:::
	else if(pid == 0)
	{
	   //write to the pipe
		if(-1 == dup2(fd[1],2))//make pidFile the write end of the pipe 
	   		perror("There was an error with dup2. ");
		if(-1 == close(fd[0]))
		   	perror("There was an error with close. ");
		if(isRedirect(argvL))
		{
			pipeCall(argvL);
		}
		else 
		{
			char * cmdPath = checkPath(argvL[0]);
			if(cmdPath == NULL)
			{
				cerr << argvL[0] << " command does not exist"<< endl;
				exit(1);
			}
			else
			{
				strcat(cmdPath, argvL[0]);
				strncat(cmdPath, "\0",1);
				if(execv(cmdPath, argvL) == -1)
				{	
					delete [] cmdPath;
					cmdPath = NULL;
					perror("execv inside doesn't work properly");
					exit(1);
				}
			}
		}
	   exit(0);  
	}
//parent:::
	else if(pid > 0) //parent function
	{
	   int savestdin;
	   if(-1 == (savestdin = dup(0)))
		perror("wrong with dup");
	   //read end of the pipe
//	   if(-1 == (savestdin = dup(0)))
//	     perror("There is an error with dup. ");
	   if(-1 == dup2(fd[0],0))//make stdin the read end of the pipe 
	      perror("There was an error with dup2. ");
	    if(-1 == close(fd[1]))
	      perror("There was an error with close. ");
	   if( -1 == wait(0)) 
	      perror("There was an error with wait().");

		char buf[BUFSIZ];
		memset(buf, '\0', BUFSIZ);
		if(-1 == read(fd[0],buf,BUFSIZ))
			perror("read");
		if(-1 == write(pidFile, buf, BUFSIZ))
			perror("write");

	   	if(-1 == dup2(savestdin,0))
			perror("wrong with calling dup");
	}
	if(-1 == close(pidFile))
	   perror("there was wrong closing pidFile");

}



bool isRedirect(char ** argv)
{
	int i = 0;
	while(argv[i] != NULL)
	{
		if(strcmp(argv[i],"<") == 0 ||
			strcmp(argv[i],">") == 0 ||
				strcmp(argv[i],"|") == 0 ||
					strcmp(argv[i],"<<<") == 0 ||
					strcmp(argv[i],"2>") == 0)
	
			return true;
		i++;
	}
	return false;
	
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


char * checkPath(char * cmd)
{
	char * path[50];
	char * tempPath = getenv("PATH");
	
	int index = 0;
	char *tok;
	tok = strtok(tempPath, ":");
	while(tok != NULL)
	{
		path[index] = new char[strlen(tok) + 1];
		memset(path[index], '\0',strlen(path[index]));
		strcpy(path[index], tok);
		strncat(path[index], "\0", 1);
		index++;
		tok = strtok(NULL, ":");
	}
	while(index != 50){
		path[index] = NULL;
		index++;
	}
	
	int i = 0;
	while(path[i] != NULL)
	{
		if(checkTarget(path[i], cmd) == 1)
		{		
			strncat(path[i],"/",1);
			strncat(path[i],"\0",1);
			return path[i];
		}
		i++;
	}	

	return NULL;
}


	
int checkTarget(char *path, char *target)
{
	DIR *dirp = opendir(path);
	if(dirp == NULL)
	{
		return 0;
		perror("some directories cannot open without permmission");
	}

	dirent *direntp;
	while((direntp = readdir(dirp)))
	{
		if(strcmp(direntp->d_name,target) == 0)
		{
			if(-1 == closedir(dirp))
			{
				perror("closedir");
			}
			return 1;
		}
	}

	if(-1 == closedir(dirp))
	{
		perror("closedir");
	}
	return 0;
}

int callCd(char ** argv)
{
	if(argv[1] == NULL)
	{
		struct passwd *pw = getpwuid(getuid());
		if(-1 == chdir(pw->pw_dir))
		{
			perror("chdir");
			return -1;
		}
		return 1;
	}
	else 
	{
		char *current = new char[100];
		memset(current, '\0', 100);
		strcpy(current, get_current_dir_name());
		
		if(current == NULL)
		{
			perror("get_current_dir_name");
			return -1;
		}
		strncat(current, "/",1); 
		strcat(current,argv[1]);
//		strncat(current,"/",1);
		strncat(current, "\0",1);
		if(-1 == chdir(current))
		{
			perror("chdir");
			return -1;
		}
		return 1;
	}
}

void handle_signal(int sig)
{
//	cout << "in signal " << childPid << endl;
	cout << endl;
	if(childPid != 1)
	{
		if(-1 == kill(childPid,SIGKILL))
		perror("kill");
	}
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
	if(test == NULL)
	{
		cerr << "nothing in here " << endl;
		return;
	}
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


