// pour debug l'euid

#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("my euid : %d \n",geteuid());
    return 0;
}