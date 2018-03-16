#ifndef PARSE_H
#define PARSE_H

/**
 * Parses a string of characters ignoring whitespace looking for the next character
 *
 */
int parse_c(char *str, char *c);

/**
 * Parses a string of characters ignoring whitespace looking for a number
 *
 * Stores the number in `i` and returns the offset to after the number in the string
 */
int parse_ui32(char *str, int *i);

int parse_str(char *str, char *buf, int buf_len);

#endif
