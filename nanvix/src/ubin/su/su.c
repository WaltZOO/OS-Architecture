/*
 * Copyright(C) 2011-2016 Maxime, Maxence & Erwan 
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#include <nanvix/accounts.h>
#include <nanvix/config.h>
#include <string.h>

int main (int argc, char ** argv) {
    if (argc < 2) {
        printf("USAGE : su [user]\n");
        return 0;
    }

    int file;         /* Passwords file.  */
    struct account a; /* Working account. */

    /* Open passwords file. */
    if ((file = open("/etc/passwords", O_RDONLY)) == -1)
    {
        fprintf(stderr, "cannot open password file\n");
        return (0);
    }

    printf("Please enter password: ");

    char *name = argv[1];
    char password[PASSWORD_MAX];

    fgets(password, PASSWORD_MAX, stdin);

    /* Search in the  passwords file. */
    while (read(file, &a, sizeof(struct account)))
    {
        account_decrypt(a.name, USERNAME_MAX, KERNEL_HASH);

        /* No this user. */
        if (strcmp(name, a.name))
            continue;

        account_decrypt(a.password, PASSWORD_MAX, KERNEL_HASH);

        /* Found. */
        if (!strcmp(password, a.password))
        {
            goto found;
        }
    }
    fprintf(stderr, "\nwrong login or password\n\n");
    return 0;

found:

    /* House keeping. */
    close(file);

    printf("Changing euid from %d to %d\n", geteuid(), (int)a.uid);

    // we set the new euid
    seteuid(a.uid);
    return 0;
}