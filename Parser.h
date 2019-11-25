#ifndef PARSER_H
#define PASRSE_H
#include "Game.h"
#include "MainAux.h"

/*	Parse summary:
 *
 *The module contain mainly the "translate" function, the function responsible for reading and
 * interpreting the player's input
 *
 */




/* The function gets the user's arguments via "fgets"
 * The arguments include the name of the function to be executed and the parameters according to the
 * function's type.
 * Than the function compares the 1'th argument's user to functions on Game module
 * and if there is a matching, calling the matching function.
 *
 *  @param  game - Current game instance
 *  @return 1 if the loading succeeded , -1 otherwise.
 */
int translate (Game** game);
/* The function getting a string and checks if there is natural number
 * among the string's chars.
 *  @param  s - A string
 *  @return 1 if there is no digit in the string , 0 otherwise.
 */
int digit_only(const char *s);
/* The function check if we can convert a string to an double between 0.0 to 1.0 safely, aka
 * the first two chars are a "0." or "1." .
 *  @param  s - A string
 *  @return 1 if the char "0." or "1." , 0 otherwise.
*/
int twoFirstDigitis(char *s);
/* The function getting a string and checks if there is digit
 * among the string's chars.
 *  @param  s - A string
 *  @return 1 if there is no digit in the string , 0 otherwise.
 */
int floating_only(const char *s);
/* if the command is not "solve"/"edit"/"save" or guess we need makes sure
 * that there are not digits on the user input so we call "digit_only" function */
int parametersAreCorrect(int copyI,char* arg1,char* arg2,char* arg3);
#endif
