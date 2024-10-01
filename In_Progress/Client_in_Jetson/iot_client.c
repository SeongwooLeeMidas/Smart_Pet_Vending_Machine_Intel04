#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/tcp.h>  // TCP_NODELAY 옵션 사용

#define BUF_SIZE 100
#define NAME_SIZE 20
#define BREED_PORT 7000  // 파이썬으로부터 breed 메시지를 받을 포트

void * send_msg(void * arg);
void * recv_msg(void * arg);
void * receive_breed(void *arg);  // 파이썬으로부터 breed 메시지를 받을 함수
void trigger_python_detection(const char *command);  // 파이썬에게 명령어를 전송
void error_handling(char * msg);

char name[NAME_SIZE]="[Default]";
char msg[BUF_SIZE];
int sock;

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread, breed_thread;
    void * thread_return;
    int flag = 1;

    if(argc != 4) {
        printf("Usage : %s <IP> <port> <name>\n",argv[0]);
        exit(1);
    }

    sprintf(name, "%s",argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");

    // TCP_NODELAY 설정: 작은 패킷도 즉시 전송되도록 설정
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int)) == -1) {
        error_handling("setsockopt() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    sprintf(msg,"[%s:PASSWD]",name);
    write(sock, msg, strlen(msg));

    // 서버로부터 메시지 수신 스레드 시작
    pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);

    // 파이썬에서 breed 메시지 수신 스레드 시작
    pthread_create(&breed_thread, NULL, receive_breed, (void *)&sock);

    // 사용자로부터 메시지 입력 스레드 시작
    pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);

    pthread_join(snd_thread, &thread_return);

    close(sock);
    return 0;
}

void * send_msg(void * arg)
{
    int *sock = (int *)arg;
    int str_len;
    char name_msg[NAME_SIZE + BUF_SIZE+2];

    while(1) {
        memset(msg, 0, sizeof(msg));
        fgets(msg, BUF_SIZE, stdin);

        if(!strncmp(msg, "quit\n", 5)) {
            *sock = -1;
            return NULL;
        }

        sprintf(name_msg, "[ALLMSG]%s", msg);

        // write()가 호출된 후 강제로 데이터를 전송
        if(write(*sock, name_msg, strlen(name_msg)) <= 0) {
            *sock = -1;
            return NULL;
        }

        printf("Message sent to server: %s\n", name_msg);
    }
}

void * recv_msg(void * arg)
{
    int * sock = (int *)arg;
    int str_len;
    char name_msg[NAME_SIZE + BUF_SIZE +1];

    while(1) {
        str_len = read(*sock, name_msg, NAME_SIZE + BUF_SIZE);
        if(str_len <= 0) {
            *sock = -1;
            return NULL;
        }
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);

        // "[KJD_QT]RECO" 명령어를 받으면 파이썬 detection 클라이언트 호출
        if (strstr(name_msg, "[KJD_QT]RECO") != NULL) {
            trigger_python_detection("START");  // 파이썬 스크립트에 START 전송
        }
    }
}


void * receive_breed(void *arg)
{
    int *sock = (int *)arg;
    int breed_sock;
    struct sockaddr_in breed_addr, client_addr;
    socklen_t client_addr_size;
    char breed_msg[BUF_SIZE];
    //char end_signal[] = "\n";  // 종료 신호

    breed_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(breed_sock == -1)
        error_handling("socket() error");

    memset(&breed_addr, 0, sizeof(breed_addr));
    breed_addr.sin_family = AF_INET;
    breed_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    breed_addr.sin_port = htons(BREED_PORT);

    if(bind(breed_sock, (struct sockaddr*)&breed_addr, sizeof(breed_addr)) == -1)
        error_handling("bind() error");

    if(listen(breed_sock, 5) == -1)
        error_handling("listen() error");

    client_addr_size = sizeof(client_addr);

    while(1) {
        int client_sock = accept(breed_sock, (struct sockaddr*)&client_addr, &client_addr_size);
        if(client_sock == -1)
            error_handling("accept() error");

        int str_len = read(client_sock, breed_msg, sizeof(breed_msg) - 1);
        if(str_len > 0) {
            breed_msg[str_len] = '\n';  // 문자열 끝 추가
            printf("Received breed message from Python: %s\n", breed_msg);

            // 품종 메시지를 즉시 서버로 전송
            if(write(*sock, breed_msg, strlen(breed_msg)) <= 0) {
                printf("Failed to send breed message to server\n");
            } else {
                printf("Sent breed message to server: %s\n", breed_msg);
                
                // 종료 신호 전송
                //write(*sock, end_signal, strlen(end_signal));
            }
        }
        close(client_sock);
    }
}



void trigger_python_detection(const char *command) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in detection_server;

    detection_server.sin_family = AF_INET;
    detection_server.sin_addr.s_addr = inet_addr("127.0.0.1");  // detection 클라이언트가 실행 중인 로컬 IP
    detection_server.sin_port = htons(5000);  // detection 클라이언트와 통신할 포트

    if (connect(client_socket, (struct sockaddr*)&detection_server, sizeof(detection_server)) == -1) {
        printf("Failed to connect to detection server\n");
        close(client_socket);
        return;
    }

    write(client_socket, command, strlen(command));  // 명령어 전송
    printf("Sent detection command: %s\n", command);

    close(client_socket);
}

void error_handling(char * msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

