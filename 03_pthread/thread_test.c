#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define MAX 255
#define SERVER_PORT 45000

// sending 기능에서 recv 기능을 사용하면 0 / 그렇지 않으면 1
int RECV_MODE = 1;

int quit = 0;

enum MSG_TYPE {
        SEND_MESSAGE = 1,
        UPLOAD_FILE,
        DOWNLOAD_FILE,
        CHECK_FILE_LIST,
        CLOSE_CONNECTION
};

struct Message {
    char user[12];
    char msg_type[4];
    int data_len;
    char msg_end[4];
    char data[MAX];
};

int get_options();

void send_message(int sock_flag, char* user);
void upload_file(int sock_flag, char* user);
void download_file(int sock_flag, char* user);
void check_file_list(int sock_flag, char* user);
void close_connection(int sock_flag, char* user);

void make_send_msg(char* buf, char* user, char* msg_type, char* data);
void make_recv_msg(struct Message* recv_msg, char* msg);

void* recv_thread(void* data)
{
    char buf[MAX];
    struct Message msg;

    int sock_flag = *(int *)data;

    printf("start recv_thread\n");
    while(!quit)
    {
        // main_thread에서 send하는 경우 recv
        while(!quit&&RECV_MODE)
        {
            memset(buf, 0x00, MAX);
            read(sock_flag, buf, sizeof(buf));
            make_recv_msg(*msg, buf);

            sleep(1);
        }

        // main_thread에서 recv하는 경우 대기
        while(!quit&&!RECV_MODE)
        {
            sleep(1);
        }

        sleep(1);
    }
}

int main(int argc, char *argv[])
{
        // get user info
        if(argc < 2)
        {
                printf("less argv");
                return 0;
        }
        char user[12];
        strcpy(user, argv[1]);
        printf("user name: %s\n", user);
        // socket creation
        int sock_flag;
        struct sockaddr_in server_addr;

        if((sock_flag = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
        {
                printf("Socket Creation Failed\n");
                exit(0);
        }
        else
                printf("Socket Creation Success\n");

        // connection
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        server_addr.sin_port = htons(SERVER_PORT);

        if((connect(sock_flag, (struct sockaddr*)&server_addr, sizeof(server_addr)))<0)
        {
                printf("Server-Client Connection Failed\n");
                exit(0);
        }
        else
                printf("Server-Client Connection Success\n");

        // sub-thread
        pthread_t thread;
        pthread_create(&thread, NULL, &recv_thread, (void*)sock_flag);

        // main thread
        while(!quit)
        {
                int selection = get_options();

                switch(selection)
                {
                        case SEND_MESSAGE:
                                send_message(sock_flag, user);
                                break;
                        case UPLOAD_FILE:
                                upload_file(sock_flag, user);
                                break;
                        case DOWNLOAD_FILE:
                                download_file(sock_flag, user);
                                break;
                        case CHECK_FILE_LIST:
                                check_file_list(sock_flag, user);
                                break;
                        case CLOSE_CONNECTION:
                                close_connection(sock_flag, user);
                                break;
                }
                break;
        }

        return 0;
}

int get_options()
{
        int option;

        do
        {
                printf("\n1. sending message\n2. uploadnig file\n3. downloading file\n4. checking uploaded file list\n5. close connection\nselect> ");
                scanf("%d", &option);
        }while(1>option||option>5);

        return option;
}

void send_message(int sock_flag, char* user)
{
        const char* msg_type = "0x10";
        char buf[MAX];
        memset(buf, 0x00, MAX);
        printf("Enter the message: ");
        scanf(" %s", buf);

        char message[MAX];
        memset(message, 0x00, MAX);
        make_send_msg(message, user, msg_type, buf);

        write(sock_flag, message, sizeof(message));
}

void upload_file(int sock_flag, char* user){}
void download_file(int sock_flag, char* user){}
void check_file_list(int sock_flag, char* user){}
void close_connection(int sock_flag, char* user)
{
    quit = 1;
}

void make_send_msg(char* buf, char* user, char* msg_type, char* data)
{
        int data_len = strlen(data);
        int message_len_without_data = strlen(user)+strlen(msg_type)+4+4;
        int current_data_len = data_len;
        if( data_len + message_len_without_data > MAX )
        {
        }

        // the condition must be changed
        char msg_end[4];
        if (message_len_without_data + current_data_len >= MAX)
                sprintf(msg_end, "0x01");
        else
                sprintf(msg_end, "0x00");

        sprintf(buf, "%s|%s|%d|%s|%s", user, msg_type, data_len, msg_end, data);
}

void make_recv_msg(struct Message* recv_msg, char* msg)
{
        char *ptr = strtok(msg, "|");
        sprintf(recv_msg->user, "%s", ptr);
        printf("user: %s\n", ptr);

        ptr = strtok(NULL, "|");
        sprintf(recv_msg->msg_type, "%s", ptr);
        printf("msg_type: %s\n", ptr);

        ptr = strtok(NULL, "|");
        recv_msg->data_len = atoi(ptr);
        printf("data_len: %s\n", ptr);

        ptr = strtok(NULL, "|");
        sprintf(recv_msg->msg_end, "%s", ptr);
        printf("msg_end: %s\n", ptr);

        ptr = strtok(NULL, "|");
        sprintf(recv_msg->data, "%s", ptr);
        printf("data: %s\n", ptr);
}