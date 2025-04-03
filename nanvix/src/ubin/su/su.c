/*
 * Copyright(C) 2011-2016 Maxime, Maxence & Erwan 
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main (int argc, char ** argv) {
    if (argc < 2) {
        printf("USAGE : su [euid : uid_t]\n");
        return 0;
    }
    printf("Changing euid from %d to %d\n", geteuid(), ((uid_t)*argv[1])-'0');
    seteuid(((uid_t)*argv[1])-'0');
    printf("Your euid : %d\n", geteuid());
    printf("Your uid : %d\n", getuid());
    return 0;
}