#include <stdio.h>
#include <stdlib.h>

int main(){
    int pointer;
    char value[11];

    while (scanf("%10s %d", value, &pointer) != EOF){
        
        if (pointer == 0){
            printf("%s\n", value);
        } else if (pointer > 10){
            printf("%c\n", value[9]);
        } else{
            printf("%c\n", value[pointer]);
        }
        
    }
    return 0;   
}
