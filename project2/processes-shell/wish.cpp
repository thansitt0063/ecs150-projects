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
    vector<pid_t> childPids;
    vector<string> commandsInOneLine;
    vector<string> paths = {"/bin/"};
    if(argc > 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    else{
        string commandLine;
        if(argc == 1){
            while(1){
                childPids.clear();
                char prompt[7] = "wish> ";
                write(STDOUT_FILENO, prompt, strlen(prompt));
                if(!getline(cin, commandLine)){
                    exit(0);
                }
                size_t start = 0;
                size_t posOfAND = commandLine.find('&', start);
                commandsInOneLine.clear();
                while(posOfAND != string::npos){
                    commandsInOneLine.push_back(commandLine.substr(start, posOfAND - start));
                    start = posOfAND + 1;
                    posOfAND = commandLine.find('&', start);
                }
                commandsInOneLine.push_back(commandLine.substr(start));
                for(size_t t = 0; t < commandsInOneLine.size(); t++){
                    buffer = new char[commandsInOneLine[t].length() + 1];
                    strcpy(buffer, commandsInOneLine[t].c_str());
                    token = strtok(buffer, " \t");
                    args.clear();
                    while(token != NULL){
                        args.push_back(token);
                        token = strtok(NULL, " \t");
                    }
                    vector<string> findRedirection;
                    for(size_t k = 0; k < args.size(); k++){
                        string currentToken = args[k];
                        size_t pos = currentToken.find('>');
                        if(pos == string::npos){
                            findRedirection.push_back(currentToken);
                        }
                        else{
                            string command = currentToken.substr(0, currentToken.find(">"));
                            if(!command.empty()){
                                findRedirection.push_back(command);
                            }
                            findRedirection.push_back(">");
                            string outFile = currentToken.substr(currentToken.find(">") + 1);
                            if(!outFile.empty()){
                                findRedirection.push_back(outFile);
                            }
                        }
                    }
                    int redirectionCount = 0;
                    for(size_t l = 0; l < findRedirection.size(); l++){
                        if(strcmp(findRedirection[l].c_str(), ">") == 0){
                            redirectionCount++;
                        }
                    }
                    if(findRedirection.empty()){
                        delete[] buffer;
                        continue;
                    }
                    if(redirectionCount > 1 || findRedirection[0] == ">" || findRedirection[findRedirection.size() - 1] == ">" || (redirectionCount == 1 && findRedirection[findRedirection.size() - 2] != ">")){
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        delete[] buffer;
                        continue;
                    }
                    string outFileName;
                    for(size_t m = 0; m < findRedirection.size(); m++){
                        if(findRedirection[m] == ">"){
                            outFileName = findRedirection[m + 1];
                            break;
                        }
                    }
                    args.clear();
                    for(size_t n = 0; n < findRedirection.size(); n++){
                        if(findRedirection[n] == ">"){
                            break;
                        }
                        args.push_back(const_cast<char*>(findRedirection[n].c_str()));
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
                                childPids.push_back(pid);
                                delete[] buffer;
                                break;
                            }
                            else if(pid == 0){
                                int fileDescriptor;
                                int dup2Check;
                                if(!outFileName.empty()){
                                    fileDescriptor = open(outFileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                    if(fileDescriptor < 0){
                                        write(STDERR_FILENO, error_message, strlen(error_message));
                                        exit(1);
                                    }
                                    dup2Check = dup2(fileDescriptor, STDOUT_FILENO);
                                    if(dup2Check < 0){
                                        write(STDERR_FILENO, error_message, strlen(error_message));
                                        exit(1);
                                    }
                                    dup2Check = dup2(fileDescriptor, STDERR_FILENO);
                                    if(dup2Check < 0){
                                        write(STDERR_FILENO, error_message, strlen(error_message));
                                        exit(1);
                                    }
                                    close(fileDescriptor);
                                }
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
                for(size_t i = 0; i < childPids.size(); i++){
                    waitpid(childPids[i], NULL, 0);
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
                vector<string> findRedirection;
                for(size_t k = 0; k < args.size(); k++){
                    string currentToken = args[k];
                    size_t pos = currentToken.find('>');
                    if(pos == string::npos){
                        findRedirection.push_back(currentToken);
                    }
                    else{
                        string command = currentToken.substr(0, currentToken.find(">"));
                        if(!command.empty()){
                            findRedirection.push_back(command);
                        }
                        findRedirection.push_back(">");
                        string outFile = currentToken.substr(currentToken.find(">") + 1);
                        if(!outFile.empty()){
                            findRedirection.push_back(outFile);
                        }
                    }
                }
                int redirectionCount = 0;
                for(size_t l = 0; l < findRedirection.size(); l++){
                    if(strcmp(findRedirection[l].c_str(), ">") == 0){
                        redirectionCount++;
                    }
                }
                if(findRedirection.empty()){
                    delete[] buffer;
                    continue;
                }
                if(redirectionCount > 1 || findRedirection[0] == ">" || findRedirection[findRedirection.size() - 1] == ">" || (redirectionCount == 1 && findRedirection[findRedirection.size() - 2] != ">")){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    delete[] buffer;
                    continue;
                }
                string outFileName;
                for(size_t m = 0; m < findRedirection.size(); m++){
                    if(findRedirection[m] == ">"){
                        outFileName = findRedirection[m + 1];
                        break;
                    }
                }
                args.clear();
                for(size_t n = 0; n < findRedirection.size(); n++){
                    if(findRedirection[n] == ">"){
                        break;
                    }
                    args.push_back(const_cast<char*>(findRedirection[n].c_str()));
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
                            int fileDescriptor;
                            int dup2Check;
                            if(!outFileName.empty()){
                                fileDescriptor = open(outFileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                if(fileDescriptor < 0){
                                    write(STDERR_FILENO, error_message, strlen(error_message));
                                    exit(1);
                                }
                                dup2Check = dup2(fileDescriptor, STDOUT_FILENO);
                                if(dup2Check < 0){
                                    write(STDERR_FILENO, error_message, strlen(error_message));
                                    exit(1);
                                }
                                dup2Check = dup2(fileDescriptor, STDERR_FILENO);
                                if(dup2Check < 0){
                                    write(STDERR_FILENO, error_message, strlen(error_message));
                                    exit(1);
                                }
                                close(fileDescriptor);
                            }
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