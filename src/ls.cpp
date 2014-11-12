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
#define FLAG_a 1
#define FLAG_l 2
#define FLAG_R 4

static bool is_l = false;
static bool is_a = false;
static char *path; 

using namespace std;

//.........................................
//these two function is for testing
void testString(char * test);
void testArgv(char * test[]);
//........................................

//get commands from the userinput.
void initialArgv();


void help(int argc, char ** argv);

//++++++++++
//user input a whole string, terminate untill enter
char * inputCommand();

//organize spaces that only have one space between two argument.
char * orgSpaces(char * cmd);

//pass commands in temp to argvNew spearate by space.
void parsingArgv(char * temp, char ** argvTemp);
//+++++++++++


//open dir and store files
void inDirectory(char ** files, char ** visibleFiles, char *dirName);

//print nicely
void print(char ** temp);

//print out for -l flag
void printInfo(char **temp);

// help printInfo 	
void printInfoHelp(char * file);

// print all files include inside of directories
void printAllDir(char **temp);

// print all subdirectories.
void printAllDirHelp(char *afile);

// clean memory
void freeArray(char ** array);




//print nicely
void print(char ** temp);


int main(int argc, char **argv)
{
 	initialArgv();
	return 0;
}

void initialArgv()
{
	char * cmdString = new char[100];
	char * argvNew[50];
	cmdString = inputCommand();
	cmdString = orgSpaces(cmdString);
	parsingArgv(cmdString, argvNew);
	int argcNew = 0;
	while(argvNew[argcNew] != NULL)
		argcNew++;
        help(argcNew, argvNew);

	initialArgv();	
        
}
 
void help(int argc, char ** argv)
{
 	char * files[200];
	char * visibleFiles[100];
	char c[2] = ".";
	int flag = 0;
	bool anotherPath= false;
	if(strcmp(argv[0],"ls") == 0)
	{
		for(int i = 1; i < argc; i++)
		{
			if(argv[i][0] == '-')
			{
				for(int j = 1; argv[i][j] != 0;j++)
				{
					if(argv[i][j] == 'a')
						flag |= FLAG_a;
					else if(argv[i][j] == 'l')
						flag |= FLAG_l;
					else if(argv[i][j] == 'R')
						flag |= FLAG_R;
				}
			}
		//if the argument is a path. 
			else if(argv[i][0] == '/' && !anotherPath)
			{
				inDirectory(files, visibleFiles, argv[i]);
				if(chdir(argv[i]) == -1)
				{
					perror("chdir");
					initialArgv();
				}
				anotherPath = true;
				path = argv[i];
			}
			else{
				cerr << "no such argument "<< endl;
				initialArgv();
			}
		}
		if(!anotherPath){
			inDirectory(files, visibleFiles, c);
			path = get_current_dir_name();
		}
	}
	else
	{
		cerr << argv[0] <<": no such command in this shell " << endl;
		initialArgv();
	}

	switch (flag){
		case 0: print(visibleFiles);  break;
			
		case 1: print(files); break;
			
		case 2: printInfo(visibleFiles); break;

		case 3: printInfo(files); break;
		
		case 4: printAllDir(visibleFiles); break;

		case 5: printAllDir(files); break;
			
		case 6: is_l = true;
			printAllDir(visibleFiles);	
			break;
		case 7: is_a = true; 
			is_l = true;
			printAllDir(files);
			break;
		default:
			cerr << "wrong option" << endl;
			initialArgv();
			break;	
			
		
	}
	freeArray(files);
	freeArray(visibleFiles);
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


void inDirectory(char ** files, char ** visibleFiles, char * dirName)
{
	DIR *dirp = opendir(dirName);
	if (dirp == NULL)
	{
		perror("opendir");
		initialArgv();	
	}
	dirent *direntp;
	int index = 0;
	while((direntp = readdir(dirp))){
		char * temp = direntp->d_name;
		files[index] = new char[strlen(direntp->d_name)];
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
			visibleFiles[j] = new char[strlen(files[i])];
			strcpy(visibleFiles[j],temp);
			j++;
		}
		i++;
	}
	visibleFiles[j] = NULL;
	closedir(dirp);
}

void print(char ** temp)
{
	int i = 0;
	while(temp[i] != NULL)
	{
		cout << left << setw(15) << temp[i];
		if(i % 6 == 4)
		cout << endl;
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
		perror("stat");
		initialArgv();
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
	
// number of hard links
	cout << right << setw(2) << statbuf.st_nlink << " ";

//user name
	char *username = getlogin();
	cout << username << " ";
	
	struct group *myGroup;
	myGroup = getgrgid(statbuf.st_gid);
	if(myGroup != NULL)
		cout << (myGroup->gr_name) << " ";
	else
		cout << "unknow"<< " ";
	cout << right << setw(6) << statbuf.st_size << " ";
	char *time = ctime(&(statbuf.st_ctime));
//the time[strlen[time] -1] contain four elements "\n,@,$,\0"
//so using while 
	int i = 0;
	while(time[i] != '\n')
	{
		cout << time[i];
		i++;
	}
	cout << " ";

	cout << left << setw(10) << file << endl;

}

void printAllDir(char **temp)
{
	if(is_l)
		printInfo(temp);
	else
		print(temp);
	cout << endl;
	int i = 0;
	while(temp[i] != NULL)
	{
		if(strcmp(temp[i],".") != 0 && strcmp(temp[i],"..") != 0)
			printAllDirHelp(temp[i]);
		i++;
	}
}

void printAllDirHelp(char *afile)
{
	struct stat buf;
	if(stat(afile, &buf) == -1)
	{
		perror("stat");
		initialArgv();
	}
	else{
		if(S_ISDIR(buf.st_mode))
		{
		//change the working path to the dirctory
			char *currentDir = get_current_dir_name();
			int index = strlen(path);
			int temp = strlen(currentDir) - strlen(path);
			char *curPath = new char[temp + 1];
			if(temp == 0){
				strncat(curPath,"\0",1);
				cout << afile << ": " << endl;
			}
			else
			{	
				memcpy(curPath, &currentDir[index+1], temp);
				strncat(curPath, "\0", 1);
				cout << "/" << curPath << "/"<<afile << ": " << endl;
			}  
			
			
			
			char *fAll[200];
			char *fvisib[100];
			inDirectory(fAll,fvisib,afile);
		
			if(chdir(afile) == -1)
			{
				perror(" ");
				initialArgv();
			}
			if(is_a)
				printAllDir(fAll);
			else
				printAllDir(fvisib);
		//change back to the previous working path
			if(chdir(currentDir) == -1)
			{
				perror(" ");
				initialArgv();
			}	
			freeArray(fAll);
			freeArray(fvisib);
		}
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


