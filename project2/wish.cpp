#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <vector>
#include <sys/wait.h>
using namespace std;
    
int main(int argc, char* argv[]){
    char error_message[30] = "An error has occurred\n";
    char* buffer;
    char* token;
    int found = -1;
    vector<char*> args;
    vector<string> paths = {"/bin/"};
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
                buffer = new char[commandLine.length() + 1];
                strcpy(buffer, commandLine.c_str());
                token = strtok(buffer, " \t");
                args.clear();
                while(token != NULL){
                    args.push_back(token);
                    token = strtok(NULL, " \t");
                }
                if(args.empty()){
                    delete[] buffer;
                    continue;
                }
                if(strcmp(args[0], "exit") == 0){
                    if(args.size() == 1){
                        exit(0);
                    }
                    else{
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        delete[] buffer;
                        continue;
                    }
                }
                if(strcmp(args[0], "cd") == 0){
                    if(args.size() != 2){
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        delete[] buffer;
                        continue;
                    }
                    else{
                        if(chdir(args[1]) != 0){
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                        delete[] buffer;
                        continue;
                    }
                }
                if(strcmp(args[0], "path") == 0){
                    paths.clear();
                    for(size_t i = 1; i < args.size(); i++){
                        paths.push_back(string(args[i]) + "/");
                    }
                    delete[] buffer;
                    continue;
                }
                args.push_back(NULL);
                found = -1;
                for(size_t j = 0; j < paths.size(); j++){
                    found = access((paths[j] + args[0]).c_str(), X_OK);
                    if(found == 0){
                        pid_t pid = fork();
                        if(pid > 0){
                            wait(NULL);
                            delete[] buffer;
                            break;
                        }
                        else if(pid == 0){
                            execv((paths[j] + args[0]).c_str(), args.data());
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(1);
                        }
                        else{
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            delete[] buffer;
                        }
                    }
                }
                if(found != 0){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    delete[] buffer;
                }
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
                args.clear();
                while(token != NULL){
                    args.push_back(token);
                    token = strtok(NULL, " \t");
                }
                if(args.empty()){
                    delete[] buffer;
                    continue;
                }
                if(strcmp(args[0], "exit") == 0){
                    if(args.size() == 1){
                        exit(0);
                    }
                    else{
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        delete[] buffer;
                        continue;
                    }
                }
                if(strcmp(args[0], "cd") == 0){
                    if(args.size() != 2){
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        delete[] buffer;
                        continue;
                    }
                    else{
                        if(chdir(args[1]) != 0){
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                        delete[] buffer;
                        continue;
                    }
                }
                if(strcmp(args[0], "path") == 0){
                    paths.clear();
                    for(size_t i = 1; i < args.size(); i++){
                        paths.push_back(string(args[i]) + "/");
                    }
                    delete[] buffer;
                    continue;
                }
                args.push_back(NULL);
                found = -1;
                for(size_t j = 0; j < paths.size(); j++){
                    found = access((paths[j] + args[0]).c_str(), X_OK);
                    if(found == 0){
                        pid_t pid = fork();
                        if(pid > 0){
                            wait(NULL);
                            delete[] buffer;
                            break;
                        }
                        else if(pid == 0){
                            execv((paths[j] + args[0]).c_str(), args.data());
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(1);
                        }
                        else{
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            delete[] buffer;
                        }
                    }
                }
                if(found != 0){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    delete[] buffer;
                }
            }
        }
    }
}