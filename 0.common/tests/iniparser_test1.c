#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "iniparser.h"



void create_default_ini_file(char *ini_name);
int parse_ini_file(char *ini_name);

int main(int argc, char *argv[])
{
    int status;

    if(argc < 2){
        printf("Error: eg:%s test.ini\n\n", argv[0]);
        return -1;
    }
    printf("\n**** ini path : %s ****\n\n", argv[1]);

    status = access(argv[1],F_OK);
    if (status == -1)
    {
        create_default_ini_file(argv[1]);
    }

    status = parse_ini_file(argv[1]);

    return status;
}

void create_default_ini_file(char *ini_name)
{
    FILE *ini;
    assert(ini_name);

    if ((ini = fopen(ini_name, "w")) == NULL)
    {
        fprintf(stderr, "iniparser: cannot create %s\n", ini_name);
        return;
    }

    fprintf(ini,ini_name);

    fflush(ini);
    fsync(fileno(ini));
    fclose(ini);
}

int parse_ini_file(char *ini_name)
{
    dictionary *ini;

    /* Some temporary variables to hold query results */
    int b;
    int i;
    double d;
    const char *s;

    ini = iniparser_load(ini_name);
    if (ini == NULL)
    {
        fprintf(stderr, "cannot parse file: %s\n", ini_name);
        return -1; 
    }
    iniparser_dump(ini, stderr);

    /* Get pizza attributes */
    printf("Pizza:\n");

    b = iniparser_getboolean(ini, "pizza:ham", -1);
    printf("Ham:       [%d]\n", b);
    b = iniparser_getboolean(ini, "pizza:mushrooms", -1);
    printf("Mushrooms: [%d]\n", b);
    b = iniparser_getboolean(ini, "pizza:capres", -1);
    printf("Capres:    [%d]\n", b);
    b = iniparser_getboolean(ini, "pizza:cheese", -1);
    printf("Cheese:    [%d]\n", b);

    /* Get wine attributes */
    printf("Wine:\n");
    s = iniparser_getstring(ini, "wine:grape", NULL);
    printf("Grape:     [%s]\n", s ? s : "UNDEF");

    i = iniparser_getint(ini, "wine:year", -1);
    printf("Year:      [%d]\n", i);

    s = iniparser_getstring(ini, "wine:country", NULL);
    printf("Country:   [%s]\n", s ? s : "UNDEF");

    d = iniparser_getdouble(ini, "wine:alcohol", -1.0);
    printf("Alcohol:   [%g]\n", d);

    iniparser_freedict(ini);
    return 0;
}
