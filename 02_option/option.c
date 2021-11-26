#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX 255

int options = 1;

/*
 * 1 :: sending message
 * 2 :: uploading file
 * 3 :: downloading file
 * 4 :: checking uploaded file list
 * 5 :: close connection
 */

int get_options();

void make_send_msg(char* buf, char* user, char* msg_type, char* data);
void make_recv_msg(char* buf, char* msg);

int main(int argc, char *argv[])
{
        char num[12]               = "s2018112552";

        char sender_example[MAX]   = "s2018112552|0x10|5|0x00|Hello";
        char received_example[MAX] = "s2018112552|0x11|5|0x00|Hello";

        char* msg_type             = "0x10";
        char* given_message        = "Hello";

        // send
        char send_buf[MAX];

        make_send_msg(send_buf, num, msg_type, given_message);

        printf("<<send>>\n");
        printf("example: %s\n", sender_example);
        printf("results: %s\n", send_buf);

        // receive
        char receiver_buf[MAX];

        printf("<<receive>>\n");
        printf("examples: %s\n", received_example);

        make_recv_msg(receiver_buf, received_example);

        printf("results: %s\n", receiver_buf);

        int selection = get_options();
        printf("select %d\n", selection);

        return 0;
}

int get_options()
{
        int option;

        printf("1. sending message\n2. uploadnig file\n3. downloading file\n4. checking uploaded file list\n5. close connection\nselect> ");
        scanf("%d", &option);

        return option;
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

void make_recv_msg(char* buf, char* msg)
{
        char *ptr = strtok(msg, "|");
        printf("user: %s\n", ptr);

        ptr = strtok(NULL, "|");
        printf("msg_type: %s\n", ptr);

        ptr = strtok(NULL, "|");
        printf("data_len: %s\n", ptr);
        
        ptr = strtok(NULL, "|");
        printf("msg_end: %s\n", ptr);

        ptr = strtok(NULL, "|");
        printf("data: %s\n", ptr);

        sprintf(buf, "%s", ptr);
}