#include <stdio.h>
#include <stdlib.h>

/*
 * This function interprets score_card as an array of pointers with size elements.
 * Return the sum of the values pointed to by the elements of score_card.
 */
int sum_card(int **score_card, int size) {
    // TODO: write the body of sum_card according to its description.
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += *score_card[i];
    }
    return sum;
}


/*
 * NOTE: Don't change the main function, except where specified
 * Sample usage:
 * $ make
 * $ ./score_card 10 -3
 * Sum: 7
 */
int main(int argc, char **argv) {
    int size = argc - 1;
    int *score_card[size];

    for (int i = 0; i < size; i++) {
        score_card[i] = malloc(sizeof(int));
        *(score_card[i]) = strtol(argv[i + 1], NULL, 10);
    }

    printf("Sum: %d\n", sum_card(score_card, size));

    /* 
     * TODO: Free allocated memory
     */
    for (int i=0; i < size; i++) {
        free(score_card[i]);
    }

    return 0;
}

