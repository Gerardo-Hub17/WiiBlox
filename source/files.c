#include <stdio.h>
#include <fat.h>

#include "files.h"

int Files_Init(void)
{
    return fatInitDefault();
}

int Files_Test(void)
{
    FILE *file;
    char buffer[64];

    file = fopen("sd:/apps/WiiBlox/data/test.txt", "r");

    if (!file)
        return 0;

    if (!fgets(buffer, sizeof(buffer), file))
    {
        fclose(file);
        return 0;
    }

    fclose(file);

    return 1;
}
