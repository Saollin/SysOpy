#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <pwd.h>

#include "common.h"

int serverQueue;
int chat = -1;
int myQueue;
int idOnServer = -1;

char * intToString(int i) {
    char * str = callock(200, sizeof(char));
    sprintf(str, "%d", i);
    return str;
}

void sendMsg(int queue, char * msg, long type) {
    msgbuf message;
    message.type = type;
    strcpy(message.text, msg);
    msgsnd(queue, &message, sizeof(message.text), 0);
}


msgbuf * getMsg(int queue) {
    msgbuf * tmp = calloc(1, sizeof(msgbuf));
    msgrcv(queue, tmp, sizeof(tmp->text), 0, MSG_NOERROR);
    return tmp;
}

msgbuf * getMsgOfType(int queue, long type) {
    msgbuf * tmp = calloc(1, sizeof(msgbuf));
    msgrcv(queue, tmp, sizeof(tmp->text), type, MSG_NOERROR);
    return tmp;
}

void stopClient() {
    sendMsg(serverQueue, intToString(idOnServer), STOP);
    if(chat != -1) {
        sendMsg(chat, intToString(idOnServer), DISCONNECT);
    }
    printf("Queue will be deleted.");
    msgctl(myQueue, IPC_RMID, NULL);
    exit(0);
}

void startChat() {
    printf("Start with %d has started.\nWrite \\end to end it.\n", chat);
    
    struct msqid_ds info;
    int messagesNumber;
    char line[100];
    bool isChatRunning = true;
    do {
        msgctl(myQueue,IPC_STAT, &info);
        messagesNumber = info.msg_qnum;
        while(messagesNumber > 0) {
            msgbuf * msg = getMsg(myQueue);
            if(msg->type == DISCONNECT) {
                sendMsg(serverQueue, intToString(idOnServer), DISCONNECT);
                isChatRunning = false;
                break;
            }
            printf("%s \n", msg->text);
            msgctl(myQueue, IPC_STAT, &info);
            messagesNumber = info.msg_qnum;
        }
        if(isChatRunning) {
            fgets(line, sizeof(line), stdin);
            if(!strcmp(line, "\\end")) {
                sendMsg(serverQueue, intToString(idOnServer), DISCONNECT);
                sendMsg(chat, line, DISCONNECT);
                isChatRunning = false;
            }
            else {
                sendMsg(chat, line, CONNECT);
            }
        }
    } while (isChatRunning);
}

void sigintHandler(int signal) {
    stopClient();
}


int main(int argc, char ** argv) {
    char *home = getpwuid(getuid())->pw_dir;
    
    key_t serverKey = ftok(home, SERVER_ID);
    serverQueue = msgget(serverKey, 0666);

    key_t clientKey = ftok(home, getpid());
    myQueue = msgget(clientKey, IPC_CREAT | 0777);

    signal(SIGINT, sigintHandler);

    sendMsg(serverQueue, intToString(myQueue), INIT);

    idOnServer = atoi(getMsgOfType(myQueue, INIT));

    printf("My id on server: %d", idOnServer);

    struct msqid_ds info;
    char inputLine[100];
    while(fgets(inputLine, sizeof(inputLine), stdin)) {
        if(!strncmp(inputLine, "LIST", strlen("LIST"))) { //starts with
            sendMsg(serverQueue, intToString(idOnServer), LIST);
        }
        else if (!strncmp(inputLine, "CONNECT", strlen("CONNECT"))) {
            (void)strtok(inputLine, " ");
            int secondClient = atoi(strtok(NULL, " "));
            char * msg[MESSAGE_LENGTH];
            sprintf(msg, "%d %d", idOnServer, secondClient);
            sendMsg(serverQueue, msg, CONNECT);
        }
        else if (!strncmp(inputLine, "DISCONNECT", strlen("DISCONNECT"))) {
            sendMsg(serverQueue, intToString(idOnServer), DISCONNECT);
        }
        else if (!strncmp(inputLine, "STOP", strlen("STOP"))) {
            stopClient();
        }
    
        sleep(1);

        //receiving messages
        msgctl(serverQueue, IPC_STAT, &info);
        int messagesNumber = info.msg_qnum;
        while(messagesNumber > 0) {
            msgbuf * answer = getMsg(myQueue);
            msgctl(serverQueue, IPC_STAT, &info);
            messagesNumber = info.msg_qnum;
            
            if(answer->type == STOP) {
                printf("Serves was closed");
                stopClient();
            }
            else if(answer->type == CONNECT) {
                chat = atoi(answer->text);
                startChat();
            }
            else if(answer->type == LIST) {
                printf("List of clients: \n");
                printf("%s\n", answer->text);
            }   
        }
    }
    stopClient();
}