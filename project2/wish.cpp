#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <fcntl.h>
using namespace std;
    
int main(int argc, char* argv[]){
    char error_message[30] = "An error has occurred\n";
    char* buffer;
    char* token;
    if(argc > 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    else{
        string commandLine;
        if(argc == 1){
            while(1){
            char prompt[7] = "wish> ";
            write(STDOUT_FILENO, prompt, strlen(prompt));
            if(!getline(cin, commandLine)){
                exit(0);
            }
            if(commandLine == "exit"){
                exit(0);
            }
            buffer = new char[commandLine.length() + 1];
            strcpy(buffer, commandLine.c_str());
            token = strtok(buffer, " \t");
            while(token != NULL){
                write(STDOUT_FILENO, token, strlen(token));
                write(STDOUT_FILENO, "\n", 1);
                token = strtok(NULL, " \t");
            }
            delete[] buffer;
            }
        }
        else{
            ifstream batchFile(argv[1]);
            if(!batchFile.is_open()){
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
            }
            while(getline(batchFile, commandLine)){
                buffer = new char[commandLine.length() + 1];
                strcpy(buffer, commandLine.c_str());
                token = strtok(buffer, " \t");
                while(token != NULL){
                    write(STDOUT_FILENO, token, strlen(token));
                    write(STDOUT_FILENO, "\n", 1);
                    token = strtok(NULL, " \t");
                }
                delete[] buffer;
            }
        }
    }
}