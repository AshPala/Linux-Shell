#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

//This is the prototype of the function that will redirect the stdout and stderr to another file
void redirection(char *file_out);



//This program was written as a shell with some built in funtions
//The funtions that are not built in if they can be called
//through setting the path using the setpath function
int main(int argc, char* argv[])
{
	//The tmp is used to read from the getline, the string is stored in to getline
	//The token variable stores the strtok from the tmp string. So token gets the word from the string in tmp
	//The value variable stores what is read from the getenv function so it stores the HOME path
	char *tmp;
	char *token, *value;

	//The following char array will hold the arguments from getline
	//They are numbered according to which argument index is read
	char *arg1;
	char arg2[256];
	char arg3[256];

	//This array is used to store the folders that the user wants in their path
	//This path will be used to call the funstion in the path
	char path[256];
	strcpy(path, "/bin/");


	char *file_out;//This variable will store the file that will stdout will write to 
	int ret;//This variable is to store the return value of the chdir funtion
	
	//The buffer array stores the input from the user using getline
	//I initialize the buffer with the buffsize using malloc
	char *buffer;
	size_t buffsize = 256;	
	buffer = (char *)malloc(buffsize * sizeof(char));
	
	char *buf; //This variable is refered to in the cd funtions - it is used the store and print the current path
	buf = (char *)malloc(buffsize * sizeof(char));
	
	
	//The following statment is for the -h flag
	//The implementations reading the number of arguments
	//if the user does not give the proper flag or the provide too many arguments then they 
	//are notified accordingly and the function is returned
	if((argc == 2))
	{
		if((strcmp(argv[1],"-h\0")==0))
		{
		printf("falsh: falsh [-h]\n\n\tfalsh is a sample shell with the purpose of manipulating environment.\n\n\tBuilt in commands:\n\n\texit   \t\texit the program\n\n\tpwd    \t\tprint the working directory\n\n\tcd     \t\tchange directory\n\n\tsetpath\t\tsets the path, user must provide atleast one argument(directory). The path is where falsh will look for executable programs to run\n\n\thelp   \t\tlists all the built in commands with short descriptions\n");
		return 0;
		}
		else
		{
			printf("Use -h for help.\n");
			return 0;
		}
	}
	else if((argc > 2))
	{
		printf("Too many arguements. Use -h for help.\n");
		return 0;
	}

	//This while loop enters the shell
	//This loop will exit only when the user enters the exit funtioin or when the user hits ctrl D
	//I print the first arrow indicator to where the user can type
	printf(">>>");
	while(getline(&buffer, &buffsize, stdin)!=EOF)
	{
		strcpy(arg2,"NULL"); //I am copying the string NULL to make sure that something is in arg2
				     //when arg2 is checked for the redirection arrow 
		tmp = buffer; //The tmp is holding the string that was read from the getline
		arg1 = strdup(strtok(tmp, " ")); //arg1 is storing the first word in the string, is done using strtok where the string is split when there is a space in the string
		arg1[strcspn(arg1, "\n")] = 0; //Here I trim of the newline character from arg1
		
		//This conditional is to check if there is another word in the string
		//Then the string is copied to arg2 and then the newline character is removed
		if((token = strtok(NULL, " "))!=NULL)
		{
			strcpy(arg2,token);
			arg2[strcspn(arg2, "\n")] = 0;
		}
		
		//This conditional is to check if the user had entered the > character
		//So it checks if the default string is in not in arg2 array
		if((strncmp(arg2,"NULL",4)!=0))
		{
			//If the user has entered the > character then the process to redirect the output
			//continues
			if((strlen(arg2)==1)&&(strncmp(arg2,">",1)==0))
			{	
				//Now if the user has not entered a filename then the user is alerted 
				//to enter the file name followed be the arrow
				if((token = strtok(NULL, " "))!=NULL) 
				{
					//If the user had enetered something after the filename 
					//then we know that the user had entered too many arguments 
					//then they are notified to follow the forma command > filename
					if(strtok(NULL," ")==NULL)
					{
					
					//Here I read the filename into the file_out variable
					//this is then truncated to exculde the newline character
					//Then the variable is passed to the redirection funtion	
					file_out = token;
					file_out[strcspn(file_out, "\n")] = 0;
					redirection(file_out);
					}
					else
					{
						printf("please follow: command > filename\n");	
					} 
				}
				else
				{
					printf("please follow: command > filename\n");
				}
			}
		}
		
		//This starts the first conditional train of if and if else statements
		//These statements are checked for the length of the string using strlen
		//The length is checked to see if user had enetered the right string
		//Then the statement checks for the string directly using strncmp 
		//And then finally we check arg2 to make sure that the user did not enter the > character
		//This is the built in exit function
		//This will exit the funtion by the use of the exit function
		if((strlen(arg1)==4)&&(strncmp(arg1,"exit",4)==0)&&(strncmp(arg2,">",1)!=0))
		{
			exit(0);
		}

		//This is the built in pwd funtion 
		//The getcwd function is able to retieve the current working directory
		//Getcwd stores what is read on the buf array
		//fflush then flushes out stdout to prevent segmentation fault
		else if((strlen(arg1)==3)&&(strncmp(arg1,"pwd",3)==0)&&(strncmp(arg2,">",1)!=0))
		{
			getcwd(buf, buffsize);
			printf("%s\n", buf);
			fflush(stdout);
		}
		
		//This is the built in cd function
		else if((strlen(arg1)==2)&&(strncmp(arg1,"cd",2)==0)&&(strncmp(arg2,">",1)!=0))
		{
			// arg2 is checked if it has changed from the default string using strcmp
			// if the default has not been chagned then the else statement is carred out
			// the else statement gets the home folder and then changes the direcory to it
			if((strncmp(arg2,"NULL",4)!=0))
			{
				//The access funtion checks if folder path that user entered exists
				//if the path does not exist then the function returns -1, thus the 
				//if statement checks if the folder path does exist
				if(access(arg2,F_OK) != -1)
				{
					//chdir is used to change the current folder path to
					//what the user had entered
					chdir(arg2);
				}
				else
				{
					printf("%s :No such file or directory\n",arg2);
				}		
			}
			else
			{
				//The getenv retrieves the home enviornment folder path
				//This is read into the value variable
				//Then chdir is used to change the directory to this folder
				value = getenv("HOME");
				chdir(value);
			}
		}
		
		//This is the built in setpath funtion
		//The setpath function will add the folders that the users enters into the path
		//if the folder exits then the commands that are available in the path are able to
		//be called
		else if((strlen(arg1)==7)&&(strncmp(arg1,"setpath",7)==0)&&(strncmp(arg2,">",1)!=0))
		{	
			//Here I check if the user had entered something after setpath 
			//if the user had then the default string would not have been found
			//so theen the if statement is carreid out
			//the else statement asks the user to enter a folder to add to the path
			if((strncmp(arg2,"NULL",4)!=0))
			{	
				//The path is stored in the path array
				//This array is cleared everytime setpath is called
				//hence the the first index in the array is set to 0
				path[0] = 0;

				//This do while loop will be used to read the words following setpath
				//if those words are a valid path then it is added to path 
				//if it does not exist the user is notified that tha path does not exist
				do{
					//The strcpy function is used to copy the next word in arg2
					//then the newline charcter is removed from arg2
					strcpy(arg2,token);
					arg2[strcspn(arg2, "\n")] = 0;
					
					//The first conditional checks if the user had entered a valid path
					//this is done using the access function which does not return -1 if the folder exists
					//then the folder is added to the path
					if(access(arg2,F_OK) != -1)
					{	
						//strncat is used to concatenate the sting onto the path
						//Then the : character is added to seperate the paths
						//The user is also notified that it has been found 
						strncat(path, arg2, strlen(arg2));
						strncat(path, ":",1);
						printf("%s :Path is found\n",arg2);
					}
					//This statement is for the first case where the user had entered a folder
					//and it does not exist, so this means that the path is empty
					//so the user is notified that the path is not found
					else if(path[0] == 0)
					{ 
						printf("%s :Path not found\n",arg2);
						strcpy(path, "/bin/");
					}
					//Since the path is not found the user is notified that the path does not exist
					else
					{
						printf("%s :Path not found\n",arg2);
					}
				}while((token = strtok(NULL," "))!= NULL);
			//Then the path is printed to show the user what is in the path
			printf("%s\n",path);
			}
			else if((strncmp(arg2,"NULL",4)==0))
			{
				printf("please specify a path\n");
			}
		}

		//This is the built in help function
		//This will print out the details of various funtions
		//The text does not look clean here but will so in the console
		else if((strlen(arg1)==4)&&(strncmp(arg1,"help",4)==0)&&(strncmp(arg2,">",1)!=0))
		{
	 		printf("falsh: falsh [-h]\n\n\tfalsh is a sample shell with the purpose of manipulating environment.\n\n\tBuilt in commands:\n\n\texit   \t\texit the program\n\n\tpwd    \t\tprint the working directory\n\n\tcd     \t\tchange directory\n\n\tsetpath\t\tsets the path, user must provide atleast one argument(directory). The path is where falsh will look for executable programs to run\n\n\thelp   \t\tlists all the built in commands with short descriptions\n");
		}

		//This is the function handler that searches the path to see if the function can be called
		//This checkd to make sure that the user had entered something for arg1 rather then hitting enter
		//so then the condition is passed and continued
		else if((arg1[0] != '\0')&&(strncmp(arg2,">",1)!=0))
		{
			
			//Refering the setpath function the folders are seperated by the : cahrarcter
			//the strtok function grabs the firest folder in the path and stores it into the token varaiable
			token = strtok(path, ":");
			
			//This do while loop conintually cycles to read all the folders in the path array
			do{
				strcpy(arg3,token); //I copy the folder to the arg3 array using strcpy
				strncat(arg3,"/",1); // I concatenate the / incase the user entered a path without cloding it with a /
				strncat(arg3,arg1,strlen(arg1)); // Finally I concatonate the command that the user had entered onto arg3
				
				//The acceess function does not return -1 if the command exists in the path
				//So the F_OK indicates that the path exists
				if(access(arg3,F_OK) != -1)
				{		
					//creating a fork and storing the process id into rc
					//wc will be the process id from waitpid 
					pid_t rc = fork();
					pid_t wc;
					
					//status is an int that will be passed into waitpid
					int status;
					
					if (rc < 0) {         // fork failed; exit
						fprintf(stderr, "fork failed\n");
						exit(1);
					} else if (rc == 0) { // child (new process)
						char* myargs[3];  //creating an array to store the arguements
						myargs[0] = strdup(arg3); //passing the command using strdup
						myargs[1] = NULL; //passing NULL for the end of the arguements
						execvp(myargs[0], myargs);  // execvp replaces the child 
									    //and passes the command that the user enters 
					} else {// parent goes down this path (main)
						//The while loop goes on until the child process is done
						do{
						//waitpid waits for the child process to complete, it
						//passes the child pid, the status of the pid and
						//WNOHANG is used to poll if the child process has exited
						wc = waitpid(rc, &status, WNOHANG);
						}while(wc == 0);
					}
				}
				//Now we know that the path has not been found if none of the above conditionals
				//have been found
				else
				{
					printf("-falsh: %s: command not found!\n",arg1);
				}
			}while((token = strtok(NULL, ":"))!=NULL);
			
		}
		//This will print if the user enters the command that can not be found
		else
		{
			if(strncmp(arg2,">",1)!=0)
			{
				printf("-falsh: %s: command not found\n",arg1);
			}
		}
		printf(">>>");
	}

	//This will free the memory for the buffers that were allocated at the begining of the function
	free(buffer);
	free(buf);
	return 0;
}



//This is the redirection function
//This passes in the name of the file as reference
//close is used to close STD_FILENO and STDERR_FILENO
//Then open is used to write stdout and stderr to the file
//Note that the file is truncated using O_TRUNC
void redirection(char *file_out)
{
	close(STDOUT_FILENO);
	open(file_out, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
	close(STDERR_FILENO);
	open(file_out, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
}






