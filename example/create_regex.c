/* vim: set sw=4 sts=4 : */
#include <stdio.h>
#include <string.h>

#define PATRON "[0-9]"
#define PATRON5 "[0-5]"

char *ipregex(char *ipX)
{
    int i = 0, j = 1;
    static char IP[70];

    IP[0] = '^';
    for (i = 0; i < strlen(ipX); i++) {
        if (*(ipX+i) == 'X') {
            if (*(ipX+i-1) == '2')
                strcat(IP, PATRON5);
            else
                strcat(IP, PATRON);
            j += 5;
        }
        else {
            IP[j++] = *(ipX+i);
            IP[j] = '\0';
        }
    }
    IP[j++] = '$';
    IP[j] = '\0';

    return IP;
}

int main(int argc, char *argv[])
{
    char ip[16];

    strcpy(ip, "255.2XX.XX.X1X");
    printf("%s\n", ipregex(ip));

    return 0;
}
