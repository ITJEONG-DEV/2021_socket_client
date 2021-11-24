#include <stdio.h>
#include <sys/socket.h> // socekt 
#include <netinet/in.h> // IPPROTO_TCP, sockaddr_in
#include <arpa/inet.h> // inet_addr
#include <stdlib.h> // implicit declaration of function 'exit'
#include <string.h> // memset( )
#include <unistd.h>
#include <pthread.h> // thread

#define MAX 256
#define SERVER_PORT 8002

struct Data {
    char[12] user;
    int msg_type;
    int data_len;
    int data;
}

// convert
Data data2msg(char[MAX] msg);
char[] msg2data(Data data);
int getNumber(char[] msg);

// send & recv
void SEND_DATA();
void RECV_DATA(int sock_flag);

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("less argv");

        return 0;
    }

    // Parse User Name
    char num[12];
    strcpy(num, argv[1]);
    printf("User name: %s", num);

    int sock_flag;
    struct sockaddr_in server_addr;

    // Socket Creation
    if((sock_flag = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Socket Creation Failed\n");
        exit(0);
    }
    else
        printf("Socket Creation Success \n");

    // Server Address Setting
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    // Server-Client Connection
    if((connect(sock_flag, (struct sockaddr*)&server_addr, sizeof(server_addr)))<0)
    {
        printf("Server-Client Connection Failed\n");
        exit(0)
    }
    else
        printf("Server-Client Connection Success\n");

    // DATA SEND & RECV THREAD
    pthread_t p_thread[2];
    int thr_id;
    int status;
    char t1[] = "DATA SEND";
    char t2[] = "DATA RECV";

    // Thread Creation :: SEND_DATA
    thr_id = pthread_create(&p_thread[0], NULL, SEND_DATA, (void*)t1);

    if(thr_id < 0)
    {
        perror("thread creation error:%s\n", t1);
        exit(0);
    }

    // Thread Creation :: RECV_DATA
    thr_id = pthread_create(&p_thread[1], NULL, RECV_DATA, (void*)t2);

    if(thr_id<0)
    {
        perror("thread creation error:%s\n", t2)
        exit(0);
    }

    // Join Thread
    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);

    // Close Connection
    close(sock_flag);

    return 0;
}

Data data2msg(char[MAX] msg)
{
    Data data;

    char* ptr = msg;

    strcpy(data.user, msg, 8);
}

void SEND_DATA()
{
    char buf[MAX];

    for(;;)
    {
        memset(buf, 0x00, MAX);
        printf("Enter the string: ");
        fgets(buf, MAX, stdin);
        write(sock_flag, buf, sizeof(buf));

        if(strcmp("exit\n", buf) == 0)
        {
            printf("Exit client\n");
            break;
        }

        sleep(1);
    }

}

void RECV_DATA()
{
    char buf[MAX];

    for(;;)
    {
        memset(buf, 0x00, MAX);
        read(sock_flag, buf, sizeof(buf));

        printf("From Server: %s\n", buf);
        if(strcmp("exit\n", buf) == 0)
        {
            printf("Exit client caused from Server terminating\n");
            break;
        }

        sleep(1);
    }
}
