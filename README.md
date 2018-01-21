# Logo-Interpreter

This program executes logo style command and prints the output. It executes one commandat a time. It has 4 windows/ workspace, Command window is where User types the command, User can move the turtle in Drawing window, turtle related information gets displayes in Info window and messages after every command execution gets displayed in Message window.
This program handles most errors and displays logo style error messages along with custom error messages.

This program support commands like home, setpencolor number (any number from 1 to 7  i.e. 7 different colors pen), fd,bk [also recognizes forward and back keywords] (turtle can move forward and backward and if the steps to move fd or bk is greater than board size, then for vertical and horizontal moving, turtle will loop back from the other side and for diagonal moving, it will stop at the board border), rt,lt [also recognizes right and left keyword] (turtle can move either 45 or 90 degrees),
pu,pd (to pen up or down i.e. to draw an invisible or visible lines while turtle is moving)

This program takes the input string entered by the User, breaks the string into array of strings, checks if the first word of input is a valid logo command, processes the input to get output, displays appropriate messages and output and logo style plus custom error messages in case of errors.This is a robust program and can handle extreme error cases. It accepts 'quit' command to quit the program.

This program takes every words seperated by a space as a single word and single space as a single word, It executes command on 'enter' key press, it also recognizes consecutive spaces and compresses them to a single space.
During program execution, it stores all the commands entered by the User in an array of string, so that after User 'quit's the program, he has a option to save every commands in a file (assets/logocommand.txt).
 

Enhancement


Additionally thisprogram supports commands like cs, clearscreen, setxy (to setxy position, User can specify the horizontal, vertical or diagonal positions, within the board limits), st,ht (to show and hide turtle), clean (to clean all the lines from drawing window ), print (it support print command to print single word starting with ", also support 'print word' command to concate 2 words - both starting with ", it also supports [] format to display all the words specified inside [])
This program supports 2 numbers arithmatic operations like +,-,*,/,%. 

Algorithm

1) Load a program from file and run it

- enter file line number (file 3) to read specific line from the file and execute the command (default file assets/logocommand.txt)
- take file linenumber as input
- check for file keyword
- breaks the string to array of words
- handle erros i.e. if total words enter are more than 3 i.e. 'file 3 wwe' or linenumber is missing and so on
- open file, read the specified line from the file
- get the string of command from the file, from the specified line number
- break the command in to array of words and paste to original word array (where file 3) was stored
- print the words on the screen and waits for 'enter' key press
- pass the word array to processOutput function
- This function will process the command and display the appropriate result
- since everything is in while loop (until user enters quit), it will again ask the user for a new command


2) implement repeat
- create an enum or array of integer to give numbers to the commands in list of supported commands list
eg [0 means home command, 1 means rt and so on]
- create a stack to keep a track of all the entered commands
- breaks the input string to array of words
- from last to first, check for the match i.e. logo commands and accordingly add the numbers (i.e. command numbers) in the stack,
check for the input and display the input i.e. for pu, it's a one word command but fd is a 2 words command,
so add everything in a stack accordingly
- now commands checking is done in processOutput function where already every commands are given a command number and swicth case is being used
- based on the repeat, use for loop or a while loop parent to switch case and runs the loop repeat-specified number of times
- Now based on the position of logo commands in the stack, execute the commands
- process the output and displays the result

3) iftrue,if/else

- add if, else,truekeywords insupported logo command list

- for if/else, after breaking the input string to array of strings i.e. words,
check for if and else words and store the commands before and after the else / if statement
- basedon the condition specified by if, check and accordingly execute either if or else part
similar for iftrue

Limitations:

1) User can enter maximum 68 alphabets command of maximum 20 words
2) User can enter maximum 100 commands
3) User can enter a single logo command at a time
4) For rt and lt 45 and 90 degree is allowed
5) For setxy user can only move horizontally, vertically or diagonally
6) Arithmetic operations are allowed between 2 numbers
7) maximum number allowed is 999999999999999.99
8) It moves till board border in case of diagonal moving for fd and bk  
