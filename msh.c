//
//msh.c
//  
//
//Created by Caroline Frey on 7/12/18.
//
//Creation date: February 20, 2018
//Project2, Operating Systems

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "stringlist.h"
#include "lexer.h"

// readline function, takes debugFlag as an additional paremeter (FEATURE 2)
char *readline(int fd, int debugFlag) {
    char *s = malloc(256); // assumes line will be 255 characters or less
    int n = 0;
    while (1) {
        int r = read(fd, s + n, 1);
        if (r == 0) {
            printf("Line didn't end, but there is no more data available in file %d\n", fd);
            s[n] = '\0'; // add the terminating zero and return the string
            return s;
        } else if (r < 0) {
            printf("There was a problem reading from file %d: error number %d\n", fd, n);
            s[n] = '\0'; // add the terminating zero and return the string
            return s;
        } else if (s[n] == '\n') {
            s[n] = '\0'; // add the terminating zero and return the string
            
            if (debugFlag == 1) {                      //FEATURE 2
                printf("About to execute: %s.\n", s);
            }
            return s;
        } else if (n == 255) {
            printf("Already read 255 characters, but line didn't end yet in file %d\n", fd);
            s[n] = '\0'; // add the terminating zero and return the string
            return s;
            
        } else {
            n++;
        }
    }
}

void howareyou(char* input) {
    printf("I'm doing well, thank you.\n");
}

void currtime(char* input) {
    struct timespec now;
    
    clock_gettime(CLOCK_REALTIME, &now);
    int seconds = now.tv_sec;
    int nanoseconds = now.tv_nsec;
    printf("The time is %d seconds, %d nanoseconds since midnight, Jan 1, 1970.\n", seconds, nanoseconds);
}

void name(char* input) {
    printf("My name is Caroline.\n");
}

void id(char* input) {
    printf("My id is process %d.\n", getpid());
}

void parentid(char* input) {
    printf("My parent's id is process %d.\n", getppid());
}

int getage(char* input) {
    struct timespec now2;
    clock_gettime(CLOCK_REALTIME, &now2);
    int seconds2 = now2.tv_sec;
    return seconds2;
}

void whoami(char* input) {
    printf("You are %s, silly.\n", getenv("USER"));
}

void help(char* input) {
    printf("I know how to respond to these commands:\n");
    printf(" who am i\n");
    printf("tell me the time\n");
    printf("tell me your age\n");
    printf("tell me your id\n");
    printf("tell me your parent's id\n");
    printf("say [any phrase]\n");
    printf("sleep [amount of time]\n");
    printf("list [dirname]\n");
    printf("open [filename]\n");
    printf("read [file number]\n");
    printf("close [file number]\n");
    printf("quit\n");
}

void quitprog(char* input) {
    printf("Goodbye!\n");
    exit(0);
}

//isFileExecutable() returns non-zero if the file exists and is executable, //
// otherwise it returns 0.
int isFileExecutable(const char *filename) {
    struct stat statinfo;
    int result = stat(filename, &statinfo);
    if (result < 0) return 0;
    if (!S_ISREG(statinfo.st_mode)) return 0;
    
    if (statinfo.st_uid == geteuid()) return statinfo.st_mode & S_IXUSR;
    if (statinfo.st_gid == getegid()) return statinfo.st_mode & S_IXGRP;
    return statinfo.st_mode & S_IXOTH;
}



int main(int argc, const char * argv[]) {
    setbuf(stdout, NULL);
    int debugFlag = 0;
    
    if (argc > 1) {                      //FEATURE 2
        const char *firstArg = argv[1];
        const char *x = "-x";
        if (strcmp(firstArg, x) == 0) {
            debugFlag = 1;
        }
    }
    
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    int seconds = now.tv_sec;
    int nanoseconds = now.tv_nsec;
    printf("Welcome to chat assistant!\n");
    printf("The time is %d seconds, %d nanoseconds since midnight, Jan 1, 1970.\n", seconds, nanoseconds);
    printf("Type \"help\" if you are lost.\n");
    printf("What next? ");
    
    char *s;                        //char declarations used for comparisons
    char *t;
    char *repeat = "say ";
    char *sleep = "sleep ";
    char *fileopen = "open ";
    char *fileread = "read ";
    char *fileclose = "close ";
    char *filelist = "list ";
    s = readline(0, debugFlag);
    
    while (s != 0) {
        
        int a = strncmp(s, repeat, 4); //determine user entry using string compare function
        int b = strncmp(s, sleep, 6);
        int c = strncmp(s, fileopen, 5);
        int d = strncmp(s, fileread, 5);
        int e = strncmp(s, fileclose, 6);
        int f = strncmp(s, filelist, 5);
        
        if (a == 0) {                                       //"say..."
            printf("%s\n", s + 4);
            
        } else if (b == 0) {                                //"sleep..."
            char *num = s + 6;
            int duration = atoi(num);
            duration = duration * 1000000;                  //convert to microseconds
            printf("Going to sleep for %d microseconds...\n", duration);
            usleep(duration);
            printf("OK, that was a nice nap.\n");
            
        } else if (c == 0) {                                //"open..."
            char *filename = s + 5;
            int filedesc = open(filename, O_RDONLY);        //Open file to be read only
            if (filedesc < 0) {
                printf("Error! Check file.\n");
            } else {
                printf("OK, I just opened file number %d.\n", filedesc);
            }
            
        } else if (d == 0) {                                //"read..."
            char *desc = s + 5;                             //store user entry
            int filedesc = atoi(desc);                      //convert entry from char to int
            if (filedesc < 0) {                             //error check
                printf("Error! Check file descriptor.\n");
            } else {
                t = readline(filedesc, debugFlag);                         //read line of specified file
                printf("The next line of file %d is: %s\n", filedesc, t);
                free(t);
            }
            
        } else if (e == 0) {                                //"close..."
            char *filetoclose = s + 6;                      //store user entry
            int filedesc = atoi(filetoclose);               //convert entry from char to int
            if (filedesc < 0) {
                printf("Error! Check file descriptor.\n");
            } else {
                close(filedesc);                            //close specified file
                printf("Closed file %d.\n", filedesc);
            }
            
        } else if (f == 0) {                               //"list..."
            char *filestolist = s + 5;                     //NOTE: code source cited in collab log
            printf("OK, here are the things in directory %s.\n", filestolist);
            DIR* opened;
            struct dirent* direntp;
            
            opened = opendir(filestolist);                 //open specified directory
            
            if (opened == NULL) {                          //error message if directory empty
                printf("Error. Can't open %s.\n", filestolist);
            } else {                                       //else, print list of files in directory...
                direntp = readdir(opened);
                while ((direntp = readdir(opened)) != NULL) {   //...until pointer is NULL (end of list)
                    
                    printf("%s\n", direntp->d_name);
                }
                
                closedir(opened);                           //close file
            }
            
        } else if (strcmp(s, "how are you") == 0) {
            howareyou(s);
            
        } else if (strcmp(s, "tell me the time") == 0) {
            currtime(s);
            
        } else if (strcmp(s, "tell me your name") == 0) {
            name(s);
            
        } else if (strcmp(s, "tell me your id") == 0) {
            id(s);
            
        } else if (strcmp(s, "tell me your parent's id") ==0) {
            parentid(s);
            
        } else if (strcmp(s, "tell me your age") == 0) {
            int currentage = getage(s);
            currentage = currentage - seconds;
            printf("I am about %d seconds old.\n", currentage);
            
        } else if (strcmp(s, "who am i") == 0) {
            whoami(s);
            
        } else if (strcmp(s, "help") == 0) {
            help(s);
            
        } else if (strcmp(s, "quit") == 0) {
            quitprog(s);
            
        } else {
            int status;
            int checkA, checkB, checkC;  //used to store return value of isFileExec()
            int outputDestCheck = -1;
            int inputDestCheck = -1;
            
            
            for (int i = 0; i < strlen(s); i++) {     //search s for '>'
                if (s[i] == '>') {
                    outputDestCheck = 0;
                }
            }
            
            for(int i = 0; i < strlen(s); i++) {      //search s for '<'
                if (s[i] == '<') {
                    inputDestCheck = 0;
                }
            }
            
            char **command = split_words(s);
            char *fileOutName;
            char *fileInName;
            char *carrot1;
            char *carrot2;
            int appendCheck = -1;
            int file1, file2;
            
            //The following two checks ensure the correct file names are saved.
            //If there is an output redirect, the last argument is the fileOutName.
            //If there is also an input redirect, the next word (after the > is removed), needs is the input file name.
            if (outputDestCheck == 0) {
                fileOutName = stringlist_pop(&command);
                carrot1 = stringlist_pop(&command);
                free(carrot1);
                if (strcmp(carrot1, ">>") == 0 ) {
                    appendCheck = 0;
                }
            }
            
            if (inputDestCheck == 0) {
                fileInName = stringlist_pop(&command);
                carrot2 = stringlist_pop(&command);
                free(carrot2);
            }
            
            char *slashcheck = strchr(command[0], '/');  //check for slash
            
            if (slashcheck != NULL) {     //slash found
                
                int cpid = fork();
                if (cpid == 0) {
                    
                    if (outputDestCheck == 0) {  //output redirection (if needed)
                        if (appendCheck == 0) {
                            file1 = open(fileOutName, O_WRONLY | O_APPEND);
                        } else {
                            file1 = open(fileOutName, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
                        }
                        dup2(file1, STDOUT_FILENO);
                        close(file1);
                    }
                    
                    if (inputDestCheck == 0) {   //input redirection (if needed)
                        file2 = open(fileInName, O_RDONLY, S_IRWXU);
                        if (file2 < 0) {        //input file doesn't exist
                            printf("Input file does not exist.\n");
                        } else {
                            dup2(file2, STDIN_FILENO);
                            close(file2);
                        }
                    }
                    
                    checkA = isFileExecutable(command[0]); //check file
                    if (checkA == 0) {
                        printf("File %s is not executable! Please try again.\n", command[0]);
                    }
                    
                    execv(command[0], command);   //execute
                    printf("Oops! Execution failed.\n");
                    exit(0);
                }
                
                int childpid = waitpid(cpid, &status, WUNTRACED); //wait for child to exit
                printf("Process %d finished with status %d.\n", childpid, WEXITSTATUS(status));
                free(fileInName);
                free(fileOutName);
                
            } else {       //(slash not found)
                
                char **filePathCheck1 = stringlist_empty();             //create new stringlist
                stringlist_append(&filePathCheck1, "/usr/local/bin/");  //append "/usr/local/bin/"
                stringlist_append(&filePathCheck1, command[0]);         //append user input
                char *check1 = stringlist_to_string(filePathCheck1);    //convert to string, ex: "/usr/local/bin/emacs"
                
                char **filePathCheck2 = stringlist_empty();
                stringlist_append(&filePathCheck2, "/bin/");
                stringlist_append(&filePathCheck2, command[0]);
                char *check2 =  stringlist_to_string(filePathCheck2);
                
                char **filePathCheck3 = stringlist_empty();
                stringlist_append(&filePathCheck3, "/usr/bin/");
                stringlist_append(&filePathCheck3, command[0]);
                char *check3 = stringlist_to_string(filePathCheck3);
                
                char *finalAnswer = NULL;
                
                checkA = isFileExecutable(check1); //determine which path is executable
                checkB = isFileExecutable(check2);
                checkC = isFileExecutable(check3);
                
                if (checkA != 0) {
                    finalAnswer = check1;
                } else if (checkB != 0) {
                    finalAnswer = check2;
                } else if (checkC != 0) {
                    finalAnswer = check3;
                } else {
                    printf("File is not executable! Please try again.\n");
                }
                
                int cpid = fork();                 //create child
                if (cpid == 0) {
                    
                    if (outputDestCheck == 0) {   //output redirection (if needed)
                        if (appendCheck == 0) {
                            file1 = open(fileOutName, O_WRONLY | O_APPEND);
                        } else {
                            file1 = open(fileOutName, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
                        }
                        dup2(file1, STDOUT_FILENO);
                        close(file1);
                    }
                    
                    if (inputDestCheck == 0) {   //input redirection (if needed)
                        file2 = open(fileInName, O_RDONLY);
                        if (file2 < 0) {  //input file doesn't exist
                            printf("Input file does not exist.\n");
                        } else {
                            dup2(file2, STDIN_FILENO);
                            close(file2);
                        }
                    }
                    
                    execv(finalAnswer, command);
                    printf("Oops! Execution failed.\n");
                    exit(0);
                }
                
                int childpid = waitpid(cpid, &status, WUNTRACED);      //wait for child to exit
                printf("Process %d finished with status %d.\n", childpid, WEXITSTATUS(status));
                free(fileInName);
                free(fileOutName);
            } 
        }
        printf("What next? ");           
        free(s);
        s = readline(0, debugFlag);
    }
    return 0;
}







