#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define MAX 255
#define SERVER_PORT 45000

int quit = 0;
int real_max = 230;

char user[12];
int sock_flag;

int fd_r;
int fd_w;

char r_filename[MAX] = "example1.txt";
char w_filename[MAX] = "example2.txt";

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

// recv thread
void* recv_thread(void *arg);

// user selection
int get_options();

void send_message();
void upload_file();
void download_file();
void check_file_list();
void close_connection();

// message parser
void make_send_msg(char* buf, char* msg_type, int msg_end, char* data);
void make_recv_msg(struct Message* recv_msg, char* msg);

// in recv_thread
void uploading();
void downloading_responce_to_server();

// related file upload/download
int open_read(char* filename);
int open_write(char* filename);
void close_read(int fd);
void close_write(int fd);
void read_file(int fd);
int write_file(int fd, char* buf);

int main(int argc, char *argv[])
{
    // get user info
    if(argc < 2)
    {
        printf("less argv\n");
        return 0;
    }

    strcpy(user, argv[1]);
    printf("\033[1;32muser name: %s\n\033[0m", user);

    // socket creation
    struct sockaddr_in server_addr;

    if((sock_flag = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
    {
        printf("\033[1;32mSocket Creation Failed\n\033[0m");
        exit(0);
    }
    else
        printf("\033[1;32mSocket Creation Success\n\033[0m");

    // connection
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    if((connect(sock_flag, (struct sockaddr*)&server_addr, sizeof(server_addr)))<0)
    {
        printf("\033[1;32mServer-Client Connection Failed\n\033[0m");
        exit(0);
    }
    else
        printf("\033[1;32mServer-Client Connection Success\n\033[0m");

    // sub-thread
    pthread_t thread;
    pthread_create(&thread, NULL, recv_thread, NULL);

    // main thread
    while(!quit)
    {
        int selection = get_options();

        switch(selection)
        {
            case SEND_MESSAGE:
                    send_message();
                    break;
            case UPLOAD_FILE:
                    upload_file();
                    break;
            case DOWNLOAD_FILE:
                    download_file();
                    break;
            case CHECK_FILE_LIST:
                    check_file_list();
                    break;
            case CLOSE_CONNECTION:
                    close_connection();
                    break;
        }

        sleep(1);
    }

    printf("\033[1;32m Close connection \n\033[0m");
    exit(0);

    close(sock_flag);

    return 0;
}


void* recv_thread(void* arg)
{
    printf("\033[1;36mstart recv_thread\033[0m\n");
    while(!quit)
    {
        char buf[MAX];
        memset(buf, 0x00, MAX);

        struct Message msg;

        read(sock_flag, buf, sizeof(buf));

        sleep(0.001);

        if(strlen(buf)<1)
            continue;

        puts(buf);

        make_recv_msg(&msg, buf);

        printf("user: %s, type: %s, len: %d, end: %4s, data: %s\n", msg.user, msg.msg_type, msg.data_len, msg.msg_end, msg.data);

        // recv message
        if(!strncmp(msg.msg_type, "0x11", 4))
        {
            printf("\033[1;36muser:%s, msg:%s\n\033[0m", msg.data);
        }
        
        // upload request
        else if(!strncmp(msg.msg_type, "0x21", 4))
        {
            // ok
            if(!strncmp(msg.msg_end, "0x00", 4))
            {
                uploading();
            }

            // no
            else if(!strncmp(msg.msg_end, "0x01", 4))
            {
                printf("\033[1;36mSever denied uploading data\n\033[0m");
            }
        }

        // uploading result
        else if(!strncmp(msg.msg_type, "0x23", 4))
        {
            printf("\033[1;36muser:%s, msg:%s\n\033[0m", msg.data);
        }

        // downloading response
        else if(!strncmp(msg.msg_type, "0x31", 4))
        {
            // ok
            if(!strncmp(msg.msg_end, "0x00", 4))
            {
                fd_w = open_write(w_filename);

                if(fd_w != -1)
                    printf("\033[1;36mopen %s\n\033[0m", w_filename);
            }

            // no
            else if(!strncmp(msg.msg_end, "0x01", 4))
            {
                printf("\033[1;36mServer denied downloading data\n\033[0m");
            }
        }

        // downloading data
        else if(!strncmp(msg.msg_type, "0x32", 4))
        {
            write_file(fd_w, msg.data);

            // last one
            if(!strncmp(msg.msg_end, "0x00", 4))
            {
                downloading_responce_to_server();
                close_write(fd_w);

                printf("\033[1;36mSuccess Downloading\n\033[0m");
            }
        }

        // file list
        else if(!strncmp(msg.msg_type, "0x41", 4))
        {
            printf("\033[1;36mmsg->msg_end\033[0m");
        }

        // close connection response
        else if(!strncmp(msg.msg_type, "0x51", 4))
        {
            if(!strncmp(msg.data, "0x00", 4))
            {
                quit = 1;
                printf("\033[1;36mClose the connection\n\033[0m");

                break;
            }
        }

        else
        {
            printf("\033[1;36m select error : %s \n\033[0m", msg.msg_type);
        }
        sleep(0.001);
    }

    pthread_exit((void*)0);
}

// user selection
int get_options()
{
    int option;
    do
    {
        printf("\033[1;32m\n1. sending message\n2. uploadnig file\n3. downloading file\n4. checking uploaded file list\n5. close connection\nselect> \033[0m");
        scanf("%d", &option);
    }while(1>option||option>5);

    return option;
}

void send_message()
{
    char* msg_type = "0x10";

    char buf[MAX];
    memset(buf, 0x00, MAX);
    printf("\033[1;32mEnter the message: \033[0m\n");
    scanf("%*c");
    fgets(buf, MAX-1, stdin);

    int length = strlen(buf);
    buf[length-1] = '\0';
    puts(buf);

    char message[MAX];
    memset(message, 0x00, MAX);
    make_send_msg(message, msg_type, 0, buf);

    write(sock_flag, message, sizeof(message));
    printf("message: %s\n", message);
}

void upload_file()
{
    char* msg_type = "0x20";

    printf("\033[1;32mEnter the filename: \033[0m\n");
    fgets(r_filename, MAX-1, stdin);

    int length = strlen(r_filename);
    r_filename[length-1] = '\0';
    puts(r_filename);

    char message[MAX];
    memset(message, 0x00, MAX);
    make_send_msg(message, msg_type, 0, r_filename);

    write(sock_flag, message, sizeof(message));
    printf("message: %s\n", message);
}

void download_file()
{
    char* msg_type = "0x30";

    printf("\033[1;32mEnter the filename: \033[0m\n");
    fgets(w_filename, MAX-1, stdin);

    int length = strlen(w_filename);
    w_filename[length-1] = '\0';
    puts(w_filename);

    char message[MAX];
    memset(message, 0x00, MAX);
    make_send_msg(message, msg_type, 0, w_filename);

    write(sock_flag, message, sizeof(message));
    printf("%s\n", message);
}

void check_file_list()
{
    char* msg_type = "0x40";
    char* msg_end = "0x00";

    puts(msg_end);

    char message[MAX];
    memset(message, 0x00, MAX);
    make_send_msg(message, msg_type, 0, msg_end);

    write(sock_flag, message, sizeof(message));
    printf("message: %s\n", message);
}
void close_connection()
{
    char* msg_type = "0x50";
    char* msg = "EXIT";
    puts(msg);

    char message[MAX];
    memset(message, 0x00, MAX);
    make_send_msg(message, msg_type, 0, msg);

    write(sock_flag, message, sizeof(message));
    printf("message: %s\n", message);
}

// message parser
void make_send_msg(char* buf, char* msg_type, int msg_end, char* data)
{
    int data_len = strlen(data);

    // the condition must be changed
    char* msg_end_c;
    if (msg_end)
        sprintf(msg_end_c, "0x01");
    else
        sprintf(msg_end_c, "0x00");

    sprintf(buf, "%s|%s|%d|%s|%s", user, msg_type, data_len, msg_end_c, data);
}

void make_recv_msg(struct Message* recv_msg, char* msg)
{
    char *ptr = strtok(msg, "|");
    strcpy(recv_msg->user, ptr);
    printf("user: %s\n", ptr);

    ptr = strtok(NULL, "|");
    strcpy(recv_msg->msg_type, ptr);
    printf("msg_type: %s\n", ptr);

    ptr = strtok(NULL, "|");
    recv_msg->data_len = atoi(ptr);
    printf("data_len: %s\n", ptr);

    ptr = strtok(NULL, "|");
    strcpy(recv_msg->msg_end, ptr);
    printf("msg_end: %s\n", ptr);

    ptr = strtok(NULL, "|");
    strcpy(recv_msg->data, ptr);
    printf("data: %s\n", ptr);
}
// in recv_thread
void uploading()
{
    fd_r = open_read(r_filename);
    read_file(fd_r);
    close_read(fd_r);
}
void downloading_responce_to_server()
{
    char* msg_type = "0x33";

    char* msg_end = "0x00";

    char message[MAX];
    memset(message, 0x00, MAX);
    make_send_msg(message, msg_type, 0, msg_end);

    write(sock_flag, message, sizeof(message));
    printf("%s\n", message);
}

// related file upload/download
int open_read(char* filename)
{
    int fd;
    if((fd = open(filename, O_RDONLY)) == -1)
    {
        printf("\033[1;32m%s open 중 오류 발생\n\033[0m", filename);
        return -1;
    }

    return fd;

}
int open_write(char* filename)
{
    int fd;
    if((fd = open(filename, O_WRONLY |  O_CREAT | O_TRUNC)) == -1)
    {
        printf("\033[1;32m%s open 중 오류 발생\n\033[0m", filename);
        return -1;
    }

    return fd;
}
void close_read(int fd)
{
    close(fd);
}
void close_write(int fd)
{
    close(fd);
}
void read_file(int fd)
{
    char* msg_type = "0x22";
    char buf[real_max];
    char msg[MAX];

    int len;
    while(1)
    {
        memset(buf, 0x00, real_max);
        memset(msg, 0x00, MAX);
        len = read(fd, buf, real_max);
        if(len == 0)
            break;

        else if(len==real_max)
            make_send_msg(msg, msg_type, 1, buf);
        else
            make_send_msg(msg, msg_type, 0, buf);
        
        write(sock_flag, msg, sizeof(msg));
        printf("%s\n", msg);
    }
}
int write_file(int fd, char* buf)
{
    if(write(fd, buf, strlen(buf)) == -1)
    {
        return -1;
    }
    return 0;
}