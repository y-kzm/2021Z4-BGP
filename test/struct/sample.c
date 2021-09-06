#include <stdio.h>

struct hoge{
    int size;
    int a[64];
} __attribute__((__packed__));

int main()
{
    struct hoge fuga;
    int i;    

    fuga.size = 3;
    for(i = 0; i < fuga.size; i ++){
        fuga.a[i] = i;
        printf("%d\n", fuga.a[i]);
    }
    printf("\n%d\n", sizeof(fuga));    

    return 0;
}
