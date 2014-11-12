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


void help(int argc, char ** argv);

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
          help(argc, argv);
          return 0;
}
 
void help(int argc, char ** argv)
{
 	char * files[200];
	char * visibleFiles[100];
	char c[2] = ".";
	int flag = 0;
	bool anotherPath= false;
	if(strcmp(argv[1],"ls") == 0)
	{
		for(int i = 2; i < argc; i++)
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
					exit(1);
				}
				anotherPath = true;
				path = argv[i];
			}
			else{
				cerr << "no such argument "<< endl;
				exit(1);
			}
		}
		if(!anotherPath){
			inDirectory(files, visibleFiles, c);
			path = get_current_dir_name();
		}
	}
	else
	{
		cerr << argv[1] <<": no such command in this shell " << endl;
		exit(1);
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
			exit(1);
			break;	
			
		
	}
	freeArray(files);
	freeArray(visibleFiles);
}

void inDirectory(char ** files, char ** visibleFiles, char * dirName)
{
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
	/*
	int i = 1;
	int maxLen = strlen(temp[0]);
	while(temp[i] != NULL)
	{
		if(maxLen < strlen(temp[i]))
			maxLen += strlen(temp[i]);
		i++;
	}
	*/
	int i = 0;
	while(temp[i] != NULL)
	{
		cout << left << setw(15) << temp[i];
		if(i % 5 == 4)
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
		exit(1);
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
				exit(1);
			}
			if(is_a)
				printAllDir(fAll);
			else
				printAllDir(fvisib);
		//change back to the previous working path
			if(chdir(currentDir) == -1)
			{
				perror(" ");
				exit(1);
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


