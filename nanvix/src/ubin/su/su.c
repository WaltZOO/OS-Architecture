/*
 * Copyright(C) 2011-2016 Maxime, Maxence & Erwan 
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

#include <nanvix/accounts.h>
#include <nanvix/config.h>
#include <string.h>

uid_t uid;

/**
 * @brief Authenticates a user in the system.
 *
 * @param name     User name.
 * @param password User's password.
 *
 * @returns One if the user has authentication and zero otherwise.
 */
static int authenticate(const char *name, const char *password)
{
    int ret;          /* Return value.    */
    int file;         /* Passwords file.  */
    struct account a; /* Working account. */

    ret = 1;

    /* Open passwords file. */
    if ((file = open("/etc/passwords", O_RDONLY)) == -1)
    {
        fprintf(stderr, "cannot open password file\n");
        return (0);
    }

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
            setgid(a.gid);
            setuid(a.uid);
            goto found;
        }
    }

    ret = 0;
    fprintf(stderr, "\nwrong login or password\n\n");

found:

    uid = a.uid;
    /* House keeping. */
    close(file);

    return (ret);
}

int main (int argc, char ** argv) {
    if (argc < 2) {
        printf("USAGE : su [user]\n");
        return 0;
    }

    char *name = argv[1];
    char password[PASSWORD_MAX];
    fgets(password, PASSWORD_MAX, stdin);

    if (authenticate(name, password)) {
        seteuid(uid);
        printf("Success\nYour new euid is %d\n", geteuid());
        return 0;
    }
    printf("Authentication failed\n");
    return 1;
}