#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[])
{

        char num[12];

        strcpy(num, argv[1]);

        printf("user: %s\n", num);

        return 0;
}