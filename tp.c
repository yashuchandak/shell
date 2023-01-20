#include<stdio.h>
#include<string.h>

int main() {
    printf("a.out me a.out\n");
    char buf[20] = "yash";
    char hh[10] = "hoho";
    strcpy(buf, hh);
    printf("%s", buf);
}