#include <stdio.h>

void print_send_msg(char* msg)
{
    printf("\033[1;33m[send]%s\033[0m", msg);
}

void print_recv_msg(char* msg)
{
    printf("\033[1;36m[recv] %s\033[0m", msg);
}

void print_option_msg(char* msg)
{
    printf("\033[1;32m[recv] %s\033[0m", msg);
}

int main(void)
{
    print_send_msg("hi\n");

    print_recv_msg("recv hi\n");

    return 0;
}