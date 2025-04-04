/*
 * Copyright(C) 2011-2016 Maxime, Maxence & Erwan
 */
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <nanvix/config.h>
#include <nanvix/accounts.h>
#include <string.h>

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

    /* House keeping. */
    close(file);

    return (ret);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: passwd <user>\n");
        return (1);
    }
    char new[PASSWORD_MAX];
    char new2[PASSWORD_MAX];

    char *name = argv[1];
    char password[PASSWORD_MAX];
    fgets(password, PASSWORD_MAX, stdin);

    if (!authenticate(name, password))
    {
        return 1;
    }

    fgets(new, PASSWORD_MAX, stdin);
    fgets(new2, PASSWORD_MAX, stdin);

    if (strcmp(new, new2) != 0)
    {
        printf("passwords do not match\n");
        return 1;
    }

    int file;         /* Passwords file.  */
    struct account a; /* Working account. */

    /* Open passwords file. */
    if ((file = open("/etc/passwords", O_RDONLY)) == -1)
    {
        fprintf(stderr, "cannot open password file\n");
        return 1;
    }

    /* Search in the  passwords file. */
    while (read(file, &a, sizeof(struct account)))
    {
        account_decrypt(a.name, USERNAME_MAX, KERNEL_HASH);

        /* No this user. */
        if (strcmp(name, a.name) != 0 )
            continue;

        strcpy(a.password, new);
        account_encrypt(a.password, PASSWORD_MAX, KERNEL_HASH);

        // changer mot de passe
        lseek(file, -sizeof(a), SEEK_CUR);
        write(file, &a, sizeof(a));
        printf("Password succesfully updated.\n");
        return 0;
    }

    fprintf(stderr, "cannot find user %s\n", name);
    close(file);
    return 1;
}
