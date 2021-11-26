#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 256
#define SERVER_PORT 45000

int main(int argc, char*argv[])
{
        if(argc < 2)
        {
                printf("less argv");
                return 0;
        }

        char num[12];
        strcpy(num, argv[1]);
        printf("user name: %s\n", num);

        int sock_flag;
        struct sockaddr_in server_addr;

        if((sock_flag = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
        {
                printf("Socket Creation Failed\n");
                exit(0);
        }
        else
                printf("Socket Creation Success\n");

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

        char buf[MAX];

        while(1)
        {
                char buf[MAX];
                // read
                memset(buf, 0x00, MAX);
                // printf("Enter the string: ");
                // fgets(buf, MAX, stdin);
                sprintf(buf, "s2018112552|0x10|5|0x00|Hello");
                printf("send %s to server\n", buf);
                write(sock_flag, buf, sizeof(buf));

                if(strcmp("exit\n", buf) == 0)
                {
                        printf("Exit client\n");
                        break;
                }

                sleep(1);

                // write
                memset(buf, 0x00, MAX);
                read(sock_flag, buf, sizeof(buf));
                printf("From Server: %s\n", buf);
                if(strcmp("exit\n", buf) == 0)
                {
                        printf("Exit client caused from Server terminating\n");
                        break;
                }

                sleep(1);

                break;
        }

        close(sock_flag);