#include <curses.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*size of different window areas*/
/* workspace 25 * 80*/
#define WORKSPACEY 25
#define WORKSPACEX 80
/* drawing area 16 * 52 */
#define DRAWINGSPACEY 16
#define DRAWINGSPACEX 52
/* command area 6 * 78 */
#define COMMANDSPACEY 6
#define COMMANDSPACEX 78
/* message area 10 * 26 */
#define MESSAGESPACEY 10
#define MESSAGESPACEX 26
/* info space 6 * 26 */
#define INFOSPACEY 6
#define INFOSPACEX 26

/* default Turtle location */
#define TURTLEY (DRAWINGSPACEY / 2) + 1
#define TURTLEX (DRAWINGSPACEX / 2) + 1


typedef struct mainTurtle
{
    int posY;
    int posX;
    int angle;
    int visibility;
    int colorCode;
    int linesVisibility;


} Turtle;

/* This function creates workspace and calls drawBoard function to draw all the boards*/
void createWorkSpace(void);
/* This function draws all 5 boards i.e. game border, message space, command space,
   drawing space, info space */
void drawBoard(int startY, int startX,int sizeY, int sizeX,char boaderY, char boaderX,int colPair, int isBold);
/* This function displays turtle's information in info space
   i.e. turtle's x co-ordinate, y co-ordinate and angle */
void displayInfoInInfoSpace(Turtle* turtle);
/* This function hides turtle's information from info space */
void hideInfoFromInfoSpace(void);
/* This function draws the turtle in drawing space */
void displayTurtleInDrawingSpace(Turtle* turtle);
/* This function hides the turtle from drawing space */
void hideTurtleFromDrawingSpace(Turtle* turtle);
/* This function displays various messages or output after command execution in message space*/
void displayMessageInMessageSpace(int isSuccess, int errorOrSuccessCode, char enteredCommandOrOutput[]);
/* This function hides messages from message space */
void hideMessageFromMessageSpace(void);
/* This function takes error code and based on the code, displays the error message in message space */
void displayErrorMessage(int errorCode,char enteredCommand[]);
/* This function takes success code and based on the code, displays the success message in message space */
void displaySuccessMessage(int successCode, char output[]);
/* This function takes the input string, breaks it to array of the strings, passes it to getOutput function which
   returns the output, then this functions displays the output
   This function also copies all the entered inputs to the array of string, to later on copy it to a file */
void processEnteredCommandInput(char* enteredInput,int count, char** storeOutput,char* allEnteredInputs[],Turtle* turtle);
/* This function fixes the distorted UI of command space and game border, after 'Enter' key is pressed */
void fixUI(void);
/* This function hides the messages and entered input from command space*/
void hideMessageFromCommandSpace(void);
/* This function stores the array of string i.e. user's entered command during the program
   execution in a file*/
char fileOperation(char* allEnteredInputs[], int count);
/* This function takes the array of strings i.e. user entered Input from processEnteredCommandInput function
   Checks if it's a valid command from the command list i.e. home, pu, pd, st, ht, fd, bk, word, print, setxy,
   setpencolor, forward, back, left, right, cs, clean, clearscreen
   and calls processOutput function to get the output */
char* getOutput(char* wordsInput[], int counter, Turtle* turtle);
/* This function takes the user entered input if it has allowed commands and based on the command
   process the output i.e. gets the output, shows the output, call the functions to display the output related
   messages, handles the error condition */
void processOutput(char* ptrOutput, char* wordsInput[], int commandNum, Turtle* turtle, int totalWords);
/* This function cleans the drawing space i.e. removes the
   forward, backward lines by turtle from fd bk setxy commands*/
void cleanDrawingSpace();
/* This function draws the forward and backward lines based on fd and bk command, it also draws diagonal lines if
   Turtle is at 45 or 135 or 225 or 315 degree angle, This function gets called from processOutput function*/
int drawLines(Turtle* turtle,char fwdOrbk, int stepsToMove);

/* This function reads a command from file and stores the command in wordsInput array of string*/
void readCommandFromTheFile(char* wordsInput[], int totalWords);

int main()
{

    Turtle turtle = {TURTLEY,TURTLEX,0,1,5,1};

    char* userEnteredCommand;
    char* userOutput;
    int count = 0, i = 0, cacheCursorPosY = 0, cacheCursorPosX = 0;
    char* allUserEnteredCommands[100];
    char isGivenPermissionToStoreInFile = ' ';


    initscr(); /*start the ncurses mode*/
    cbreak();


    start_color();

    /*color pairs that are used to show colors */
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);


    /*function to draw the work area, drawing area,
      command area, message area, info area */
    createWorkSpace();

    for (i=0 ; i<100; i++) {
        if ((allUserEnteredCommands[i] = malloc(sizeof(char) * 70)) == NULL) {
            displayMessageInMessageSpace(0,5,""); /* To display messages in message space */
            break;
        }
    }

    userEnteredCommand = (char*)malloc(70*sizeof(char));
    userOutput = (char*)malloc(75*sizeof(char));




    getyx(stdscr,cacheCursorPosY,cacheCursorPosX); /*temporary cache current cursor position*/
    do
    {
        getnstr(userEnteredCommand, 68);
        fixUI(); /* fix the distorted UI from 'Enter' */
        /* It takes the input entered by user and process it, output gets stored in userOutput string
           All the commands entered by user during program execution gets stored in allUserEnteredCommands
           array of string, and turtle related information gets stored in turtle
        */
        processEnteredCommandInput(userEnteredCommand,count,&userOutput,allUserEnteredCommands,&turtle);
        count++;
        move(cacheCursorPosY,cacheCursorPosX);
    } while ((strcmp(userEnteredCommand,"quit"))); /* Takes the commands from the user, until user enters 'quit'*/
    /* fileOperation function is to store allUserEnteredCommands array of string to a file*/
    isGivenPermissionToStoreInFile = fileOperation(allUserEnteredCommands,count);

    if (isGivenPermissionToStoreInFile == 'y')
    {
        displayMessageInMessageSpace(1,0,"The file is stored..");

    }

    /*Memory deallocation*/
    for (i=0 ; i<100; i++)
    {
       free(allUserEnteredCommands[i]);

    }
    free(userEnteredCommand);
    free(userOutput);

    getch();
    endwin();
    return 0;

}

/*
This function displays the upper 'LOGO' heading and calls drawBoard function to draw the gameBoard
It does not take any input and does not return anything
*/
void createWorkSpace(void)
{

    attron(COLOR_PAIR(4));
    mvprintw(0,22,"**** ");
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvprintw(0,27," L ");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(5));
    attron(A_BOLD);
    mvprintw(0,30,"O ");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    mvprintw(0,32,"G ");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(5));
    attron(A_BOLD);
    mvprintw(0,34,"O ");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(5));

    attron(COLOR_PAIR(4));
    mvprintw(0,36," ****");
    attroff(COLOR_PAIR(4));


    mvprintw(0,59," Type 'quit' to Quit");
    refresh();

    move(1,0);
    drawBoard(1,0,WORKSPACEY, WORKSPACEX,'|','_',7,0); /*draw work space*/
    refresh();


    move(2,1);
    drawBoard(2,1,DRAWINGSPACEY + 2, DRAWINGSPACEX + 1,'|','-',4,1); /*draw drawing space*/
    refresh();


    move(18,1);
    drawBoard(18,1,COMMANDSPACEY + 18, COMMANDSPACEX + 1,'|','-',1,1); /*draw command space*/
    refresh();


    move(2,53);
    drawBoard(2,53,MESSAGESPACEY + 2, MESSAGESPACEX + 53,'|','-',3,1); /*draw message space*/
    refresh();


    move(12,53);
    drawBoard(12,53,INFOSPACEY + 12, INFOSPACEX + 53,'|','-',6,1); /*draw info space*/
    refresh();


    attron(A_UNDERLINE);
    mvprintw(3,62," Message");
    attroff( A_UNDERLINE);

    mvprintw(19,2," Welcome to Ravina LOGO Version 1.0");
    attron(COLOR_PAIR(4));
    mvprintw(5,55,"Type file 2(any number)");
    mvprintw(6,55,"to read a specific");
    mvprintw(7,55,"command line ");
    mvprintw(8,55,"from file");
    mvprintw(9,55,"assets/logocommand.txt");
    attroff(COLOR_PAIR(4));
    attron(A_BOLD);
    mvaddch(22, 2, '?');
    attroff(A_BOLD);
    refresh();

    move(22,4); /* Location in command box where user will enter the command*/

}

/* This function to displays the work space, drawing space, command space,
   message space, info space
   It takes starting position of Y and X i.e.startY, startX; Horizontal and vertical size of the board
   i.e. sizeY and sizeX; Characters to draw horizontal and vertical borders i.e. boarderY and boarderX;
   Color to draw the boarder i.e. colPair and isBold to make the board boarder bold or not
   It doesn't return anything
*/
void drawBoard(int startY, int startX,int sizeY, int sizeX, char boaderY, char boaderX, int colPair, int isBold)
{
    /*
    Draw the board based on startY, startX to sizeY and sizeX
    */
   int i=startY,j=startX;
   for (i=startY;i<sizeY;i++)
   {

        if ((i==startY)||(i==sizeY-1))/*Draw only to edge*/
        {
            for (j=startX;j<sizeX;j++)
            {
                if (isBold)
                {
                    attron(COLOR_PAIR(colPair));
                    attron(A_BOLD);
                    mvaddch(i, j, boaderX);
                    attroff(A_BOLD);
                    attroff(COLOR_PAIR(colPair));

                }
                else
                {
                    attron(COLOR_PAIR(colPair));
                    mvaddch(i, j, boaderX);
                    attroff(COLOR_PAIR(colPair));

                }

            }

        }
        else
        {
            for (j=startX;j<sizeX;j++)
            {
                if ((j==startX)||(j==sizeX-1)) /*Draw only to edge*/
                {
                    if (isBold)
                    {
                        attron(COLOR_PAIR(colPair));
                        attron(A_BOLD);
                        mvaddch(i, j, boaderY);
                        attroff(A_BOLD);
                        attroff(COLOR_PAIR(colPair));

                    }
                    else
                    {
                        attron(COLOR_PAIR(colPair));
                        mvaddch(i, j, boaderY);
                        attroff(COLOR_PAIR(colPair));

                    }


                }


            }


        }


    }

}

/* This function displays Turtle's X position, Y position and current angle
   in Info Space
   It takes address of turtle struct and does not return anything*/
void displayInfoInInfoSpace(Turtle* turtle)
{

    /*temporary cache current cursor position*/
    int x=0,y=0;
    getyx(stdscr,y,x);

    hideInfoFromInfoSpace(); /*function to hide all the info from info space i.e. clear the box*/
    mvprintw(14,54," Turtle: ");
    attron(COLOR_PAIR(5));
    mvprintw(15,56," (%d, %d, %d degree)",turtle->posX,turtle->posY,turtle->angle);
    attroff(COLOR_PAIR(5));

    refresh();

    move(y,x); /* Move the cursor to the original location*/

}

/* This function clears the info space
   It does not return anything */
void hideInfoFromInfoSpace(void)
{
     int x=0,y=0;
     getyx(stdscr,y,x);

     mvprintw(14,54,"         ");
     mvprintw(15,56,"                      ");
     refresh();
     move(y,x);
}

/* This function draws the turtle in drawing space, it takes address of turtle as input and draws the
   turtle at specific location based on turtle's angle member
   It also draws the character at the location where turtle was standing, if turtle moves to a new location
   it doesn't return anything
*/
void displayTurtleInDrawingSpace(Turtle* turtle)
{

    int x=0,y=0,turtlePosY=0,turtlePosX=0,tempAngle=0, color = 0;

    getyx(stdscr,y,x);

    turtlePosY = turtle->posY;
    turtlePosX = turtle->posX;
    tempAngle = turtle->angle;
    color = turtle->colorCode;


        attron(COLOR_PAIR(color));
        attron(A_BOLD);
      /* Draw the turtle at particular location based on turtle's angle */
       switch (tempAngle)
       {

            case 45:
                {

                    mvaddch(turtlePosY-1, turtlePosX+1, '#');
                    mvaddch(turtlePosY, turtlePosX, '@');


                }
                break;

            case 90:
                {
                    mvaddch(turtlePosY, turtlePosX+1, '@');

                    mvaddch(turtlePosY, turtlePosX, '#');


                }
                break;

            case 135:
                {
                    mvaddch(turtlePosY+1, turtlePosX+1, '@');

                    mvaddch(turtlePosY, turtlePosX, '#');


                }
                break;

            case 180:
                {
                    mvaddch(turtlePosY+1, turtlePosX, '@');

                    mvaddch(turtlePosY, turtlePosX, '#');


                }
                break;

            case 225:
                {
                    mvaddch(turtlePosY+1, turtlePosX-1, '@');

                    mvaddch(turtlePosY, turtlePosX, '#');


                }
                break;

            case 270:
                {
                    mvaddch(turtlePosY, turtlePosX-1, '@');

                    mvaddch(turtlePosY, turtlePosX, '#');


                }
                break;

            case 315:
                {
                    mvaddch(turtlePosY-1, turtlePosX-1, '@');

                    mvaddch(turtlePosY, turtlePosX, '#');


                }
                break;

            case 360:{}

            case 0: /*default 0 degree*/
                {

                mvaddch(turtlePosY-1, turtlePosX, '@');

                mvaddch(turtlePosY, turtlePosX, '#');


                }
                break;

       }

    attroff(A_BOLD);
    attroff(COLOR_PAIR(color));

    turtle->visibility =1;


    refresh();
    move(y,x);
}

/* This function hides turtle from drawing space
   It takes address of turtle as input and doesn't return anything*/
void hideTurtleFromDrawingSpace(Turtle* turtle)
{
    int x=0,y=0,turtlePosY=0,turtlePosX=0,tempAngle=0;
    getyx(stdscr,y,x);

    turtlePosY = turtle->posY;
    turtlePosX = turtle->posX;
    tempAngle = turtle->angle;

       switch (tempAngle)
       {

            case 45:
                {
                    mvaddch(turtlePosY-1, turtlePosX+1, ' ');

                    mvaddch(turtlePosY, turtlePosX, ' ');


                }
                break;

            case 90:
                {
                    mvaddch(turtlePosY, turtlePosX+1, ' ');

                    mvaddch(turtlePosY, turtlePosX, ' ');


                }
                break;

            case 135:
                {
                    mvaddch(turtlePosY+1, turtlePosX+1, ' ');

                    mvaddch(turtlePosY, turtlePosX, ' ');


                }
                break;

            case 180:
                {
                    mvaddch(turtlePosY+1, turtlePosX, ' ');

                    mvaddch(turtlePosY, turtlePosX, ' ');


                }
                break;

            case 225:
                {
                    mvaddch(turtlePosY+1, turtlePosX-1, ' ');

                    mvaddch(turtlePosY, turtlePosX, ' ');


                }
                break;

            case 270:
                {
                    mvaddch(turtlePosY, turtlePosX-1, ' ');

                    mvaddch(turtlePosY, turtlePosX, ' ');


                }
                break;

            case 315:
                {
                    mvaddch(turtlePosY-1, turtlePosX-1, ' ');

                    mvaddch(turtlePosY, turtlePosX, ' ');


                }
                break;

            case 360:{}

            case 0:
                {

                mvaddch(turtlePosY-1, turtlePosX, ' ');

                mvaddch(turtlePosY, turtlePosX, ' ');

                }
                break;

       }

    turtle->visibility =0;
    refresh();
    move(y,x);

}

/* This function displays error or success message in Message space
   If it's an error, it will call displayErrorMessage function and will pass the error code
   to display particular error message
   If it's a success, it will display the success output and call displaySuccessMessage function and
   will pass the success code to display particular success message
   It takes isSuccess to check if it's an error or success i.e. 0-> error and 1->success
   and errorOrSuccessCode, It also takes enteredCommandOrOutput string to display as a message
   It does not return anything
*/
void displayMessageInMessageSpace(int isSuccess, int errorOrSuccessCode, char enteredCommandOrOutput[])
{

    int x=0,y=0;
    getyx(stdscr,y,x);

    hideMessageFromMessageSpace(); /* function to clear message space*/


    if (isSuccess) /* if success */
    {
        attron(COLOR_PAIR(1));
        attron(A_BOLD);
        mvprintw(5,55," Success");
        attroff(A_BOLD);
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(4));
        mvprintw(5,63," :)");
        attroff(COLOR_PAIR(4));

        /*This function displays particular success message based on the code i.e.
        errorOrSuccessCode and it also displays passed message i.e. enteredCommandOrOutput */
        displaySuccessMessage(errorOrSuccessCode, enteredCommandOrOutput);

    }
    else /* if error*/
    {
        attron(COLOR_PAIR(2));
        attron(A_BOLD);
        mvprintw(5,55," Error");
        attroff(A_BOLD);
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(4));
        mvprintw(5,61," :(");
        attroff(COLOR_PAIR(4));
        /*This function displays particular error message based on the code i.e.
        errorOrSuccessCode and it also displays passed message i.e. enteredCommandOrOutput */
        displayErrorMessage(errorOrSuccessCode,enteredCommandOrOutput);
    }


    refresh();
    move(y,x);

}

/* This function clears the message space
  It doesn't take any input and does not return anything */
void hideMessageFromMessageSpace(void)
{

   mvprintw(5,54,"                        ");
   mvprintw(6,54,"                        ");
   mvprintw(7,54,"                        ");
   mvprintw(8,54,"                        ");
   mvprintw(9,54,"                        ");
   mvprintw(10,54,"                       ");

   refresh();
}

/* This function displays error messages as per error code
   It takes errorCode as input and additional string to display i.e. enteredCommand
   It doesn't return anything
*/
void displayErrorMessage(int errorCode, char enteredCommand[])
{

   switch (errorCode)
   {
        case 0:
          break;

        case 1:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"I don't know how to ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19) /* if size of entered command is greater than message space size*/
            {
               mvprintw(8,55," %s","too long..");
            }
            else
            {
               mvprintw(8,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;

        case 2:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Not enough inputs to ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(8,55," %s","too long..");
            }
            else
            {
               mvprintw(8,55," %s",enteredCommand);
            }


            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;

        case 3:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"You don't say what to ");
            mvprintw(8,55,"do with ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(9,55," %s","too long..");
            }
            else
            {
               mvprintw(9,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;

       case 4:
            attron(COLOR_PAIR(7));
            attron(A_BOLD);
            mvprintw(7,55," %s",enteredCommand[0]);
            attroff(A_BOLD);
            mvprintw(8,55,"Doesn't like %s",enteredCommand[1]);
            mvprintw(9,55,"as input");
            attroff(COLOR_PAIR(7));
          break;
       case 5:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Unable to allocate memory ");
            attron(A_BOLD);
            mvprintw(8,55," %s",enteredCommand);
            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;

      case 6:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Entered angle Not");
            mvprintw(8,55,"allowed ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(9,55," %s"," ");
            }
            else
            {
               mvprintw(9,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
     case 7:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Invalid input ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(8,55," %s","too long..");
            }
            else
            {
               mvprintw(8,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;

    case 8:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"No multiple command ");
            mvprintw(8,55,"execution at a time ");
            mvprintw(9,55,"allowed ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(10,55," %s"," ");
            }
            else
            {
               mvprintw(10,55," %s",enteredCommand);
            }


            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;

    case 9:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Please put the words ");
            mvprintw(8,55,"inside [] or start a ");
            mvprintw(9,55,"word with \" ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(10,55," %s"," ");
            }
            else
            {
               mvprintw(10,55," %s",enteredCommand);
            }


            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
    case 10:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Sorry this ");
            mvprintw(8,55,"program only works for ");
            mvprintw(9,55,"2 numbers ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(10,55," %s"," ");
            }
            else
            {
               mvprintw(10,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
     case 11:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Please enter ");
            mvprintw(8,55,"2 numbers and an ");
            mvprintw(9,55,"arithmetic operator ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(10,55," %s"," ");
            }
            else
            {
               mvprintw(10,55," %s",enteredCommand);
            }


            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
    case 12:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Invalid syntax ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(8,55," %s","too long..");
            }
            else
            {
               mvprintw(8,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
    case 13:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Insufficient Input ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(8,55," %s","too long..");
            }
            else
            {
               mvprintw(8,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
    case 14:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Please specify the ");
            mvprintw(8,55,"arithmetic operator ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(9,55," %s"," ");
            }
            else
            {
               mvprintw(9,55," %s",enteredCommand);
            }


            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
    case 15:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Invalid Command ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(8,55," %s","too long..");
            }
            else
            {
               mvprintw(8,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;
    case 16:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"Turtle couldn't move ");
            attron(A_BOLD);
            if (strlen(enteredCommand) > 19)
            {
               mvprintw(8,55," %s","too long..");
            }
            else
            {
               mvprintw(8,55," %s",enteredCommand);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(7));
          break;




   }
   refresh();

}


/*This function displays the success message based on successCode,it takes
  succesCode and additional output message to display as input
  It doesn't return anything

*/
void displaySuccessMessage(int successCode, char output[])
{
    int strSize = 0;
    switch (successCode)
   {
        case 0:
            mvprintw(7,55," %s",output);
          break;

        case 1:

            mvprintw(7,55,"Turtle Moved to..");
            attron(A_BOLD);
            mvprintw(8,55," %s",output);
            attroff(A_BOLD);

          break;

        case 2:

            strSize = strlen(output);
            if (strSize <=13) /* if size of message i.e. string is greater than message box size*/
            {
                mvprintw(7,55,"O\\P: ");
                attron(A_BOLD);
                mvprintw(8,55," %s",output);
                attroff(A_BOLD);
            }
            else
            {
                mvprintw(7,55,"O\\P: ");
                attron(A_BOLD);
                mvprintw(8,55," %s","too long.. :D");
                attroff(A_BOLD);

            }


          break;

        case 3:

            mvprintw(7,55,"Operation Performed ");
            attron(A_BOLD);
            mvprintw(8,55," %s",output);
            attroff(A_BOLD);

          break;

   }

   refresh();

}


/* This function processes the input string entered by the user and displays the output
   It breaks the input string to array of string (of words)
   It passes the array of string along with address of turtle and total words count of input to
   getOutput function which returns the output
   This function also stores all commands entered by the user during program execution in array of string i.e.allEnteredInputs
   This function takes address of enteredInput string, total command's count i.e. count, pointer to address of string to
   store output i.e. storeOutput, address of turtle and address of array of string allEnteredInputs
   This function doesn't return anything

*/
void processEnteredCommandInput(char* enteredInput, int count, char** storeOutput, char* allEnteredInputs[],Turtle* turtle)
{
    int i = 0, wordCounter = 0, j = 0, lengthOfInput = 0, tempCounter = 0;
    char* wordsInInput[20]; /* store maximum 20 words */

    char tempString2[75], tempString[75];
    char tempWord[70];

    hideMessageFromCommandSpace(); /* clear the command space*/
    attron(A_BOLD);
    mvaddch(20, 2, '?');
    attroff(A_BOLD);
    mvprintw(20,4,"%s",enteredInput);


    for (j=0 ; j<20; j++) {
        if ((wordsInInput[j] = malloc(sizeof(char) * 70)) == NULL) {
            displayMessageInMessageSpace(0,5,""); /* display error message*/
            break;
        }
    }


    lengthOfInput = strlen(enteredInput);
    strcpy(tempString2,enteredInput);

    /* compress the multiple occurance of space*/
    for (i=0, j=0; tempString2[i] != '\0'; i++, j++) {
        if (tempString2[i] == ' ' && tempString2[i+1] == ' ') {
            j--;
            continue;
        }
        tempString[j] = tempString2[i];
    }
    tempString[j] = '\0';

    /*split the string i.e. entered user input (enteredInput) in to words*/
    for (i=0;i<lengthOfInput;i++)
    {

        if (isspace(tempString[i])) /* split the string to words based on space delimiter*/
        {
            if (i != 0)
            {
                tempWord[tempCounter] = '\0';

                tempCounter  = 0;
                strcpy(wordsInInput[wordCounter], tempWord);
                wordCounter++;
                strcpy(tempWord, "");

            }


        }
        else
        {
            tempWord[tempCounter] = tolower(tempString[i]); /* convert all the words to lower case*/
            tempCounter++;
            if (i == lengthOfInput-1)
            {
                tempWord[tempCounter] = '\0';
                strcpy(wordsInInput[wordCounter], tempWord);
            }

        }


    }


    /* getOut function takes array of string i.e. wordsInInput,count of total words
      i.e. wordCounter and address of turtle and it returns the output*/
    *storeOutput = getOutput(wordsInInput,wordCounter,turtle);


    attron(A_BOLD);
    mvprintw(21,4,"%s",*storeOutput);
    attroff(A_BOLD);


    refresh();

    /*Memory deallocation*/
    for (j=0 ; j<20; j++)
    {
       free(wordsInInput[j]);

    }


    /*store command in string array to later on store to a file*/
    strcpy(allEnteredInputs[count], enteredInput);


}

/* This function fixes the distorted UI, when user presses 'Enter' key
   It doesn't take any input and doesn't return anything*/
void fixUI(void)
{

    int x=0,y=0;
    getyx(stdscr,y,x);

    drawBoard(1,0,WORKSPACEY, WORKSPACEX,'|','_',7,0);
    drawBoard(18,1,COMMANDSPACEY + 18, COMMANDSPACEX + 1,'|','-',1,1);


    refresh();
    move(y,x);
}

/* This function clears the message space
It doesn't take any input and does not return anything
*/
void hideMessageFromCommandSpace(void)
{

   mvprintw(20,3,"                                                                        ");
   mvprintw(21,3,"                                                                        ");
   mvprintw(22,3,"                                                                        ");


   refresh();
}

/*
This function stores all the commands entered by user during program execution to a file
It takes address of allEnteredInputs array of string containing all the commands entered by the user
and count of number of commands i.e. count
It returns a character 'y' or 'n', if user wants to store or not
*/
char fileOperation(char* allEnteredInputs[], int count)
{
    char permission = ' ';
    // change the location
    char location[70] = "assets/logoCommand.txt"; /* Default name and location of the text file*/
    FILE *filePtr;
    int i = 0;
    char **pointerToUserCommand = allEnteredInputs;
    attron(A_BOLD);
    mvprintw(21, 2, "Type 'y' to store all your commands in a file (assets/logocommand.txt)");
    attroff(A_BOLD);
    move(22,4);
    permission = getch();

    if (isalpha(permission))
    {
        permission = tolower(permission);
    }
    if (permission == 'y') /* If permission is given to store the file*/
    {


        filePtr=fopen(location,"w");
        if (filePtr==NULL){

          mvprintw(20, 4, "           ERROR                        "); /* Error message if file doesn't get stored*/
          getch();
          endwin();
         return permission;

       } else
       {
           for (i = 0; i<count-1;i++)
           {
               fprintf(filePtr,"%s \n",pointerToUserCommand[i]);

           }
           fclose(filePtr);

       }
    }
     return permission;

}

/*
This function takes address of array of string i.e. wordsInput containing entered user input string broken into array
of words, and count of number of words i.e. counter , and address of struct turtle
It checks, if the particular input is allowed logo command or not, if the first word of the array of input words is
a valid logo command, it passes the word array to processOutput function to get the output
This function return the string i.e. the output

*/
char* getOutput(char* wordsInput[], int counter, Turtle* turtle)
{
    char* ptrChar;
    /* allowed logo commands and keywords*/
    char* allowedCommands[21] = {"home","forward","fd", "back", "bk",
    "pd", "pu", "rt", "lt",  "quit", "setpencolor", "setxy", "st", "ht", "clean", "clearscreen", "cs", "print", "right", "left", "file"};
    int i = 0, matchFound = 0;
    char tempStr[70];



    ptrChar=(char*)malloc(70 * sizeof(char));

    for (i=0;i<21;i++)
    {
        /* check if the first word of input matches with the allowed logo keyword*/
        if ((strcmp(allowedCommands[i],wordsInput[0]) == 0))
        {
            if (i==20)
            {
                readCommandFromTheFile(wordsInput,counter);/* function to read command from a file based on the line number specified*/
            }
            matchFound = 1;
            break;
        }

    }

    if (matchFound == 1) /* if it's a valid logo keyword*/
    {
        /* logo commands with a duplicate name i.e. lt is left*/
         if (i == 18)
        {
            i = 7;
        }
        else if (i == 19)
        {
            i =8;
        }

        /* This function process the output and stores output in ptrChar string*/
        processOutput(ptrChar,wordsInput,i,turtle,counter);

    } /* if not a valid logo command*/
    else
    {
        if (strlen(wordsInput[0]) > 53 ) /* if size of the word is greater than command space size*/
        {
            strcpy(tempStr, "I don't know how to ");
            strcat(tempStr,"");
            strcpy(ptrChar,tempStr);
            displayMessageInMessageSpace(0,15,"too long..");


        }
        else
        {
            strcpy(tempStr, "I don't know how to ");
            strcat(tempStr,wordsInput[0]);
            strcpy(ptrChar,tempStr);
            displayMessageInMessageSpace(0,15," ");


        }


    }


    return (char *)ptrChar; /* return the output*/

}

/*This function process the output
It either calculates the output based on print arithmatic operation or calls related function to move turtle
Command execution and error handling takes place in this function
it takes address of string to store the output i.e. ptrOutput, address of array of words of input i.e. wordsInput,
command number (eg: home's 0, fd's 1 and so on)i.e. commandNum, address of turtle, and count o total words in wordsInput array of string i.e.
totalWords
This function doesn't return anything

*/
void processOutput(char* ptrOutput, char* wordsInput[], int commandNum, Turtle* turtle, int totalWords)
{

    char outputToDisplay[70] = "", outputMessageToDisplay[15] = "";
    int tempAngle = 0, enteredAngle = 0, tempTurtleVisibility = 0, i =0, tempValid = 0;
    int temp = 0, tempNewPosY = 0, tempNewPosX = 0, currentPosY = 0, currentPosX = 0;
    int minY = 4, maxY = 15, minX = 3, maxX = 50, tempCount = 0;
    int colorToUse = turtle->colorCode;
    float num1 = 0.0, num2 = 0.0, answer = 0.0, tempf = 0.0;

        /* Note: totalWords index start from 0, i.e. totalWords = 0 means 1 word and so on */
        switch (commandNum)
        {
        case 0:   /* home command*/
            {
               if (totalWords == 0) /* if its a single word i.e. valid command*/
               {
                   hideTurtleFromDrawingSpace(turtle); /* function to hide the turtle*/
                   /* reset the turtle*/
                   turtle->posY = TURTLEY;
                   turtle->posX = TURTLEX;
                   turtle->angle = 0;

                   /* display the turtle after changing it's location
                      i.e. with a new location  */
                   displayTurtleInDrawingSpace(turtle);
                   displayInfoInInfoSpace(turtle); /* display turtle info in info box*/


                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Command Executed.."); /* display message in message box*/

               }
               else if (totalWords == 1) /* 2 words command*/
               {
                   if (atoi(wordsInput[1]) != 0) /* if the second word is a number*/
                   {
                       strcat(outputToDisplay, "You don't say what to do with ");
                       if (strlen(wordsInput[1])>53) /* size of word is greater than allowable command space size*/
                       {
                           strcat(outputToDisplay, " ");
                       }
                       else
                       {
                           strcat(outputToDisplay, wordsInput[1]);

                       }

                       strcpy(ptrOutput, outputToDisplay);
                       displayMessageInMessageSpace(0,3,wordsInput[1]); /* display the message in message space*/
                   }
                   else /* if second word is not a number*/
                   {
                        strcat(outputToDisplay, "Execute only single command at a time ");
                        strcat(outputToDisplay, "");
                        strcpy(ptrOutput, outputToDisplay);
                        displayMessageInMessageSpace(0,8,"");

                   }

               }
               else
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 1:   /* forward*/
            {


            }
        case 2:   /* fd */
            {
                if (totalWords == 0) /* valid single word command*/
                {
                       strcat(outputToDisplay, "Not enough input to ");

                       if (strlen(wordsInput[0])>53)
                       {
                           strcat(outputToDisplay, "");
                       }
                       else
                       {
                           strcat(outputToDisplay, wordsInput[0]);
                       }

                       strcpy(ptrOutput, outputToDisplay);
                       displayMessageInMessageSpace(0,2,wordsInput[0]);


                }
                else if (totalWords == 1)
                {
                   /* invalid command*/
                   if (atoi(wordsInput[1]) == 0) /* if second word is not a number*/
                   {
                       /* if the second word is from allowed command list */
                        if ((strcmp(wordsInput[1],"pu") == 0)||(strcmp(wordsInput[1],"pd") == 0)||
                           (strcmp(wordsInput[1],"ht") == 0)||(strcmp(wordsInput[1],"st") == 0)||
                           (strcmp(wordsInput[1],"fd") == 0)||(strcmp(wordsInput[1],"bk") == 0) ||
                           (strcmp(wordsInput[1],"home") == 0)||(strcmp(wordsInput[1],"print") == 0)||
                           (strcmp(wordsInput[1],"cs") == 0)||(strcmp(wordsInput[1],"clean") == 0)||
                            (strcmp(wordsInput[1],"setpencolor") == 0)||(strcmp(wordsInput[1],"forward") == 0)||
                            (strcmp(wordsInput[1],"back") == 0)||(strcmp(wordsInput[1],"left") == 0)||
                            (strcmp(wordsInput[1],"right") == 0)||(strcmp(wordsInput[1],"clearscreen") == 0)||
                           (strcmp(wordsInput[1],"rt") == 0)||(strcmp(wordsInput[1],"lt") == 0))
                               {

                                   strcat(outputToDisplay, "You can only execute single command at a time ");
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,8,wordsInput[1]);
                               }
                               else
                               {

                                  strcat(outputToDisplay, "I don't know how to ");
                                  if (strlen(wordsInput[1])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[1]);

                                   }
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,1,wordsInput[1]);
                               }


                   }
                   else /* valid command*/
                   {
                        temp = atoi(wordsInput[1]); /* number of lines to move */
                        if ((temp< 1000) && (temp > -1000)) /* entered input is less than 1000 or greater than -1000*/
                        {
                            tempTurtleVisibility = turtle->visibility;
                            hideTurtleFromDrawingSpace(turtle);
                        if (temp > 0) /* move forward if input is a positive number*/
                        {
                           tempCount = drawLines(turtle,'f', temp); /* this function will draw the lines*/
                        }
                        else /* move backword if input is a negative number*/
                        {
                            temp = temp * (-1);
                            tempCount = drawLines(turtle,'b', temp);

                        }


                        if (tempTurtleVisibility == 1)
                        {
                            displayTurtleInDrawingSpace(turtle);
                        }
                        displayInfoInInfoSpace(turtle);
                        if (tempCount>0)
                        {
                            strcat(outputToDisplay, "Move forward  ");
                            if (strlen(wordsInput[1])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[1]);

                           }

                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(1,0,"Turtle Moved..");



                        }
                        else
                        {
                            strcat(outputToDisplay, "Turtle couldn't move because it's already at the edge of the border ");
                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(0,16,wordsInput[1]);


                        }


                        }
                        else
                        {
                            strcat(outputToDisplay, "Please enter the number between -1000 to 1000");
                            strcat(outputToDisplay, "");
                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(0,7,"");



                        }



                   }

                }
                else /* if more than 2 words input*/
                {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

                }

               break;
            }
        case 3:   /* back */
            {

            }
        case 4:   /* bk */
            {
                if (totalWords == 0) /* single word - invalid */
                {
                       strcat(outputToDisplay, "Not enough input to ");
                       if (strlen(wordsInput[0])>53)
                       {
                           strcat(outputToDisplay, " ");
                       }
                       else
                       {
                           strcat(outputToDisplay, wordsInput[0]);

                       }

                       strcpy(ptrOutput, outputToDisplay);
                       displayMessageInMessageSpace(0,2,wordsInput[0]);


                }
                 else if (totalWords == 1) /* 2 words input */
                {
                   if (atoi(wordsInput[1]) == 0) /* if second word is not a number*/
                   {

                       if ((strcmp(wordsInput[1],"pu") == 0)||(strcmp(wordsInput[1],"pd") == 0)||
                           (strcmp(wordsInput[1],"ht") == 0)||(strcmp(wordsInput[1],"st") == 0)||
                           (strcmp(wordsInput[1],"fd") == 0)||(strcmp(wordsInput[1],"bk") == 0) ||
                           (strcmp(wordsInput[1],"home") == 0)||(strcmp(wordsInput[1],"print") == 0)||
                           (strcmp(wordsInput[1],"cs") == 0)||(strcmp(wordsInput[1],"clean") == 0)||
                            (strcmp(wordsInput[1],"setpencolor") == 0)||(strcmp(wordsInput[1],"forward") == 0)||
                            (strcmp(wordsInput[1],"back") == 0)||(strcmp(wordsInput[1],"left") == 0)||
                            (strcmp(wordsInput[1],"right") == 0)||(strcmp(wordsInput[1],"clearscreen") == 0)||
                           (strcmp(wordsInput[1],"rt") == 0)||(strcmp(wordsInput[1],"lt") == 0))
                               {

                                   strcat(outputToDisplay, "You can only execute single command at a time ");
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,8,wordsInput[1]);
                               }
                               else
                               {

                                  strcat(outputToDisplay, "I don't know how to ");
                                  if (strlen(wordsInput[1])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[1]);

                                   }
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,1,wordsInput[1]);
                               }

                   }
                   else /* valid command*/
                   {
                        temp = atoi(wordsInput[1]); /* number of lines to move */
                        if ((temp< 1000) && (temp > -1000)) /* entered input is less than 1000 or greater than -1000*/
                        {
                            tempTurtleVisibility = turtle->visibility;
                        hideTurtleFromDrawingSpace(turtle);

                        if (temp > 0) /* if positive number input*/
                        {
                          tempCount = drawLines(turtle,'b', temp); /* call this function to draw backward line*/

                        }
                        else /* if negative number input */
                        {
                            temp = temp * (-1);
                            tempCount = drawLines(turtle,'f', temp); /* draw forward lines*/
                        }


                        if (tempTurtleVisibility == 1)
                        {
                            displayTurtleInDrawingSpace(turtle);
                        }
                        displayInfoInInfoSpace(turtle);
                        if (tempCount>0)
                        {
                            strcat(outputToDisplay, "Move back  ");
                             if (strlen(wordsInput[1])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[1]);

                           }

                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(1,0,"Turtle Moved..");

                        }
                        else
                        {
                            strcat(outputToDisplay, "Turtle couldn't move because it's already at the edge of the border ");
                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(0,16,wordsInput[1]);


                        }

                        }
                        else
                        {

                            strcat(outputToDisplay, "Please enter the number between -1000 to 1000  ");
                            strcat(outputToDisplay, "");
                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(0,7,"");


                        }


                   }

                }
                else /* if more than 2 words in input*/
                {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

                }

               break;
            }
        case 5:   /* pd i.e. pendown*/
            {
               if (totalWords == 0) /* valid single word command*/
               {
                   turtle->linesVisibility = 1; /* change linesvisibility*/
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Pen Visible..");


               }
               else /* invalid command*/
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 6:   /* pu i.e. penup */
            {
               if (totalWords == 0) /* valid single word command*/
               {
                   turtle->linesVisibility = 0; /* change linesVisibility*/
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Pen Invisible..");

               }
               else /* invalid command*/
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 7:   /* rt or right i.e. right turn*/
            {
               if (totalWords == 0) /* if only single word input*/
               {

                   strcat(outputToDisplay, "Not enough input to ");
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(0,2,wordsInput[0]);

               }
               else if (totalWords == 1) /* 2 words input */
               {

                   if (atoi(wordsInput[1]) == 0) /* second word is not a number*/
                   {
                       if ((strcmp(wordsInput[1],"pu") == 0)||(strcmp(wordsInput[1],"pd") == 0)||
                           (strcmp(wordsInput[1],"ht") == 0)||(strcmp(wordsInput[1],"st") == 0)||
                           (strcmp(wordsInput[1],"fd") == 0)||(strcmp(wordsInput[1],"bk") == 0) ||
                           (strcmp(wordsInput[1],"home") == 0)||(strcmp(wordsInput[1],"print") == 0)||
                           (strcmp(wordsInput[1],"cs") == 0)||(strcmp(wordsInput[1],"clean") == 0)||
                            (strcmp(wordsInput[1],"setpencolor") == 0)||(strcmp(wordsInput[1],"forward") == 0)||
                            (strcmp(wordsInput[1],"back") == 0)||(strcmp(wordsInput[1],"left") == 0)||
                            (strcmp(wordsInput[1],"right") == 0)||(strcmp(wordsInput[1],"clearscreen") == 0)||
                           (strcmp(wordsInput[1],"rt") == 0)||(strcmp(wordsInput[1],"lt") == 0))
                               {

                                   strcat(outputToDisplay, "You can only execute single command at a time ");
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,8,wordsInput[1]);
                               }
                               else
                               {

                                  strcat(outputToDisplay, "I don't know how to ");
                                  if (strlen(wordsInput[1])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[1]);

                                   }
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,1,wordsInput[1]);
                               }

                   }
                   else /* valid input with second word as a number*/
                   {
                       enteredAngle = atoi(wordsInput[1]);
                       if ((enteredAngle == 45) || (enteredAngle == 90)) /*if entered input is 45 or 90*/
                       {
                           tempAngle = turtle->angle + enteredAngle; /* add the angles for right turn*/

                           if (tempAngle == 360) /* 360 degree = 0 degree*/
                           {
                               tempAngle = 0;
                           }

                           hideTurtleFromDrawingSpace(turtle);

                           turtle->angle = tempAngle;


                           displayTurtleInDrawingSpace(turtle);
                           displayInfoInInfoSpace(turtle);

                           strcat(outputToDisplay, "to right, degree moved: ");
                           if (strlen(wordsInput[1])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[1]);

                           }

                           strcpy(ptrOutput, outputToDisplay);
                           displayMessageInMessageSpace(1,0,"Turtle turned right..");



                       }
                       else /* if input is not 45 or 90*/
                       {
                           strcat(outputToDisplay, "Only 45 or 90 degree is allowed ");
                           strcat(outputToDisplay, " ");
                           strcpy(ptrOutput, outputToDisplay);
                           displayMessageInMessageSpace(0,6,wordsInput[1]);

                       }




                   }


               }
               else
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }


               break;
            }
        case 8:   /* lt or left i.e. left turn*/
            {
               if (totalWords == 0) /*one word input i.e. invalid input*/
               {

                   strcat(outputToDisplay, "Not enough input to ");
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(0,2,wordsInput[0]);

               }
               else if (totalWords == 1) /* if 2 words input*/
               {
                   if (atoi(wordsInput[1]) == 0) /* 2nd word is not a number*/
                   {
                       if ((strcmp(wordsInput[1],"pu") == 0)||(strcmp(wordsInput[1],"pd") == 0)||
                           (strcmp(wordsInput[1],"ht") == 0)||(strcmp(wordsInput[1],"st") == 0)||
                           (strcmp(wordsInput[1],"fd") == 0)||(strcmp(wordsInput[1],"bk") == 0) ||
                           (strcmp(wordsInput[1],"home") == 0)||(strcmp(wordsInput[1],"print") == 0)||
                           (strcmp(wordsInput[1],"cs") == 0)||(strcmp(wordsInput[1],"clean") == 0)||
                            (strcmp(wordsInput[1],"setpencolor") == 0)||(strcmp(wordsInput[1],"forward") == 0)||
                            (strcmp(wordsInput[1],"back") == 0)||(strcmp(wordsInput[1],"left") == 0)||
                            (strcmp(wordsInput[1],"right") == 0)||(strcmp(wordsInput[1],"clearscreen") == 0)||
                           (strcmp(wordsInput[1],"rt") == 0)||(strcmp(wordsInput[1],"lt") == 0))
                               {

                                   strcat(outputToDisplay, "You can only execute single command at a time ");
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,8,wordsInput[1]);
                               }
                               else
                               {

                                  strcat(outputToDisplay, "I don't know how to ");
                                  if (strlen(wordsInput[1])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[1]);

                                   }
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,1,wordsInput[1]);
                               }

                   }
                    else /* valid input with 2nd word as a number*/
                   {
                       enteredAngle = atoi(wordsInput[1]);
                       if ((enteredAngle == 45) || (enteredAngle == 90))
                       {


                           tempAngle = (int)turtle->angle;
                           if (tempAngle ==0)
                           {
                               tempAngle =360;
                           }
                           tempAngle = tempAngle - enteredAngle; /* subtract angles for left turn*/

                           hideTurtleFromDrawingSpace(turtle);

                           turtle->angle = tempAngle;


                           displayTurtleInDrawingSpace(turtle);
                           displayInfoInInfoSpace(turtle);

                           strcat(outputToDisplay, "to left, degree moved: ");
                           if (strlen(wordsInput[1])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[1]);

                           }

                           strcpy(ptrOutput, outputToDisplay);
                           displayMessageInMessageSpace(1,0,"Turtle turned left..");



                       }
                       else /* if angle entered is not 45 or 90 degree*/
                       {
                           strcat(outputToDisplay, "Only 45 or 90 degree is allowed ");
                           strcat(outputToDisplay, " ");
                           strcpy(ptrOutput, outputToDisplay);
                           displayMessageInMessageSpace(0,6,wordsInput[1]);

                       }

                   }

               }
               else
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 9:   /* quit */
            {
                if (totalWords > 0) /* invalid command - more than one word*/
               {
                    strcat(outputToDisplay, "Type only quit to exit ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 10:   /* setpencolor */
            {
                 if (totalWords == 0) /* invalid input - only 1 word input*/
                 {
                       strcat(outputToDisplay, "Not enough input to ");
                       strcat(outputToDisplay, wordsInput[0]);
                       strcpy(ptrOutput, outputToDisplay);
                       displayMessageInMessageSpace(0,2,wordsInput[0]);


                 }
                  else if (totalWords == 1) /* 2 words input */
                  {
                      if (atoi(wordsInput[1]) == 0) /* 2nd word is not a number */
                      {
                          if ((strcmp(wordsInput[1],"pu") == 0)||(strcmp(wordsInput[1],"pd") == 0)||
                           (strcmp(wordsInput[1],"ht") == 0)||(strcmp(wordsInput[1],"st") == 0)||
                           (strcmp(wordsInput[1],"fd") == 0)||(strcmp(wordsInput[1],"bk") == 0) ||
                           (strcmp(wordsInput[1],"home") == 0)||(strcmp(wordsInput[1],"print") == 0)||
                           (strcmp(wordsInput[1],"cs") == 0)||(strcmp(wordsInput[1],"clean") == 0)||
                            (strcmp(wordsInput[1],"setpencolor") == 0)||(strcmp(wordsInput[1],"forward") == 0)||
                            (strcmp(wordsInput[1],"back") == 0)||(strcmp(wordsInput[1],"left") == 0)||
                            (strcmp(wordsInput[1],"right") == 0)||(strcmp(wordsInput[1],"clearscreen") == 0)||
                           (strcmp(wordsInput[1],"rt") == 0)||(strcmp(wordsInput[1],"lt") == 0))
                               {

                                   strcat(outputToDisplay, "You can only execute single command at a time ");
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,8,wordsInput[1]);
                               }
                               else
                               {

                                  strcat(outputToDisplay, "I don't know how to ");
                                  if (strlen(wordsInput[1])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[1]);

                                   }
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,1,wordsInput[1]);
                               }



                      }
                      else /* valid input */
                      {
                         temp = atoi(wordsInput[1]);
                         if (temp <= 7) /* numbers allowed are 1 to 7*/
                         {
                             switch (temp)
                             {
                                 case 1:
                                {
                                    turtle->colorCode =3; /* blue pen */
                                    strcat(outputToDisplay, "Blue pen ");
                                    break;
                                }
                            case 2:
                                {
                                    turtle->colorCode = 1; /* green pen */
                                    strcat(outputToDisplay, "Green pen ");
                                    break;
                                }
                            case 3:
                                {
                                    turtle->colorCode = 5; /* cyan pen */
                                    strcat(outputToDisplay, "Default Cyan pen  ");
                                    break;
                                }
                            case 4:
                                {
                                    turtle->colorCode = 2; /* red pen */
                                    strcat(outputToDisplay, "Red pen ");
                                    break;
                                }
                            case 5:
                                {
                                    turtle->colorCode = 6; /* magenta pen */
                                    strcat(outputToDisplay, "Magenta pen ");
                                    break;
                                }
                            case 6:
                                {
                                    turtle->colorCode = 4; /* yellow pen */
                                    strcat(outputToDisplay, "Yellow pen ");
                                    break;
                                }
                            case 7:
                                {
                                    turtle->colorCode = 7; /* white pen */
                                    strcat(outputToDisplay, "White pen ");
                                    break;
                                }


                             }
                               displayTurtleInDrawingSpace(turtle);
                               displayInfoInInfoSpace(turtle);

                                if (strlen(wordsInput[1])>53)
                               {
                                   strcat(outputToDisplay, " ");
                               }
                               else
                               {
                                   strcat(outputToDisplay, wordsInput[1]);

                               }

                               strcpy(ptrOutput, outputToDisplay);
                               displayMessageInMessageSpace(1,0,"Color changed..");


                         }
                         else /* if number is greater than 7 or less than 1*/
                           {
                               strcat(outputToDisplay, "Choose between 1 to 7 ");
                               strcpy(ptrOutput, outputToDisplay);
                               displayMessageInMessageSpace(0,7,wordsInput[1]);

                           }


                      }


                  }
                  else
                   {
                        strcat(outputToDisplay, "Execute only single command at a time ");
                        strcat(outputToDisplay, "");
                        strcpy(ptrOutput, outputToDisplay);
                        displayMessageInMessageSpace(0,8,"");

                   }


               break;
            }
        case 11:   /* setxy */
            {
                 if (totalWords <= 1) /* invalid input - input words less than 2*/
                 {
                        strcat(outputToDisplay, "Please provide x and y both the co ordinates ");
                        strcat(outputToDisplay, "");
                        strcpy(ptrOutput, outputToDisplay);
                        displayMessageInMessageSpace(0,2,wordsInput[0]);

                 }
                 else if (totalWords == 2) /* 3 words input*/
                 {

                    if (atoi(wordsInput[1]) == 0) /* if 2nd word is not a number*/
                      {
                           if ((strcmp(wordsInput[1],"pu") == 0)||(strcmp(wordsInput[1],"pd") == 0)||
                           (strcmp(wordsInput[1],"ht") == 0)||(strcmp(wordsInput[1],"st") == 0)||
                           (strcmp(wordsInput[1],"fd") == 0)||(strcmp(wordsInput[1],"bk") == 0) ||
                           (strcmp(wordsInput[1],"home") == 0)||(strcmp(wordsInput[1],"print") == 0)||
                           (strcmp(wordsInput[1],"cs") == 0)||(strcmp(wordsInput[1],"clean") == 0)||
                            (strcmp(wordsInput[1],"setpencolor") == 0)||(strcmp(wordsInput[1],"forward") == 0)||
                            (strcmp(wordsInput[1],"back") == 0)||(strcmp(wordsInput[1],"left") == 0)||
                            (strcmp(wordsInput[1],"right") == 0)||(strcmp(wordsInput[1],"clearscreen") == 0)||
                           (strcmp(wordsInput[1],"rt") == 0)||(strcmp(wordsInput[1],"lt") == 0))
                               {

                                   strcat(outputToDisplay, "You can only execute single command at a time ");
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,8,wordsInput[1]);
                               }
                               else
                               {

                                  strcat(outputToDisplay, "I don't know how to ");
                                  if (strlen(wordsInput[1])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[1]);

                                   }
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,1,wordsInput[1]);
                               }

                      }
                      else if (atoi(wordsInput[2]) == 0) /* if 3rd word is not a number*/
                      {
                           if ((strcmp(wordsInput[2],"pu") == 0)||(strcmp(wordsInput[2],"pd") == 0)||
                           (strcmp(wordsInput[2],"ht") == 0)||(strcmp(wordsInput[2],"st") == 0)||
                           (strcmp(wordsInput[2],"fd") == 0)||(strcmp(wordsInput[2],"bk") == 0) ||
                           (strcmp(wordsInput[2],"home") == 0)||(strcmp(wordsInput[2],"print") == 0)||
                           (strcmp(wordsInput[2],"cs") == 0)||(strcmp(wordsInput[2],"clean") == 0)||
                            (strcmp(wordsInput[2],"setpencolor") == 0)||(strcmp(wordsInput[2],"forward") == 0)||
                            (strcmp(wordsInput[2],"back") == 0)||(strcmp(wordsInput[2],"left") == 0)||
                            (strcmp(wordsInput[2],"right") == 0)||(strcmp(wordsInput[2],"clearscreen") == 0)||
                           (strcmp(wordsInput[2],"rt") == 0)||(strcmp(wordsInput[2],"lt") == 0))
                               {

                                   strcat(outputToDisplay, "You can only execute single command at a time ");
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,8,wordsInput[2]);
                               }
                               else
                               {

                                  strcat(outputToDisplay, "I don't know how to ");
                                  if (strlen(wordsInput[2])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[2]);

                                   }
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,1,wordsInput[2]);
                               }


                      }
                      else /* both 2nd and 3rd words are number */
                      {
                          tempNewPosY = (int)atoi(wordsInput[2]);
                          tempNewPosX = (int)atoi(wordsInput[1]);
                          /* if the input X and Y co-ordinates are not in board range */
                          if (((tempNewPosY < minY) || (tempNewPosY > maxY)) || ((tempNewPosX < minX) ||(tempNewPosX > maxX)))
                          {
                               strcat(outputToDisplay, "Please provide the range between X:(3 to 50), Y:(4 to 15)  ");
                               strcat(outputToDisplay,"");

                               strcat(outputMessageToDisplay,wordsInput[1]);
                               strcat(outputMessageToDisplay," ");
                               strcat(outputMessageToDisplay,wordsInput[2]);
                               strcpy(ptrOutput, outputToDisplay);
                               displayMessageInMessageSpace(0,7,outputMessageToDisplay);

                          }
                          else /* X and Y co-ordinates are in board range */
                          {
                            currentPosY = turtle->posY;
                            currentPosX = turtle->posX;

                            if (currentPosY == tempNewPosY) /* move horizontally */
                            {
                                tempTurtleVisibility = turtle->visibility;
                                hideTurtleFromDrawingSpace(turtle);
                                if (currentPosX<tempNewPosX) /* move right */
                                {
                                    temp = tempNewPosX - currentPosX;
                                    for (i=currentPosX;i<tempNewPosX;i++)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(currentPosY, i, '-');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }
                                }
                                else /* move left */
                                {
                                    temp = currentPosX -  tempNewPosX;
                                    for (i=currentPosX;i>tempNewPosX;i--)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(currentPosY, i, '-');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }

                                }
                            ++tempValid;

                            }
                            else if (currentPosX == tempNewPosX) /* move vertically */
                            {
                                tempTurtleVisibility = turtle->visibility;
                                hideTurtleFromDrawingSpace(turtle);
                                if (currentPosY<tempNewPosY) /* move down */
                                {
                                    temp = tempNewPosY - currentPosY;
                                    for (i=currentPosY;i<tempNewPosY;i++)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(i, currentPosX, '|');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }
                                }
                                else /* move up */
                                {
                                    temp = currentPosY - tempNewPosY;
                                    for (i=currentPosY;i>tempNewPosY;i--)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(i, currentPosX, '|');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }

                                }

                                ++tempValid;

                            }
                            else /* move diagonally */
                            {
                                if (currentPosY > tempNewPosY)
                                {

                                    temp = currentPosY - tempNewPosY;
                                    if ((currentPosX + temp) == tempNewPosX) /* move 45 degree */
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for (i=currentPosY;i>tempNewPosY;i--)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(--currentPosY, ++currentPosX, '/');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }
                                    else if ((currentPosX - temp) == tempNewPosX) /* move 315 degree */
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for (i=currentPosY;i>tempNewPosY;i--)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(--currentPosY, --currentPosX, '\\');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }

                                }
                                else if (currentPosY < tempNewPosY)
                                {
                                    temp = tempNewPosY - currentPosY;
                                    if ((currentPosX - temp) == tempNewPosX) /* move 225 degree */
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for (i=currentPosY;i<tempNewPosY;i++)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(++currentPosY, --currentPosX, '/');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }
                                    else if ((currentPosX + temp) == tempNewPosX) /* move 135 degree */
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for (i=currentPosY;i<tempNewPosY;i++)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(++currentPosY, ++currentPosX, '\\');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }


                                }



                            }


                          }

                          /* if not valid co-ordinates */
                          if (tempValid == 0)
                                {
                                   strcat(outputToDisplay, "It only supports Horizontal, Vertical and Diagonal movement ");
                                   strcat(outputToDisplay,"");
                                   strcat(outputMessageToDisplay,wordsInput[1]);
                                   strcat(outputMessageToDisplay," ");
                                   strcat(outputMessageToDisplay,wordsInput[2]);
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,7,outputMessageToDisplay);

                                }
                                else /* valid co-ordinates */
                                {
                                    turtle->posY = atoi(wordsInput[2]);
                                    turtle->posX = atoi(wordsInput[1]);
                                    if (tempTurtleVisibility == 1)
                                    {
                                        displayTurtleInDrawingSpace(turtle);
                                    }
                                    displayInfoInInfoSpace(turtle);
                                    strcat(outputToDisplay, "Moved to ");
                                    strcat(outputToDisplay, wordsInput[1]);
                                    strcat(outputToDisplay, " ");
                                    strcat(outputToDisplay, wordsInput[2]);
                                    strcat(outputToDisplay, " position  ");
                                    strcpy(ptrOutput, outputToDisplay);
                                    strcat(outputMessageToDisplay, wordsInput[1]);
                                    strcat(outputMessageToDisplay, "  ");
                                    strcat(outputMessageToDisplay,wordsInput[2]);
                                    displayMessageInMessageSpace(1,1,outputMessageToDisplay);


                                }


                      }

                 }
                 else
                 {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

                 }


               break;
            }
        case 12:   /* st i.e. show turtle */
            {
               if (totalWords == 0) /* valid single word command */
               {
                   displayTurtleInDrawingSpace(turtle);
                   displayInfoInInfoSpace(turtle);

                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Show Turtle..");

               }
               else
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 13:   /* ht i.e. hide turtle */
            {
               if (totalWords == 0) /* valid single word command */
               {
                   hideTurtleFromDrawingSpace(turtle);
                   displayInfoInInfoSpace(turtle);

                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Hide Turtle..");

               }
               else
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 14:   /* clean */
            {
               if (totalWords == 0) /* valid single word command */
               {

                   cleanDrawingSpace();
                   displayTurtleInDrawingSpace(turtle);
                   displayInfoInInfoSpace(turtle);

                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Cleaned..");

               }
               else
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 15:   /* clearscreen */
            {


            }
        case 16:   /* cs */
            {
               if (totalWords == 0) /* valid single word command */
               {
                   cleanDrawingSpace();
                   turtle->posY = TURTLEY;
                   turtle->posX = TURTLEX;
                   turtle->angle = 0;
                   turtle->visibility = 1;
                   turtle->linesVisibility = 1;

                   displayTurtleInDrawingSpace(turtle);
                   displayInfoInInfoSpace(turtle);

                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Clear Screen..");

               }
               else
               {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 17:   /* print */
            {

                 if (totalWords == 0) /* invalid single word input */
                 {
                   strcat(outputToDisplay, "Not enough input to ");
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(0,2,wordsInput[0]);


                 }
                 else if (totalWords == 1) /* if total 1 input word */
                 {
                     if ((strcmp(wordsInput[1],"word") == 0)) /* word keyword is present in input */
                     {
                           strcat(outputToDisplay, "Not enough input to ");
                           if (strlen(wordsInput[1])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[1]);

                           }

                           strcpy(ptrOutput, outputToDisplay);
                           displayMessageInMessageSpace(0,2,wordsInput[1]);

                     }
                     else if (wordsInput[1][0] == '"') /* if word begins with " i.e. valid input */
                     {

                        outputToDisplay[0] = ' ';
                        for (i=1;i<strlen(wordsInput[1]);i++)
                        {
                          outputToDisplay[i] = wordsInput[1][i];
                        }
                        outputToDisplay[strlen(wordsInput[1])] = '\0';

                        strcpy(ptrOutput, outputToDisplay);
                        displayMessageInMessageSpace(1,2,outputToDisplay);

                     }
                     else if (wordsInput[1][0] == '[') /* words begins with [*/
                     {
                         if (wordsInput[1][strlen(wordsInput[1])-1] == ']')
                         {
                             outputToDisplay[0] = ' ';
                            for (i=1;i<strlen(wordsInput[1])-1;i++)
                            {
                              outputToDisplay[i] = wordsInput[1][i];
                            }
                            outputToDisplay[strlen(wordsInput[1])-1] = '\0';
                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(1,2,outputToDisplay);

                         }
                         else
                         {
                                strcat(outputToDisplay, "Please put the words inside [] ");
                                strcat(outputToDisplay, "");
                                strcpy(ptrOutput, outputToDisplay);
                                displayMessageInMessageSpace(0,7,wordsInput[1]);

                         }


                     }
                     else /* invalid input */
                     {
                            strcat(outputToDisplay, "I don't know how to ");
                            if(strlen(wordsInput[1])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[1]);

                           }

                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(0,9,"");

                     }

                 }
                 else if (totalWords >= 2) /* total more than 2 words input */
                 {

                      if ((strcmp(wordsInput[1],"word") == 0)) /* if input contains word keyword */
                      {
                          if (totalWords < 4) /* for 4 words input*/
                          {
                              if (wordsInput[2][0] == '"') /* if 3rd word starts with "*/
                              {
                                  if (wordsInput[3][0] == '"') /* if 4th word starts with "*/
                                  {
                                        outputToDisplay[0] = ' ';
                                        for (i=1;i<strlen(wordsInput[2]);i++) /* copy the 3rd word in output */
                                        {
                                            ++tempCount;
                                          outputToDisplay[tempCount] = wordsInput[2][i];
                                        }
                                        for (i=1;i<strlen(wordsInput[3]);i++) /* merge 3rd word with 4th word */
                                        {
                                          ++tempCount;
                                          outputToDisplay[tempCount] = wordsInput[3][i];
                                        }
                                        outputToDisplay[++tempCount] = '\0';
                                        strcpy(ptrOutput, outputToDisplay);
                                        displayMessageInMessageSpace(1,2,outputToDisplay);


                                  }
                                  else /* invalid input if 3rd word doesn't start with " */
                                  {
                                      strcat(outputToDisplay, "I don't know how to ");
                                       if (strlen(wordsInput[3])>53)
                                       {
                                           strcat(outputToDisplay, " ");
                                       }
                                       else
                                       {
                                           strcat(outputToDisplay, wordsInput[3]);

                                       }

                                      strcpy(ptrOutput, outputToDisplay);
                                      displayMessageInMessageSpace(0,9,"");

                                  }

                              }
                              else /* invalid input if 4th word doesn't start with " */
                              {
                                    strcat(outputToDisplay, "I don't know how to ");
                                     if (strlen(wordsInput[2])>53)
                                       {
                                           strcat(outputToDisplay, " ");
                                       }
                                       else
                                       {
                                           strcat(outputToDisplay, wordsInput[2]);

                                       }

                                    strcpy(ptrOutput, outputToDisplay);
                                    displayMessageInMessageSpace(0,9,"");


                              }


                          }
                          else /* word command can only use for 2 words */
                          {
                                strcat(outputToDisplay, "Support for word command is available for not more than 2 words ");
                                strcat(outputToDisplay, "");
                                strcpy(ptrOutput, outputToDisplay);
                                displayMessageInMessageSpace(0,8,"");

                          }


                      }
                      else if (wordsInput[1][0] == '[') /* word begins with [ */
                      {
                        if (wordsInput[totalWords][strlen(wordsInput[totalWords])-1] == ']') /*last word ends with ]*/
                        {

                            outputToDisplay[0] = ' ';
                            /* copy all the words between [] in to output string */
                            for (temp = 1; temp<totalWords+1;temp++)
                            {
                                for (i=0;i<strlen(wordsInput[temp]);i++)
                                {
                                   if (((temp==1)&&(i==0)) || ((temp == totalWords)&&(i==(strlen(wordsInput[temp])-1))))
                                   {
                                       continue;
                                   }
                                   ++tempCount;
                                   outputToDisplay[tempCount] = wordsInput[temp][i];
                                }
                                 ++tempCount;
                                 outputToDisplay[tempCount] = ' ';

                            }
                            outputToDisplay[++tempCount] = '\0';
                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(1,2,outputToDisplay);


                        }
                        else /* if words are not put between []*/
                        {
                                strcat(outputToDisplay, "Please put the words inside [] ");
                                strcat(outputToDisplay, "");
                                strcpy(ptrOutput, outputToDisplay);
                                displayMessageInMessageSpace(0,7,wordsInput[totalWords]);

                        }


                      }
                      /* for arithmetic operation */
                      else if (atoi(wordsInput[1]) != 0) /* if 2nd word is a number */
                      {
                        if (totalWords == 3) /* operation of only 2 words are allowed and total 4 words allowed */
                        {

                            if (atoi(wordsInput[totalWords]) != 0) /* if 4th word is a number */
                            {
                               if ((strcmp(wordsInput[2],"+") != 0)&&(strcmp(wordsInput[2],"-") != 0)&&
                               (strcmp(wordsInput[2],"*") != 0)&&(strcmp(wordsInput[2],"/") != 0)&&
                               (strcmp(wordsInput[2],"%") != 0)) /* if arithmatic operator is not specified */
                                {
                                    strcat(outputToDisplay, "I don't know how to ");
                                     if (strlen(wordsInput[2])>53)
                                       {
                                           strcat(outputToDisplay, " ");
                                       }
                                       else
                                       {
                                           strcat(outputToDisplay, wordsInput[2]);

                                       }

                                    strcpy(ptrOutput, outputToDisplay);
                                    displayMessageInMessageSpace(0,14,"");

                                }
                                else /* valid input */
                                {
                                    /* checking for maximum number allowed */
                                    if ((atof(wordsInput[1]) < 999999999999999.99)&& (atof(wordsInput[2]) < 999999999999999.99))
                                    {
                                         num1 = atof(wordsInput[1]);

                                        num2 = atof(wordsInput[3]);

                                    if (strcmp(wordsInput[2],"+") == 0) /* add */
                                    {
                                        answer = num1 + num2;



                                    }
                                    else if (strcmp(wordsInput[2],"-") == 0) /* subtract */
                                    {
                                        answer = num1 - num2;


                                    }
                                    else if (strcmp(wordsInput[2],"*") == 0) /* multiply */
                                    {
                                        answer = num1 * num2;

                                    }
                                    else if (strcmp(wordsInput[2],"/") == 0) /* divide */
                                    {
                                        answer = num1 / num2;

                                    }
                                    else if (strcmp(wordsInput[2],"%") == 0) /* mod operator */
                                    {
                                        temp = (int)num1 % (int)num2;
                                    }
                                     mvprintw(21,3,"                                                                        ");
                                    if (strcmp(wordsInput[2],"%") != 0)
                                    {
                                        temp = (int)answer;
                                        tempf = answer - temp;

                                        if (tempf == 0.00f)
                                        {
                                            attron(A_BOLD);
                                            mvprintw(21,5,"%d",temp); /* display output */
                                            attroff(A_BOLD);
                                        }
                                        else
                                        {
                                            attron(A_BOLD);
                                            mvprintw(21,5,"%0.2f",answer); /* display output */
                                            attroff(A_BOLD);
                                        }


                                    }
                                    else
                                    {
                                            attron(A_BOLD);
                                            mvprintw(21,5,"%d",temp);
                                            attroff(A_BOLD);

                                    }

                                    strcat(outputToDisplay, "");
                                    strcpy(ptrOutput, outputToDisplay);
                                    displayMessageInMessageSpace(1,3,wordsInput[2]);


                                    }
                                    else /* number is greater than 9999999999999.99*/
                                    {
                                    strcat(outputToDisplay, "Numbers can not be greater than 9999999999999.99 ");

                                    strcpy(ptrOutput, outputToDisplay);
                                    displayMessageInMessageSpace(0,7,"");


                                    }


                                }


                            }
                            else /* invalid input */
                            {

                              if ((strcmp(wordsInput[3],"+") == 0)||(strcmp(wordsInput[3],"-") == 0)||
                               (strcmp(wordsInput[3],"*") == 0)||(strcmp(wordsInput[3],"/") == 0)||
                               (strcmp(wordsInput[3],"%") == 0))
                                {
                                    if ((strlen(wordsInput[2])==1))
                                    {
                                        strcat(outputToDisplay, wordsInput[3]);
                                        strcat(outputToDisplay, " doesn't like [] as inputs ");
                                        strcpy(ptrOutput, outputToDisplay);
                                        displayMessageInMessageSpace(0,12,"");
                                    }



                                }
                                else /* invalid input */
                                {

                                    strcat(outputToDisplay, "I don't know how to ");
                                     if (strlen(wordsInput[3])>53)
                                       {
                                           strcat(outputToDisplay, " ");
                                       }
                                       else
                                       {
                                           strcat(outputToDisplay, wordsInput[3]);

                                       }

                                    strcpy(ptrOutput, outputToDisplay);
                                    displayMessageInMessageSpace(0,11,"");

                                }

                            }

                        }
                        else if (totalWords == 2) /* invalid input */
                        {

                            if (((strcmp(wordsInput[2],"+") == 0)||(strcmp(wordsInput[2],"-") == 0)||
                               (strcmp(wordsInput[2],"*") == 0)||(strcmp(wordsInput[2],"/") == 0)||
                               (strcmp(wordsInput[2],"%") == 0)) && (strlen(wordsInput[2])==1))
                            {
                                strcat(outputToDisplay, wordsInput[2]);
                                strcat(outputToDisplay, " doesn't like [] as inputs ");
                                strcpy(ptrOutput, outputToDisplay);
                                displayMessageInMessageSpace(0,13,"");


                            }
                            else
                            {
                                strcat(outputToDisplay, "I don't know how to ");
                                 if (strlen(wordsInput[2])>53)
                                   {
                                       strcat(outputToDisplay, " ");
                                   }
                                   else
                                   {
                                       strcat(outputToDisplay, wordsInput[2]);

                                   }

                                strcpy(ptrOutput, outputToDisplay);
                                displayMessageInMessageSpace(0,11,"");

                            }


                        }
                        else
                        {
                            strcat(outputToDisplay, "Arithmetic operation is allowed for not more than 2 numbers  ");
                            strcat(outputToDisplay, "");
                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(0,10,"");

                        }


                      }
                      else
                      {
                            strcat(outputToDisplay, "I don't know how to ");
                            if (strlen(wordsInput[1])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[1]);

                           }

                            strcpy(ptrOutput, outputToDisplay);
                            displayMessageInMessageSpace(0,9,"");

                      }


                 }


               break;
            }


        }





}

/*
This function clears the drawing board
It doesn't take any value and doesn't return anything
*/
void cleanDrawingSpace()
{

    int i = 0, j = 0;
    for (i=3;i<DRAWINGSPACEY + 1;i++)
    {
        for (j=2;j<DRAWINGSPACEX;j++)
        {
            mvaddch(i, j, ' ');
        }

    }



}

/*
This function draws the lines when turtle moves forward or backward or diagonal
It takes address of turtle, where to move i.e. fwdOrbk i.e. y-> forward and b->backword and
number of steps to move i.e. stepsToMove as input
It returns the number of steps line moved
*/
int drawLines(Turtle* turtle,char fwdOrbk, int stepsToMove)
{
    int currentPosY = turtle->posY;
    int currentPosX = turtle->posX;
    int currentAngle = turtle->angle;
    int colorToUse = turtle->colorCode;
    int currentLineVisibility = turtle->linesVisibility;

    // in 45 degree position, when entered higher values, turtle moves strangly
    int count = 0, tempCount = 0;

    int minY = 4, maxY = 15, minX = 3, maxX = 50;
    char line = ' ';

    while (count <= stepsToMove) /* move specified steps */
    {

        switch (currentAngle) /* move turtle based on the angle */
        {
        case 45:
            {

                if (fwdOrbk == 'f') /* move forward*/
                {
                        /* 45 degree */
                        if ((currentPosY>minY)&&(currentPosX<maxX))
                        {
                         --currentPosY;
                         ++currentPosX;
                         ++tempCount;

                        }



                }
                else if (fwdOrbk == 'b') /* backward */
                {
                    /* 225 degree */
                    if ((currentPosY<maxY)&&(currentPosX>minX))
                    {
                        ++currentPosY;
                        --currentPosX;
                        ++tempCount;
                    }


                }
                line = '/';
                break;
            }
        case 90:
            {
                if (fwdOrbk == 'f') /* forward */
                {
                    ++currentPosX; /* 90 degree */
                    ++tempCount;
                    if (currentPosX > maxX)
                    {
                       currentPosX = minX;

                    }

                }
                else if (fwdOrbk == 'b') /* backward */
                {
                    --currentPosX; /* 270 degree */
                    ++tempCount;
                    if (currentPosX < minX)
                    {
                       currentPosX = maxX;

                    }

                }
                line = '-';

                break;
            }
        case 135:
            {

               if (fwdOrbk == 'f') /* forward*/
                {
                    /* 135 degree */
                    if ((currentPosY < maxY) &&(currentPosX < maxX))
                    {
                         ++currentPosY;
                        ++currentPosX;
                        ++tempCount;

                    }



                }
                else if (fwdOrbk == 'b') /* backward */
                {
                    /* 315 degree */
                    if ((currentPosY>minY)&&(currentPosX>minX))
                    {
                        --currentPosY;
                        --currentPosX;
                        ++tempCount;
                    }


                }
                line = '\\';
                break;
            }
        case 180:
            {

                if (fwdOrbk == 'f') /* forward */
                {
                    currentPosY++; /* 180 degree down */
                    ++tempCount;
                    if (currentPosY > maxY)
                    {
                       currentPosY = minY;

                    }

                }
                else if (fwdOrbk == 'b') /* backward */
                {
                    --currentPosY; /* up */
                    ++tempCount;
                    if (currentPosY < minY)
                    {
                       currentPosY = maxY;
                    }

                }
                line = '|';
                break;
            }
        case 225:
            {
                if (fwdOrbk == 'f') /* forward */
                {
                    /* 225 degree */
                    if((currentPosY<maxY)&&(currentPosX>minX))
                    {
                        ++currentPosY;
                        --currentPosX;
                        ++tempCount;
                    }



                }
                else if (fwdOrbk == 'b') /* backward */
                {
                    /* 45 degree */

                    if ((currentPosY > minY) &&(currentPosX < maxX))
                    {
                        --currentPosY;
                        ++currentPosX;
                        ++tempCount;

                    }

                }
                line = '/';
                break;

            }
        case 270:
            {

                if (fwdOrbk == 'f')
                {
                    --currentPosX; /* left */
                    ++tempCount;
                    if (currentPosX < minX)
                    {
                       currentPosX = maxX;

                    }

                }
                else if (fwdOrbk == 'b')
                {
                    ++currentPosX; /* right */
                    ++tempCount;
                    if (currentPosX > maxX)
                    {
                       currentPosX = minX;

                    }

                }
                line = '-';
                break;
            }
        case 315:
            {

                if (fwdOrbk == 'f')
                {
                    /* 315 degree */
                    if ((currentPosY>minY)&&(currentPosX>minX))
                    {
                        --currentPosY;
                    --currentPosX;
                    ++tempCount;
                    }
                }
                else if (fwdOrbk == 'b')
                {
                    /* 135 degree */
                   if ((currentPosY<maxY)&&(currentPosX<maxX))
                    {
                        ++currentPosY;
                       ++currentPosX;
                       ++tempCount;
                    }

                }
                line = '\\';
                break;
            }
        case 0:
            {


            }
        case 360:
            {

                if (fwdOrbk == 'f')
                {
                    --currentPosY; /* up */
                    ++tempCount;
                    if (currentPosY < minY)
                    {
                        currentPosY = maxY;
                    }

                }
                else if (fwdOrbk == 'b')
                {
                    ++currentPosY; /* down */
                    ++tempCount;
                    if (currentPosY > maxY)
                    {
                        currentPosY = minY;
                    }

                }
                line = '|';
                break;
            }


        }

        if (currentLineVisibility == 1)
        {
            /* draw line */
            if ((currentPosY >=minY && currentPosY <=maxY) && (currentPosX >=minX && currentPosX <=maxX))
            {
                 attron(COLOR_PAIR(colorToUse));
                 mvaddch(currentPosY, currentPosX, line);
                 attroff(COLOR_PAIR(colorToUse));

            }


        }

        ++count;

        }
        turtle->posY = currentPosY;
        turtle->posX = currentPosX;
        return tempCount;

}

void readCommandFromTheFile(char* wordsInput[], int totalWords)
{
    char outputToDisplay[70] = "";
    FILE *filePtr;
    char location[70] = "D:\\logoCommand.txt";
    char line[256];
    int count = 0;

    if (totalWords == 0) /*one word input i.e. invalid input*/
       {

           strcat(outputToDisplay, "Not enough input to ");
           strcat(outputToDisplay, wordsInput[0]);
            mvprintw(21,3,"                                                                        ");
           attron(A_BOLD);
            mvprintw(21,5,"%s",outputToDisplay);
            attroff(A_BOLD);
           displayMessageInMessageSpace(0,2,wordsInput[0]);

       }
    else if (totalWords > 1)
    {
                    strcat(outputToDisplay, "Execute only single command at a time ");
                    strcat(outputToDisplay, "");
                    mvprintw(21,3,"                                                                        ");
                    attron(A_BOLD);
                    mvprintw(21,5,"%s",outputToDisplay);
                    attroff(A_BOLD);
                    displayMessageInMessageSpace(0,8,"");



    }
    else
    {
        if (atoi(wordsInput[1]) == 0)
        {
             mvprintw(21,3,"                                                                        ");
            strcat(outputToDisplay, "I don't know how to ");
          if (strlen(wordsInput[1])>53)
           {
               strcat(outputToDisplay, " ");
           }
           else
           {
               strcat(outputToDisplay, wordsInput[1]);

           }
           attron(A_BOLD);
            mvprintw(21,5,"%s",outputToDisplay);
            attroff(A_BOLD);
           displayMessageInMessageSpace(0,1,wordsInput[1]);


        }
        else
        {
            filePtr = fopen(location,"r");
            if ( filePtr == NULL )
            {
                mvprintw(20, 4, "           ERROR                        ");
                getch();
                endwin();

            }
            else
            {
               while (fgets(line, sizeof line, filePtr) != NULL) /* read a line */
                {
                    if (count == 1)
                    {
                       // mvprintw(21,5,"%s",line);
                    }
                    count++;
                }
                fclose(filePtr);


            }


        }


    }




}
