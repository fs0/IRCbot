#include "util.h"

void logprint(char *s)
{
    FILE *log;
    log = fopen("./bot.log", "a");
    fprintf(log, "%s", s);
    fclose(log);
}

void errprint(char *s)
{
    FILE *log;
    log = fopen("./bot.log", "a");
    fprintf(log, "ERROR %s", s);
    fclose(log);
}

int strfind(char *s1, char *s2)
{
    int i;

    #ifdef DEBUG
    logprint("start strfind()\n");
    #endif

    for (i=0; i<strnlen(s1, MAX); i++) {
        if (strncmp(s1+i, s2, strnlen(s2, MAX)) == 0) {
            #ifdef DEBUG
            logprint("end strfind()\n");
            #endif
            return 0;
        }
    }

    #ifdef DEBUG
    logprint("end strfind()\n");
    #endif

    return -1;
}

int strend(char *s0, char *s1)
{
    #ifdef DEBUG
    logprint("start strend()\n");
    #endif
    if (strncmp(s0+strnlen(s0, MAX)-3, s1, strnlen(s1, MAX)) == 0) {
        #ifdef DEBUG
        logprint("end strend()\n");
        #endif
        return 0;
    }
    #ifdef DEBUG
    logprint("end strend()\n");
    #endif
    return -1;
}

int getmsg(char *s, char *key)
{
    int i;
    char tmp[MAX];

    #ifdef DEBUG
    logprint("start getmsg()\n");
    #endif

    /*strcpy(tmp, key);*/
    snprintf(tmp, MAX, "%s", key);
    /*strcat(tmp, " :");*/

    for (i=0; i<strnlen(s, MAX); i++) {
        if (strncmp(s+i, tmp, strnlen(tmp, MAX)) == 0) {
            /*strcpy(s, s+i+strlen(tmp));*/
            snprintf(s, MAX, "%s", s+i+strnlen(tmp, MAX));
            #ifdef DEBUG
            logprint("end getmsg()\n");
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
    logprint("start usernamecount()\n");
    #endif

    for (i=2; i<strnlen(s, MAX); i++) {
        if (strncmp(s+i, "!", strnlen("!", MAX)) == 0) {
            #ifdef DEBUG
            logprint("end usernamecount()\n");
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

    memset(line, 0, MAX);

    // open file
    fd = fopen("passphrase", "r");
    if (fd == NULL)
        return -1;

    while ((c = getc(fd)) != EOF) {
        if (c == '\n') {
            break;
        } else {
            line[i] = c;
            i++;
        }
    }

    // close file
    fclose(fd);

    return strfind(serverline, line);
}

// TODO improve
// TODO look for ": " and " :"
int yesnoq(char *s, char *nick)
{
    char tmp[MAX];
    char tmp2[MAX];

    #ifdef DEBUG
    logprint("start yesnoq()\n");
    #endif

    /*strcpy(tmp, nick);*/
    snprintf(tmp, MAX, "%s", nick);
    strncat(tmp, ": ", MAX-strnlen(tmp, MAX)-1);

    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "Why", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "why", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "What", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "what", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "Who", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "who", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }

    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "When", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "when", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "Where", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "where", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "How", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }
        
    /*strcpy(tmp2, tmp);*/
    snprintf(tmp2, MAX, "%s", tmp);
    strncat(tmp2, "how", MAX-strnlen(tmp2, MAX)-1);
    if (strfind(s, tmp2) == 0) {
        return -1;
    }

    #ifdef DEBUG
    logprint("end yesnoq()\n");
    #endif

    return 0;
}

int osinfo(char *info)
{
    struct utsname name;

    #ifdef DEBUG
    logprint("start osinfo()\n");
    #endif

    if (uname(&name) == -1) {
        errprint("uname()\n");
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
    logprint("end osinfo()\n");
    #endif

    return 0;
}

int getrand(int a)
{
    int fd;
    int random;
    FILE *log;

    #ifdef DEBUG
    logprint("start getrand()\n");
    #endif

    fd = open("/dev/urandom", O_RDONLY);
    read(fd, &random, sizeof(int));
    close(fd);
    random = abs(random) % a;
    
    /*
    // compile with -lrt
    int i, random;
    struct timespec t;

    clock_gettime(CLOCK_REALTIME, &t);
    random = t.tv_nsec % a;
    */

    #ifdef DEBUG
    log = fopen("./bot.log", "a");
    fprintf(log, "random number generated: %d\n", random);
    fclose(log);
    #endif

    #ifdef DEBUG
    logprint("end getrand()\n");
    #endif

    return random;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    #ifdef DEBUG
    logprint("start readline()\n");
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
    logprint("end readline()\n");
    #endif

    return n;
}

int getLine(char *line, char *textfile)
{
    FILE *fd;
    int numLines = 0;
    int currentLine = 0;
    int randnum;
    int c;
    int i = 0;

    #ifdef DEBUG
    logprint("start getLine()\n");
    #endif

    // open file
    fd = fopen(textfile, "r");
    if (fd == NULL) {
        return -1;
    }
    // count lines
    while ((c = getc(fd)) != EOF) {
        if (c == '\n') {
            numLines++;
        }
    }
    fseek(fd, 0, SEEK_SET);

    // generate random number
    randnum = getrand(numLines);
    // get the <randnum>th line
    while ((c = getc(fd)) != EOF) {
        if (randnum > currentLine) {
            if (c == '\n') {
                currentLine++;
            }
        } else {
            if (c == '\n') {
                break;
            } else {
                line[i] = c;
                i++;
            }
        }
        
    }

    // close file
    fclose(fd);

    #ifdef DEBUG
    logprint("end getLine()\n");
    #endif

    return 0;
}
