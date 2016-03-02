#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void child(const char *,struct sockaddr_in *);
int main() {
    const char *server = "127.0.0.1";
    struct sockaddr_in sin;
    memset(&sin, sizeof(sin),0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(40713);
    if (inet_pton(AF_INET,server,&sin.sin_addr)<=0)
    {
        perror("inet_pton");
        return 1;
    }
    FILE *fp = NULL;
    fp = fopen("./send.txt","r");
    if (fp == NULL){
        perror("fopen");
        return 1;
    }
    char *line = NULL;
    size_t  len = 0;
    while(getline(&line,&len,fp) != -1){
        if(fork() == 0){                 //just fuck for fun
            fclose(fp);
            child(line,&sin);
            exit(0);
        }
    }
    free(line);
    fclose(fp);

    return 0;
}

void  child(const char* str ,struct sockaddr_in *sin){
    int sockfd;
    ssize_t remaining;
    ssize_t  len = strlen(str);
    char *buf = (char *)malloc(strlen(str)+1);
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd<0){
        perror("socket");
        free(buf);
        return;
    }
    if (connect(sockfd,(struct sockaddr*) sin, sizeof(*sin))<0) {
        perror("connect");
        free(buf);
        return;
    }
    const char *cp = str;
    remaining = strlen(str);
    while (remaining){
        int n_send = send(sockfd,cp,remaining,0);
        if (n_send<=0) {
            perror("send");
            return;
        }
        remaining -= n_send;
        cp += n_send;
    }
    ssize_t buf_used = 0;
    while (1){
        ssize_t result = recv(sockfd,buf+buf_used,len-buf_used,0);
        if (result == 0) {
            break;
        }
        else if (result <0)
        {
            perror("recv");
            return;
        }
        buf_used += result;

    }
    buf[strlen(str)] = '\0';
    fwrite(buf,strlen(buf),1,stdout);
    free(buf);
    close(sockfd);
}