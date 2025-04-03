// pour debug l'euid

#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("my uid : %d \n", getuid());
    return 0;
}