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

#define MODE(z) ((statbuf.st_mode & (z)) == (z))
using namespace std;

//.........................................
//these two function is for testing
void testString(char * test);
void testArgv(char * test[]);
//........................................


void help(int argc, char ** argv);

//user input a whole string, terminate untill enter
char * inputCommand();

//organize spaces that only have one space between two argument.
char * orgSpaces(char * cmd);

//pass commands in temp to argvNew spearate by space.
void parsingArgv(char * temp, char ** argvTemp);

//open dir and store files
void inDirectory(char ** files, char ** visibleFiles);

//passing argvNew and manipulate commands in it.
void outputCmd(char ** argvcmd, char ** argvfiles, char ** argvvib);

//print nicely
void print(char ** temp);

//print out for -l flag
void printInfo(char **temp);

// help printInfo 	
void printInfoHelp(char * file);

int main(int argc, char ** argv)
{
          help(argc, argv);
          return 0;
}
 
void help(int argc, char ** argv)
{
//	char * cmdString;
//       char * argvNew[10];
//	cmdString = inputCommand();
//        cmdString = orgSpaces(cmdString);
//        parsingArgv(cmdString, argvNew);

 	char * files[200];
	char * visibleFiles[100];
 	inDirectory(files, visibleFiles);
//passing argument argvNew
	outputCmd(argv, files, visibleFiles);
}

/*
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

char * orgSpaces(char * temp)
{
	char * cmd = new char[50];
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
	free(temp);
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
*/

void inDirectory(char ** files, char ** visibleFiles)
{
	char c[2] = ".";
	char *dirName = c;
	DIR *dirp = opendir(dirName);
	if (dirp == NULL)
	{
		perror("opendir");
		exit(1);
	}
	dirent *direntp;
	int index = 0;
	while((direntp = readdir(dirp))){
		char * temp = direntp->d_name;
		files[index] = new char[strlen(direntp->d_name)+1];
		strcpy(files[index],temp);
		index++;
	}
	files[index] = NULL;
//store all visible files without '.' in the front to the visibleFiles array
	int i = 0;
	int j = 0;
	char * temp;
	while(files[i] != NULL){
		temp = files[i];
		if(temp[0] != '.'){
			visibleFiles[j] = new char[strlen(files[i])+1];
			strcpy(visibleFiles[j],temp);
			j++;
		}
		i++;
	}
	visibleFiles[j] = NULL;

	closedir(dirp);
}

void outputCmd(char ** argvcmd, char ** argvfiles, char ** argvvib)
{
	if(strcmp(argvcmd[1],"ls") != 0)
	{
		cerr << "*** "<<argvcmd[0] << " is not command" << endl;
		exit(1);
	}
	else{
		if(argvcmd[2] == NULL)
			print(argvvib);
		else if(strcmp(argvcmd[2],"-a") == 0)
			print(argvfiles);
		else if(strcmp(argvcmd[2],"-l") == 0)
			printInfo(argvvib);
	}
	
}

void print(char ** temp)
{
	int i = 0;
	while(temp[i] != NULL)
	{
		cout << left << setw(10) << temp[i];
		i++;
	}
	cout << endl;
}


void printInfo(char **temp)
{
	int i = 0;
	while(temp[i] != NULL)
	{
		printInfoHelp(temp[i]);
		i++;
	}
}

void printInfoHelp(char *file)
{
	struct stat statbuf;
	if(stat(file, &statbuf) == -1)
	{
		perror("");
		exit(1);
	}

	if(S_ISDIR(statbuf.st_mode))
		cout << "d";
	else
		cout << "-";
	if(MODE(S_IRUSR))
		cout << "r";
	else
		cout << "-";
	if(MODE(S_IWUSR))
		cout << "w";
	else
		cout << "-";
	if(MODE(S_IXUSR))
		cout << "x";
	else
		cout << "-";
	if(MODE(S_IRGRP))
		cout << "r";
	else
		cout << "-";
	if(MODE(S_IWGRP))
		cout << "w";
	else
		cout << "-";
	if(MODE(S_IXGRP))
		cout << "x";
	else
		cout << "-";
	if(MODE(S_IROTH))
		cout << "r";
	else
		cout << "-";
	if(MODE(S_IWOTH))
		cout << "w";
	else
		cout << "-";
	if(MODE(S_IXOTH))
		cout << "x";
	else
		cout << "-";
	cout << " ";
//.............
// I don't know what should I insert? what the number means
//............

//user name
	char *username = getlogin();
	cout << username << " ";
	
	struct group *myGroup;
	myGroup = getgrgid(statbuf.st_gid);
	if(myGroup != NULL)
		cout << (myGroup->gr_name) << " ";
	else
		cout << "unknow"<< " ";
	cout << statbuf.st_size << " ";
	char *time = ctime(&(statbuf.st_ctime));
	int i = 0;
	while(time[i] != '\n')
	{
		cout << time[i];
		i++;
	}
	cout << " ";

	cout << file << endl;
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


