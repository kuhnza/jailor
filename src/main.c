#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

void usage() 
{
    printf("\nUsage: jailor [OPTION]... /path/to/program [ARGS...]\n");
    printf("\n");
    printf("  Options:\n");
    printf("  -u \tThe user to run the program as. Defaults to `nobody`.\n");
    printf("  -j \tPath to your chroot jail directory. Defaults to `/var/jail`.\n");
    exit(1);
}

int main(int argc, char **argv) 
{
    int c, i;
    char *prog, *jail_dir = NULL, *user = NULL;
    char **params;
    struct passwd *p;

    /* Check that we have root privileges. We'll need them to chroot later on. */
    if (setuid(0)) {
        perror("setuid");
        usage();
    }

    /* Perform some argument parsing. */
    while ((c = getopt (argc, argv, "u:j:")) != -1) {
        switch (c) {
            case 'u':
                user = optarg;
                break;
            case 'j':
                jail_dir = optarg;                
                break;
            case '?':
                if (optopt == 'u' || optopt == 'j') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                break;
            default:
                usage();
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Missing required argument `program`");
        usage();
    } 

    /* Default user to `nobody` if -u argument not provided. */ 
    if (user == NULL) {
        user = "nobody";
    }

    /* Get the UID of the specified account. */
    if ((p = getpwnam(user)) == NULL) {
        perror("getpwname");
        usage();
    }

    /* Default jail directory to `/var/jail` if -j argument no provided. */
    if (jail_dir == NULL) {
        jail_dir = "/var/jail";
    }

    /* Lower privileges before launching. */
    chdir(jail_dir);
    if (chroot(jail_dir) != 0) {
        perror("chroot");
        usage();
    }
    setgid(p->pw_gid);
    setuid(p->pw_uid);

    /* Launch process inside our jail using remaining non-argument options. */
    prog = argv[optind];    
    params = &argv[optind];        
    return execvp(prog, params);
}
