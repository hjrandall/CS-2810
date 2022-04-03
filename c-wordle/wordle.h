#pragma once

#include <stdbool.h>

#define WORD_LIST_FILENAME "words.txt"
#define EXACT_HIT_POINTS 5
#define PARTIAL_HIT_POINTS 1
#define RECOMMENDATION_COUNT 20

enum feedback { MISS, EXACT_HIT, PARTIAL_HIT };
typedef struct {
    char letters[6];
    enum feedback feedback[5];
} guess;

char **read_word_list(char *filename);
void free_word_list(char **list);
guess parse_guess(char *line);
bool is_viable_candidate(char *candidate, guess *guesses, int guess_count);
int score(char **word_list, char *candidate, guess *guesses, int guess_count);
void recommend(char **word_list, guess *guesses, int guess_count);
