//header file for libraries required
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<sys/wait.h> 
#include<sys/types.h> 
#include<unistd.h> 
#include<fcntl.h> 
#include<errno.h> 
#include<sys/stat.h>

// methods declaration
char* getCurrentWorkingDirectory();
char* getCommandPrompt(char*);
char* getUserCommand();
void trim(char *);
char** parseInput(char*, int*);
int executeCommand(char*, char*[]);
char* changeDirectories(char*, char*, char*);

// main definition
int main() {
    char *netId = "arouf";
    char *commandPrompt = getCommandPrompt(netId);
    char *userCommand;
    do {
        printf("\n%s", commandPrompt);
        userCommand = getUserCommand();
        trim(userCommand);
        
        if(!strcmp(userCommand,"")) {
            continue;
        }

        int totalParts = 0;
        char** commandParts = parseInput(userCommand, &totalParts);
        
        char* command = commandParts[0];
        char* arguements[totalParts];
        for(int i = 0; i < totalParts; i++) {
            arguements[i] = commandParts[i];
        }

        if(!strcmp("exit", userCommand)) {
            printf("Command prompt terminated!\n");
            break;
        } else if (!strcmp("cd", userCommand)) {
            if(totalParts != 3) {
                printf("Path Not Formatted Correctly!");
            } else {
                commandPrompt = changeDirectories(commandPrompt, arguements[1], netId);
            }
        } else {   
            executeCommand(command, arguements);
            //printf("Execution done!");
        }
        free(commandParts);
    } while(1);
    
    free(userCommand);
    free(commandPrompt);
    return 0;
}
// methods definition

char* getUserCommand() {
    size_t maxCommandSize = 1024;
    char rawCommand[maxCommandSize];
    fgets(rawCommand, sizeof(rawCommand), stdin);   
    size_t length = strlen(rawCommand);
    if (length > 0 && rawCommand[length - 1] == '\n') {
        rawCommand[length - 1] = '\0';
    }
    char* command = (char *) malloc(length * sizeof(char));
    strcpy(command, rawCommand);
    return command;
}

void trim(char *sentence) {
    if (sentence == NULL) {
        return;
    }
    size_t len = strlen(sentence);
    if (len == 0) {
        return;
    }
    size_t start = 0;
    while (sentence[start] == ' ') {
        start++;
    }
    size_t end = len - 1;
    while (end > start && sentence[end] == ' ') {
        end--;
    }
    for (size_t i = 0; i <= end - start; i++) {
        sentence[i] = sentence[start + i];
    }
    sentence[end - start + 1] = '\0';
}

char* getCommandPrompt(char *netId) {
    char *cwd = getCurrentWorkingDirectory();
    char *promptText = (char *)malloc((strlen(cwd) + strlen(netId) + 2) * sizeof(char));
    
    strcat(promptText, netId);
    strcat(promptText, ":");
    strcat(promptText, cwd);
    strcat(promptText, "$");
    
    free(cwd);
    return promptText;
}

char* getCurrentWorkingDirectory() {
    char *command;
    size_t size = 512; // You can change the command size as needed
    size_t totalBytes = (size + 1) * sizeof(char);
    command = (char *)malloc(totalBytes);
    if (command == NULL) {
        perror("error in memory allocation!");
        exit(-1);
    }
    if (getcwd(command, size) == NULL) {
        perror("error in calling current working directory method!");
    }
    return command;
}

void printToken(char** tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        printf("%s\n", tokens[i]);
    }
}
char** parseInput(char* userCommand, int* partsCount) {
    int token_count = 0; 
    char *token; 
    char** tokens = NULL;
    tokens = malloc(sizeof(char *));
    token = strtok(userCommand, " ");
    while (token != NULL) {
        tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
        tokens[token_count] = token;
        token_count++;
        token = strtok(NULL, " ");
    }
    tokens = realloc(tokens, (token_count + 1) * sizeof(char *));
    tokens[token_count++] = NULL;
    (*partsCount) = token_count;
    return tokens;
}

int executeCommand(char* command, char* arguements[]) {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork error:");
        exit(pid);
    } else if (pid == 0) {
        int exec_result = execvp(command, arguements);
        perror("execvp failed:");
        _exit(exec_result);
    } else {
        int status;
        wait(&status);
        if (!WIFEXITED(status)) {
            printf("Child process did not exit normally\n");
            return -1;
        } else {
            return WEXITSTATUS(status); 
        }
    }
}

char* changeDirectories(char* commandPrompt, char* path, char* netId) {
    if (chdir(path) != 0) {
        perror("change directory error!"); 
    }
    return getCommandPrompt(netId);
}


