#include "util.h"

void logprint(char *s)
{
    FILE *log;
    log = fopen(LOG, "a");
    time_t t = time(NULL);
    struct tm timeinfo = *localtime(&t);
    fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
                  timeinfo.tm_year+1900,
                  timeinfo.tm_mon+1,
                  timeinfo.tm_mday,
                  timeinfo.tm_hour,
                  timeinfo.tm_min,
                  timeinfo.tm_sec,
                  s);
    fclose(log);
}

void errprint(char *s)
{
    FILE *log;
    log = fopen("./bot.log", "a");
    time_t t = time(NULL);
    struct tm timeinfo = *localtime(&t);
    fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d] ERROR %s\n",
                  timeinfo.tm_year+1900,
                  timeinfo.tm_mon+1,
                  timeinfo.tm_mday,
                  timeinfo.tm_hour,
                  timeinfo.tm_min,
                  timeinfo.tm_sec,
                  s);
    fclose(log);
}

void msglogprint(char *s)
{
    FILE *log;
    log = fopen(MSGLOG, "a");
    time_t t = time(NULL);
    struct tm timeinfo = *localtime(&t);
    fprintf(log, "[%04d-%02d-%02d %02d:%02d:%02d] %s",
                     timeinfo.tm_year+1900,
                     timeinfo.tm_mon+1,
                     timeinfo.tm_mday,
                     timeinfo.tm_hour,
                     timeinfo.tm_min,
                     timeinfo.tm_sec,
                     s);
    fclose(log);
}

int strfind(char *s1, char *s2)
{
    int i;

    #ifdef DEBUG
    logprint("start strfind()");
    #endif

    for (i=0; i<strnlen(s1, MAX); i++) {
        if (strncmp(s1+i, s2, strnlen(s2, MAX)) == 0) {
            #ifdef DEBUG
            logprint("string found");
            logprint("end strfind()");
            #endif
            return 1; /* s2 in s1 */
        }
    }

    #ifdef DEBUG
    logprint("string not found");
    logprint("end strfind()");
    #endif

    return 0; /* s2 not in s1 */
}

int getmsg(char *s, char *key)
{
    int i;
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start getmsg()");
    #endif

    /*strcpy(tmp, key);*/
    snprintf(tmp, MAX, "%s", key);
    /*strcat(tmp, " :");*/

    for (i=0; i<strnlen(s, MAX); i++) {
        if (strncmp(s+i, tmp, strnlen(tmp, MAX)) == 0) {
            /*strcpy(s, s+i+strlen(tmp));*/
            snprintf(s, MAX, "%s", s+i+strnlen(tmp, MAX));
            #ifdef DEBUG
            logprint("end getmsg()");
            #endif
            return 0;
        }
    }
    return -1;
}

int usernamecount(char *s)
{
    int i;

    #ifdef DEBUG
    logprint("start usernamecount()");
    #endif

    for (i=2; i<strnlen(s, MAX); i++) {
        if (strncmp(s+i, "!", strnlen("!", MAX)) == 0) {
            #ifdef DEBUG
            logprint("end usernamecount()");
            #endif
            return i-1;
        }
    }
    return -1;
}

int checkPass(char *serverline)
{
    FILE *fd;
    char line[MAX];
    int c;
    int i = 0;

    #ifdef DEBUG
    logprint("start checkPass()");
    #endif

    memset(line, 0, MAX);

    /* open file */
    fd = fopen("passphrase", "r");
    if (fd == NULL) {
        #ifdef DEBUG
        logprint("no passphrase file");
        logprint("end checkPass()");
        #endif
        return 0; /* no passphrase file -> deny */
    }

    while ((c = getc(fd)) != EOF) {
        if (c == '\n') {
            break;
        } else {
            line[i] = c;
            i++;
        }
    }
    #ifdef DEBUG
    logprint("pass: ");
    logprint(line);
    #endif

    /* close file */
    fclose(fd);

    #ifdef DEBUG
    logprint("end checkPass()");
    #endif

    return strfind(serverline, line);
}

int osinfo(char *info)
{
    struct utsname name;

    #ifdef DEBUG
    logprint("start osinfo()");
    #endif

    if (uname(&name) == -1) {
        errprint("uname()");
        return -1;
    }
    /*strcpy(info, "System: ");*/
    snprintf(info, MAX, "%s", "System: ");
    strncat(info, name.sysname, MAX-strnlen(info, MAX)-1);
    strncat(info, ", release: ", MAX-strnlen(info, MAX)-1);
    strncat(info, name.release, MAX-strnlen(info, MAX)-1);
    strncat(info, ", architecture: ", MAX-strnlen(info, MAX)-1);
    strncat(info, name.machine, MAX-strnlen(info, MAX)-1);

    #ifdef DEBUG
    logprint("end osinfo()");
    #endif

    return 0;
}

int getrand(int a)
{
    int fd;
    int random;
    FILE *log;

    #ifdef DEBUG
    logprint("start getrand()");
    #endif

    fd = open("/dev/urandom", O_RDONLY);
    read(fd, &random, sizeof(int));
    close(fd);
    random = abs(random) % a;
    
    /* compile with -lrt
    int i, random;
    struct timespec t;

    clock_gettime(CLOCK_REALTIME, &t);
    random = t.tv_nsec % a;
    */

    #ifdef DEBUG
    log = fopen(LOG, "a");
    fprintf(log, "random number generated: %d\n", random);
    fclose(log);
    #endif

    #ifdef DEBUG
    logprint("end getrand()");
    #endif

    return random;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    #ifdef DEBUG
    logprint("start readline()");
    #endif

    ptr = vptr;

    for (n=1; n < (ssize_t)maxlen; n++) {

        if((rc=read(fd, &c, 1)) == 1) {
            *ptr++ = c;

            if (c == '\n') {
                break;
            }

        } else if (rc == 0) {
            if (n == 1) {
                return (0);
            } else {
                break;
            }
        } else if (errno != EINTR) {
            return -1;
        }
    }
    *ptr = '\0';

    #ifdef DEBUG
    logprint("end readline()");
    #endif

    return n;
}
