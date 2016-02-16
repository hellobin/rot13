#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 16384

char rot13_char(char c){
    if ((c>='a' && c<='m') || (c>='A' && c<='M') ){
        return c+13;
    }
    else if ((c>'m' && c<='z') || (c>'M' && c<='Z')){
        return c-13;
    }
    else{
        return  c;
    }

}

void child(int fd) {
    char out_buf[MAX_LINE + 1];
    ssize_t outbuf_used = 0;
    ssize_t result;
    while (1) {
        char ch;
        result = recv(fd, &ch, 1, 0);
        if (result == 0) {
            break;
        }
        else if (result == -1){
            perror("read");
            break;
        }
        if (outbuf_used < sizeof(out_buf)){
            out_buf[outbuf_used++] = rot13_char(ch);
        }
        if (ch == '\n'){
            send(fd,out_buf,outbuf_used,0);
            fwrite(out_buf,outbuf_used,1,stdout);
            outbuf_used = 0;
            continue;
        }

    }
}
void  run(void){
    int listener;
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(40713);
    listener = socket(AF_INET,SOCK_STREAM,0);
    if (bind(listener,(struct sockaddr*) &sin, sizeof(sin))<0){
        perror("bind");
        return;
    }
    if (listen(listener,16)<0){
        perror("listen");
        return;
    }
    while (1){
        struct sockaddr_storage ss;
        socklen_t slen = sizeof(ss);
        int fd = accept(listener,(struct sockaddr*) &sin,&slen);
        if (fd<0){
            perror("accept");
            return;
        }
        else{
            if (fork() == 0){        //just fuck for fun
                child(fd);
                exit(0);
            }
        }

    }
}
int main() {
    run();
    return 0;
}