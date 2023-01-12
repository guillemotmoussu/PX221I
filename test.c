#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h> // Only for time()
#include <unistd.h> // Only for sleep()

int main(int argc, char const *argv[])
{
    char *tab = "Electronique numerique";
    for(int i=0;i<7;i++)
    {
        printf("%s\n",tab[i]);
    }
    return 0;
}
