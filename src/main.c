#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

void usage() 
{
    printf("\nUsage: jailor [OPTION]... /path/to/program [ARGS...]\n");
    printf("\n");
    printf("Options:\n");        
    printf("  -d \tChange current working directory before executing program. Defaults to root directory of jail.\n");
    printf("  -j \tPath to your chroot jail directory. Defaults to `/var/jail`.\n");
    printf("  -u \tThe user to run the program as. Defaults to `nobody`.\n");
    exit(1);
}

int main(int argc, char **argv) 
{
    int c, i;
    char *prog, *wd = NULL, *jail_dir = NULL, *user = NULL;
    char **params;
    struct passwd *p;

    /* Check that we have setuid permissions. We'll need them to chroot later on. */
    if (setuid(0)) {        
        perror("setuid");
        fprintf(stderr, "Program must be run as root or have setuid permissions.");
        usage();
    }

    /* Perform some argument parsing. */
    while ((c = getopt (argc, argv, "d:j:u:")) != -1) {
        switch (c) {
            case 'd':
                wd = optarg;
                break;
            case 'j':
                jail_dir = optarg;                
                break;
            case 'u':
                user = optarg;
                break;            
            case '?':
                if (optopt == 'd' || optopt == 'j' || optopt == 'u') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                break;
            default:
                usage();
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Missing required argument `program`.");
        usage();
    } 

    /* Default user to `nobody` if -u argument not provided. */ 
    if (user == NULL) {
        user = "nobody";
    } else {
        if (user == "root") {
            fprintf(stderr, "Running jailed program as root is not permitted.");
            usage();
        }
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

    /* Change the CWD to the jail so we don't end up becoming a gateway 
     * out of the jail. */
    if (chdir(jail_dir) != 0) {
        fprintf(stderr, "chdir: %s: ", jail_dir);
        perror("");
        exit(1);
    }

    /* Enter jail. */
    if (chroot(jail_dir) != 0) {
        fprintf(stderr, "chroot: %s: ", jail_dir);
        perror("");
        exit(1);
    }

    /* Change working directory if -d option supplied. */
    if (wd != NULL) {
        if (chdir(wd) != 0) {
            fprintf(stderr, "chdir: %s: ", wd);
            perror("");
            exit(1);
        }
    }

    /* Lower privileges before launching. */
    setgid(p->pw_gid);
    setuid(p->pw_uid);

    /* Launch process inside our jail using remaining non-argument options. */
    prog = argv[optind];    
    params = &argv[optind];        
    return execvp(prog, params);
}
