#ifndef TOKEN_SEMI_H
#define TOKEN_SEMI_H

void trimleadingandTrailing(char *s);

int tokenize_semi(char *inp, char output[50][2000]);

int tokenize_pipe(char *inp, char output[20][1000]);

int *tokenize_each(char *inp, char *output[50], char *rdpip[10]);

int tokenize_each_bynewline(char *inp, char *output[21]);

#endif