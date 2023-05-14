#include <stdio.h>
#include <string.h>

int main()
{
    char str[10005];
    int i;
    scanf("%s%d", str, &i);
    strcpy(str, str + i);
    printf("%s", str);
}