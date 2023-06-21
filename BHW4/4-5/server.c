#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define MAX_CLNT 256
#define MONITOR_MSG "MONITOR_CONNECT"

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
int monitor_sock = -1;
pthread_mutex_t monitor_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutx;

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");

    while (1) {
        clnt_adr_sz = sizeof(clnt_adr);
        char message[BUF_SIZE];
        int str_len = recvfrom(serv_sock, message, BUF_SIZE - 1, 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        message[str_len] = 0;
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));

        clnt_sock = socket(PF_INET, SOCK_DGRAM, 0);
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);

        sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
    }

    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg) {
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];
    int is_monitor = 0;

    while (1) {
        struct sockaddr_in clnt_adr;
        socklen_t clnt_adr_sz = sizeof(clnt_adr);
        str_len = recvfrom(clnt_sock, msg, sizeof(msg) - 1, 0, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        msg[str_len] = 0;

        if (strcmp(msg, MONITOR_MSG) == 0) {
            is_monitor = 1;
            pthread_mutex_lock(&monitor_mutex);
            monitor_sock = clnt_sock;
            pthread_mutex_unlock(&monitor_mutex);
            continue;
        }

        if (is_monitor) {
            // send detailed information to monitor
            sprintf(msg, "Detailed information...");
            sendto(clnt_sock, msg, strlen(msg), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
        } else {
            // generate a random grade and send it to student
            srand(time(NULL));
            int grade = rand() % 5 + 1; // generate a random grade between 1 and 5
            sprintf(msg, "Your grade is %d", grade);
            sendto(clnt_sock, msg, strlen(msg), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);

            // send update to monitor
            pthread_mutex_lock(&monitor_mutex);
            if (monitor_sock != -1) {
                sprintf(msg, "Generated grade for student: %d", grade);
                sendto(monitor_sock, msg, strlen(msg), 0, (struct sockaddr *)&clnt_adr, clnt_adr_sz);
            }
            pthread_mutex_unlock(&monitor_mutex);
        }
    }

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++) {
        if (clnt_sock == clnt_socks[i]) {
            while (i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char *msg, int len) {
    int i;
    pthread_mutex_lock(&mutx);
    for (i = 0; i < clnt_cnt; i++)
        sendto(clnt_socks[i], msg, len, 0, NULL, 0);
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
