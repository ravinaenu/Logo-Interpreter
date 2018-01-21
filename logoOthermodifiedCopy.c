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
    int oldLocY;
    int oldLocX;
    int oldAngle;
    char preCharAtBody;
    char preCharAtHead;


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
void drawLines(Turtle* turtle,char fwdOrbk, int stepsToMove);
/* This function caches the character that are present on turtle's new location */
void cacheCharAtTurtleLoc(int newPosY, int newPosX, int newPosAngle, Turtle* turtle);

int main()
{

    Turtle turtle = {TURTLEY,TURTLEX,0,1,5,1,TURTLEY,TURTLEX,0,' ',' '};

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
    } while((strcmp(userEnteredCommand,"quit"))); /* Takes the commands from the user, until user enters 'quit'*/
    /* fileOperation function is to store allUserEnteredCommands array of string to a file*/
    isGivenPermissionToStoreInFile = fileOperation(allUserEnteredCommands,count);

    if(isGivenPermissionToStoreInFile == 'y')
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

   int i=startY,j=startX;

   for(i=startY;i<sizeY;i++)
   {
        if((i==startY)||(i==sizeY-1))
        {
            for(j=startX;j<sizeX;j++)
            {
                if(isBold)
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
            for(j=startX;j<sizeX;j++)
            {
                if((j==startX)||(j==sizeX-1))
                {
                    if(isBold)
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

// Display Turtle's X position, Y position and current angle
// in Info Space
void displayInfoInInfoSpace(Turtle* turtle)
{

    // temporary cache current cursor position
    int x=0,y=0;
    getyx(stdscr,y,x);
    hideInfoFromInfoSpace();
    mvprintw(14,54," Turtle: ");

    attron(COLOR_PAIR(5));
    mvprintw(15,56," (%d, %d, %d degree)",turtle->posX,turtle->posY,turtle->angle);
    attroff(COLOR_PAIR(5));

    refresh();

    move(y,x);


}

// Hide Turtle's information like X position, Y position
// and angle in Info space
void hideInfoFromInfoSpace(void)
{
     // temporary cache current cursor position
     int x=0,y=0;
     getyx(stdscr,y,x);


     mvprintw(14,54,"         ");
     mvprintw(15,56,"                      ");
     refresh();
     move(y,x);
}

// display turtle in drawing space
void displayTurtleInDrawingSpace(Turtle* turtle)
{
    // temporary cache current cursor position
    int x=0,y=0,turtlePosY=0,turtlePosX=0,tempAngle=0, color = 0, tempOldLocY = 0, tempOldLocX = 0, tempOldAngle = 0;
    char oldLocTurtleBody = ' ', oldLocTurtleHead = ' ';
    getyx(stdscr,y,x);

    turtlePosY = turtle->posY;
    turtlePosX = turtle->posX;
    tempAngle = turtle->angle;
    color = turtle->colorCode;
    tempOldLocY = turtle->oldLocY;
    tempOldLocX = turtle->oldLocX;
    tempOldAngle = turtle->oldAngle;
    oldLocTurtleBody = turtle->preCharAtBody;
    oldLocTurtleHead = turtle->preCharAtHead;

    attron(COLOR_PAIR(color));
    attron(A_BOLD);

    switch(tempOldAngle)
       {

        case 45:
            {
               //oldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
               //oldLocTurtleHead = mvinch(tempOldLocY-1,tempOldLocX+1);
               if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#') && (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY-1, tempOldLocX+1, oldLocTurtleHead);
               }

            }
            break;

        case 90:
            {
               //oldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
               //oldLocTurtleHead = mvinch(tempOldLocY,tempOldLocX+1);

                if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#') && (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY, tempOldLocX+1, oldLocTurtleHead);
               }




            }
            break;

        case 135:
            {
              //ldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
              //ldLocTurtleHead = mvinch(tempOldLocY+1,tempOldLocX+1);
               if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#')&& (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY+1, tempOldLocX+1, oldLocTurtleHead);
               }


            }
            break;

        case 180:
            {
               //dLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
               //oldLocTurtleHead = mvinch(tempOldLocY+1,tempOldLocX);

                if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#')&& (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY+1, tempOldLocX, oldLocTurtleHead);
               }
            }
            break;

        case 225:
            {
               //  oldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
               //oldLocTurtleHead = mvinch(tempOldLocY+1,tempOldLocX-1);

               if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#')&& (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY+1, tempOldLocX-1, oldLocTurtleHead);
               }

            }
            break;

        case 270:
            {
               //  oldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
               //oldLocTurtleHead = mvinch(tempOldLocY,tempOldLocX-1);
               if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#')&& (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY, tempOldLocX-1, oldLocTurtleHead);
               }


            }
            break;

        case 315:
            {
               //  oldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
               //oldLocTurtleHead = mvinch(tempOldLocY-1,tempOldLocX-1);

               if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#')&& (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY-1, tempOldLocX-1, oldLocTurtleHead);
               }



            }
            break;

        case 360:{}

        case 0:
            {
               //  oldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
               //oldLocTurtleHead = mvinch(tempOldLocY-1,tempOldLocX);
               if(((oldLocTurtleBody != '@') && (oldLocTurtleBody != '#')&& (oldLocTurtleBody != ' ')))
               {
                   mvaddch(tempOldLocY, tempOldLocX, oldLocTurtleBody);
               }
               if(((oldLocTurtleHead != '@') && (oldLocTurtleHead != '#')&& (oldLocTurtleHead != ' ')))
               {
                  mvaddch(tempOldLocY-1, tempOldLocX, oldLocTurtleHead);
               }


            }
            break;
       }
     // 360 degree = 0degree
       switch(tempAngle)
       {

            case 45:
                {
                   //oldLocTurtleBody = mvinch(tempOldLocY,tempOldLocX);
                    //oldLocTurtleHead = mvinch(tempOldLocY-1,tempOldLocX+1);
                    mvaddch(turtlePosY-1, turtlePosX+1, '#');
                    //mvaddch(turtlePosY, turtlePosX-1, '*');
                    mvaddch(turtlePosY, turtlePosX, '@');
                    //temp = mvinch(turtlePosY-1,turtlePosX+1);


                }
                break;

            case 90:
                {
                    mvaddch(turtlePosY, turtlePosX+1, '@');
                    //mvaddch(turtlePosY-1, turtlePosX, '*');
                    mvaddch(turtlePosY, turtlePosX, '#');
                    //mvaddch(turtlePosY+1, turtlePosX, '*');

                }
                break;

            case 135:
                {
                    mvaddch(turtlePosY+1, turtlePosX+1, '@');
                    //mvaddch(turtlePosY, turtlePosX-1, '*');
                    mvaddch(turtlePosY, turtlePosX, '#');
                    //mvaddch(turtlePosY, turtlePosX+1, '*');

                }
                break;

            case 180:
                {
                    mvaddch(turtlePosY+1, turtlePosX, '@');
                    //mvaddch(turtlePosY, turtlePosX-1, '*');
                    mvaddch(turtlePosY, turtlePosX, '#');
                    //mvaddch(turtlePosY, turtlePosX + 1, '*');

                }
                break;

            case 225:
                {
                    mvaddch(turtlePosY+1, turtlePosX-1, '@');
                    //mvaddch(turtlePosY, turtlePosX-1, '*');
                    mvaddch(turtlePosY, turtlePosX, '#');
                    //mvaddch(turtlePosY, turtlePosX+1, '*');

                }
                break;

            case 270:
                {
                    mvaddch(turtlePosY, turtlePosX-1, '@');
                    //mvaddch(turtlePosY-1, turtlePosX, '*');
                    mvaddch(turtlePosY, turtlePosX, '#');
                    //mvaddch(turtlePosY+1, turtlePosX, '*');

                }
                break;

            case 315:
                {
                    mvaddch(turtlePosY-1, turtlePosX-1, '@');
                    //mvaddch(turtlePosY, turtlePosX-1, '*');
                    mvaddch(turtlePosY, turtlePosX, '#');
                    //mvaddch(turtlePosY, turtlePosX+1, '*');

                }
                break;

            case 360:{}

            case 0:
                {
                    // default 0 degree
                mvaddch(turtlePosY-1, turtlePosX, '@');
                //mvaddch(turtlePosY, turtlePosX-1, '*');
                mvaddch(turtlePosY, turtlePosX, '#');
                //mvaddch(turtlePosY, turtlePosX + 1, '*');

                }
                break;



       }

    attroff(A_BOLD);
    attroff(COLOR_PAIR(color));

    turtle->visibility =1;


    refresh();
    move(y,x);
}

// Hide turtle from drawing space
void hideTurtleFromDrawingSpace(Turtle* turtle)
{
    // temporary cache current cursor position
    int x=0,y=0,turtlePosY=0,turtlePosX=0,tempAngle=0;
    getyx(stdscr,y,x);

    turtlePosY = turtle->posY;
    turtlePosX = turtle->posX;
    tempAngle = turtle->angle;
   // 360 degree = 0degree
       switch(tempAngle)
       {

            case 45:
                {
                    mvaddch(turtlePosY-1, turtlePosX+1, ' ');
                    //mvaddch(turtlePosY, turtlePosX-1, ' ');
                    mvaddch(turtlePosY, turtlePosX, ' ');
                    //mvaddch(turtlePosY, turtlePosX+1, ' ');

                }
                break;

            case 90:
                {
                    mvaddch(turtlePosY, turtlePosX+1, ' ');
                    //mvaddch(turtlePosY-1, turtlePosX, ' ');
                    mvaddch(turtlePosY, turtlePosX, ' ');
                    //mvaddch(turtlePosY+1, turtlePosX, ' ');

                }
                break;

            case 135:
                {
                    mvaddch(turtlePosY+1, turtlePosX+1, ' ');
                    //mvaddch(turtlePosY, turtlePosX-1, ' ');
                    mvaddch(turtlePosY, turtlePosX, ' ');
                    //mvaddch(turtlePosY, turtlePosX+1, ' ');

                }
                break;

            case 180:
                {
                    mvaddch(turtlePosY+1, turtlePosX, ' ');
                    //mvaddch(turtlePosY, turtlePosX-1, ' ');
                    mvaddch(turtlePosY, turtlePosX, ' ');
                    //mvaddch(turtlePosY, turtlePosX + 1, ' ');

                }
                break;

            case 225:
                {
                    mvaddch(turtlePosY+1, turtlePosX-1, ' ');
                    //mvaddch(turtlePosY, turtlePosX-1, ' ');
                    mvaddch(turtlePosY, turtlePosX, ' ');
                    //mvaddch(turtlePosY, turtlePosX+1, ' ');

                }
                break;

            case 270:
                {
                    mvaddch(turtlePosY, turtlePosX-1, ' ');
                    //mvaddch(turtlePosY-1, turtlePosX, ' ');
                    mvaddch(turtlePosY, turtlePosX, ' ');
                    //mvaddch(turtlePosY+1, turtlePosX, ' ');

                }
                break;

            case 315:
                {
                    mvaddch(turtlePosY-1, turtlePosX-1, ' ');
                    //mvaddch(turtlePosY, turtlePosX-1, ' ');
                    mvaddch(turtlePosY, turtlePosX, ' ');
                    //mvaddch(turtlePosY, turtlePosX+1, ' ');

                }
                break;

            case 360:{}

            case 0:
                {
                    // default 0 degree
                mvaddch(turtlePosY-1, turtlePosX, ' ');
                //mvaddch(turtlePosY, turtlePosX-1, ' ');
                mvaddch(turtlePosY, turtlePosX, ' ');
                //mvaddch(turtlePosY, turtlePosX + 1, ' ');

                }
                break;



       }



    turtle->visibility =0;
    refresh();
    move(y,x);

}

// Display error or success message in Message space
// If it's an error, it will call displayErrorMessage function
// to display particular error message
// or it will display the success output
void displayMessageInMessageSpace(int isSuccess, int errorOrSuccessCode, char enteredCommandOrOutput[])
{
    // temporary cache current cursor position
    int x=0,y=0;
    getyx(stdscr,y,x);

    hideMessageFromMessageSpace();

    // success message can be success output of print command or message like
    // if input is rt45, message (in command work space):
    // assuming you mean rt45 and not rt45
    if(isSuccess)
    {
        attron(COLOR_PAIR(1));
        attron(A_BOLD);
        mvprintw(5,55," Success");
        attroff(A_BOLD);
        attroff(COLOR_PAIR(1));

        attron(COLOR_PAIR(4));
        mvprintw(5,63," :)");
        attroff(COLOR_PAIR(4));

        displaySuccessMessage(errorOrSuccessCode, enteredCommandOrOutput);

    }
    else
    {
        attron(COLOR_PAIR(2));
        attron(A_BOLD);
        mvprintw(5,55," Error");
        attroff(A_BOLD);
        attroff(COLOR_PAIR(2));

        attron(COLOR_PAIR(4));
        mvprintw(5,61," :(");
        attroff(COLOR_PAIR(4));

        displayErrorMessage(errorOrSuccessCode,enteredCommandOrOutput);
    }


    refresh();
    move(y,x);

}

// Hide Messages from Message space
void hideMessageFromMessageSpace(void)
{

   mvprintw(5,54,"                       ");
   mvprintw(6,54,"                       ");
   mvprintw(7,54,"                       ");
   mvprintw(8,54,"                       ");
   mvprintw(9,54,"                       ");

   refresh();
}

// Display Error message as per error code
void displayErrorMessage(int errorCode, char enteredCommand[])
{
   //char *tempStr;
   //char tempStr[74], count = 0;


   switch(errorCode)
   {
        case 0:
          break;

        case 1:
            attron(COLOR_PAIR(7));
            mvprintw(7,55,"I don't know how to ");
            attron(A_BOLD);
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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
            if(strlen(enteredCommand) > 19)
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



// to display success message in message space
void displaySuccessMessage(int successCode, char output[])
{
    int strSize = 0;
    switch(successCode)
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
            if(strSize <=13)
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


// process the input entered by user and display the output accordingly
void processEnteredCommandInput(char* enteredInput, int count, char** storeOutput, char* allEnteredInputs[],Turtle* turtle)
{
    int i = 0, wordCounter = 0, j = 0, lengthOfInput = 0, tempCounter = 0;
    char* wordsInInput[20]; // limitation, can store maximum 20 words

    char tempString2[75], tempString[75];
    char tempWord[70];

    hideMessageFromCommandSpace();
    attron(A_BOLD);
    mvaddch(20, 2, '?');
    attroff(A_BOLD);
    mvprintw(20,4,"%s",enteredInput);


    for (j=0 ; j<20; j++) {
        if ((wordsInInput[j] = malloc(sizeof(char) * 70)) == NULL) {
            displayMessageInMessageSpace(0,5,"");
            break;
        }
    }

    // split the string in to words
    lengthOfInput = strlen(enteredInput);
    strcpy(tempString2,enteredInput);

    // compress the multiple occurance of space
    for(i=0, j=0; tempString2[i] != '\0'; i++, j++) {
        if(tempString2[i] == ' ' && tempString2[i+1] == ' ') {
            j--;
            continue;
        }
        tempString[j] = tempString2[i];
    }
    tempString[j] = '\0';



    for(i=0;i<lengthOfInput;i++)
    {

        if(isspace(tempString[i]))
        {
            if(i != 0)
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
            tempWord[tempCounter] = tolower(tempString[i]);
            tempCounter++;
            if(i == lengthOfInput-1)
            {
                tempWord[tempCounter] = '\0';
                strcpy(wordsInInput[wordCounter], tempWord);
            }

        }


    }



    *storeOutput = getOutput(wordsInInput,wordCounter,turtle);


    // temp for testing
    //mvprintw(10,4,"%d",tempTurtle->posY);


    attron(A_BOLD);
    mvprintw(21,4,"%s",*storeOutput);
    attroff(A_BOLD);


    refresh();

    for (j=0 ; j<20; j++)
    {
       free(wordsInInput[j]);

    }


    // store command in string array
    strcpy(allEnteredInputs[count], enteredInput);
    //move(22,4);

}

void fixUI(void)
{
     // temporary cache current cursor position
    int x=0,y=0;
    getyx(stdscr,y,x);

    drawBoard(1,0,WORKSPACEY, WORKSPACEX,'|','_',7,0);
    drawBoard(18,1,COMMANDSPACEY + 18, COMMANDSPACEX + 1,'|','-',1,1);


    refresh();
    move(y,x);
}

// Hide Messages from Message space
void hideMessageFromCommandSpace(void)
{

   mvprintw(20,3,"                                                                        ");
   mvprintw(21,3,"                                                                        ");
   mvprintw(22,3,"                                                                        ");


   refresh();
}

char fileOperation(char* allEnteredInputs[], int count)
{
    char permission = ' ';
    char location[70] = "D:\\logoCommand.txt";
    FILE *filePtr;
    int i = 0;
    char **pointerToUserCommand = allEnteredInputs;
    attron(A_BOLD);
    mvprintw(21, 2, " Please enter 'y' to store all your commands in a file (D:\\logocommand.txt) ");
    attroff(A_BOLD);
    move(22,4);
    permission = getch();

    if(isalpha(permission))
    {
        permission = tolower(permission);
    }
    if(permission == 'y')
    {
        // TO-DO
        // Set the default location as desktop with a default file name
        // And display the message that file is stored on the desktop


        filePtr=fopen(location,"w");
        if(filePtr==NULL){
          // TO-DO
          // display error message
          mvprintw(20, 4, "           ERROR                        ");
          getch();
          endwin();

       } else
       {
           for(i = 0; i<count-1;i++)
           {
               fprintf(filePtr,"%s \n",pointerToUserCommand[i]);

           }
           fclose(filePtr);

       }
    }
     return permission;

}


char* getOutput(char* wordsInput[], int counter, Turtle* turtle)
{
    char* ptrChar;
    char* allowedCommands[20] = {"home","forward","fd", "back", "bk",
    "pd", "pu", "rt", "lt",  "quit", "setpencolor", "setxy", "st", "ht", "clean", "clearscreen", "cs", "print", "right", "left"};
    int i = 0, matchFound = 0;
    char tempStr[70];



    ptrChar=(char*)malloc(70 * sizeof(char));

    for(i=0;i<20;i++)
    {
        if((strcmp(allowedCommands[i],wordsInput[0]) == 0))
        {
            matchFound = 1;
            break;
        }

    }

    if(matchFound == 1)
    {
        // ADD the provision to execute multiple commands
        // create an integer array with the integer represents the command sequence
        // fill the integer array based on the command sequence
        // run the switch case in forloop by executing all commands

        // call the function to process output
        //strcpy(ptrChar,"Success");
         if(i == 18)
        {
            i = 7;
        }
        else if(i == 19)
        {
            i =8;
        }
        processOutput(ptrChar,wordsInput,i,turtle,counter);

    }
    else
    {
        if(strlen(wordsInput[0]) > 53 )
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


    return (char *)ptrChar;
    // free(ptrChar);

}


void processOutput(char* ptrOutput, char* wordsInput[], int commandNum, Turtle* turtle, int totalWords)
{

    char outputToDisplay[70] = "", outputMessageToDisplay[15] = "";
    int tempAngle = 0, enteredAngle = 0, tempTurtleVisibility = 0, i =0, tempValid = 0;
    int temp = 0, tempNewPosY = 0, tempNewPosX = 0, currentPosY = 0, currentPosX = 0;
    int minY = 4, maxY = 15, minX = 3, maxX = 50, tempCount = 0;
    int colorToUse = turtle->colorCode;
    float num1 = 0.0, num2 = 0.0, answer = 0.0, tempf = 0.0;

    // temp for testing
    // mvprintw(10,4,"%d",totalWords);

        // totalWords index start from 0, i.e. totalWords = 0 means 1 word and so on
        // LIMITATION: you can only execute single command at a time
        switch(commandNum)
        {
        case 0:   // home
            {
               if(totalWords == 0)
               {
                   hideTurtleFromDrawingSpace(turtle);
                   turtle->posY = TURTLEY;
                   turtle->posX = TURTLEX;
                   turtle->angle = 0;
                   turtle->oldLocY = TURTLEY;
                   turtle->oldLocX = TURTLEX;
                   turtle->oldAngle = 0;
                   turtle->preCharAtBody = ' ';
                   turtle->preCharAtHead = ' ';
                   displayTurtleInDrawingSpace(turtle);
                   displayInfoInInfoSpace(turtle);


                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Command Executed..");

               }
               else if(totalWords == 1)
               {
                   if(atoi(wordsInput[1]) != 0)
                   {
                       strcat(outputToDisplay, "You don't say what to do with ");
                       if(strlen(wordsInput[1])>53)
                       {
                           strcat(outputToDisplay, " ");
                       }
                       else
                       {
                           strcat(outputToDisplay, wordsInput[1]);

                       }

                       strcpy(ptrOutput, outputToDisplay);
                       displayMessageInMessageSpace(0,3,wordsInput[1]);
                   }
                   else
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
        case 1:   // forward
            {


            }
        case 2:   // fd
            {
                if(totalWords == 0)
                {
                       strcat(outputToDisplay, "Not enough input to ");
                       if(strlen(wordsInput[0])>53)
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
                else if(totalWords == 1)
                {
                   if(atoi(wordsInput[1]) == 0)
                   {
                       // here if the command is from commandList
                       // show the output:
                       // ''pu' didn't output to lt
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
                       displayMessageInMessageSpace(0,1,wordsInput[1]);
                   }
                   else
                   {
                        temp = atoi(wordsInput[1]);
                        tempTurtleVisibility = turtle->visibility;
                        hideTurtleFromDrawingSpace(turtle);
                        if(temp > 0)
                        {
                           drawLines(turtle,'f', temp);
                        }
                        else
                        {
                            temp = temp * (-1);
                            drawLines(turtle,'b', temp);

                        }


                        if(tempTurtleVisibility == 1)
                        {
                            displayTurtleInDrawingSpace(turtle);
                        }
                        displayInfoInInfoSpace(turtle);

                        strcat(outputToDisplay, "Move forward  ");
                        if(strlen(wordsInput[1])>53)
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
        case 3:   // back
            {

            }
        case 4:   // bk
            {
                if(totalWords == 0)
                {
                       strcat(outputToDisplay, "Not enough input to ");
                       if(strlen(wordsInput[0])>53)
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
                 else if(totalWords == 1)
                {
                   if(atoi(wordsInput[1]) == 0)
                   {
                       // here if the command is from commandList
                       // show the output:
                       // ''pu' didn't output to lt
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
                       displayMessageInMessageSpace(0,1,wordsInput[1]);
                   }
                   else
                   {
                        temp = atoi(wordsInput[1]);
                        tempTurtleVisibility = turtle->visibility;
                        hideTurtleFromDrawingSpace(turtle);

                        if(temp > 0)
                        {
                          drawLines(turtle,'b', temp);

                        }
                        else
                        {
                            temp = temp * (-1);
                            drawLines(turtle,'f', temp);
                        }


                        if(tempTurtleVisibility == 1)
                        {
                            displayTurtleInDrawingSpace(turtle);
                        }
                        displayInfoInInfoSpace(turtle);

                        strcat(outputToDisplay, "Move back  ");
                         if(strlen(wordsInput[1])>53)
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
        case 5:   // pd
            {
               if(totalWords == 0)
               {
                   turtle->linesVisibility = 1;
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Pen Visible..");


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
        case 6:   // pu
            {
               if(totalWords == 0)
               {
                   turtle->linesVisibility = 0;
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(1,0,"Pen Invisible..");

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
        case 7:   // rt
            {
               if(totalWords == 0)
               {

                   strcat(outputToDisplay, "Not enough input to ");
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(0,2,wordsInput[0]);

               }
               else if(totalWords == 1)
               {

                   if(atoi(wordsInput[1]) == 0)
                   {
                       // here if the command is from commandList
                       // show the output:
                       // ''pu' didn't output to rt
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
                       displayMessageInMessageSpace(0,1,wordsInput[1]);
                   }
                   else
                   {
                       enteredAngle = atoi(wordsInput[1]);
                       if((enteredAngle == 45) || (enteredAngle == 90))
                       {
                           tempAngle = (int)turtle->angle + enteredAngle;

                           if(tempAngle == 360)
                           {
                               tempAngle = 0;
                           }
                           hideTurtleFromDrawingSpace(turtle);
                           turtle->angle = tempAngle;


                           displayTurtleInDrawingSpace(turtle);
                           displayInfoInInfoSpace(turtle);

                           strcat(outputToDisplay, "to right, degree moved: ");
                           if(strlen(wordsInput[1])>53)
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
                       else
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
        case 8:   // lt
            {
               if(totalWords == 0)
               {

                   strcat(outputToDisplay, "Not enough input to ");
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(0,2,wordsInput[0]);

               }
               else if(totalWords == 1)
               {
                   if(atoi(wordsInput[1]) == 0)
                   {
                       // here if the command is from commandList
                       // show the output:
                       // ''pu' didn't output to lt
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
                       displayMessageInMessageSpace(0,1,wordsInput[1]);
                   }
                    else
                   {
                       enteredAngle = atoi(wordsInput[1]);
                       if((enteredAngle == 45) || (enteredAngle == 90))
                       {



                           tempAngle = (int)turtle->angle;
                           if(tempAngle ==0)
                           {
                               tempAngle =360;
                           }
                           tempAngle = tempAngle - enteredAngle;

                           hideTurtleFromDrawingSpace(turtle);
                           turtle->angle = tempAngle;


                           displayTurtleInDrawingSpace(turtle);
                           displayInfoInInfoSpace(turtle);

                           strcat(outputToDisplay, "to left, degree moved: ");
                           if(strlen(wordsInput[1])>53)
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
                       else
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
        case 9:   // quit
            {
                if(totalWords > 0)
               {
                    strcat(outputToDisplay, "Type only quit to exit ");
                    strcat(outputToDisplay, "");
                    strcpy(ptrOutput, outputToDisplay);
                    displayMessageInMessageSpace(0,8,"");

               }

               break;
            }
        case 10:   // setpencolor
            {
                 if(totalWords == 0)
                 {
                       strcat(outputToDisplay, "Not enough input to ");
                       strcat(outputToDisplay, wordsInput[0]);
                       strcpy(ptrOutput, outputToDisplay);
                       displayMessageInMessageSpace(0,2,wordsInput[0]);


                 }
                  else if(totalWords == 1)
                  {
                      if(atoi(wordsInput[1]) == 0)
                      {
                          // here if the command is from commandList
                           // show the output:
                           // ''pu' didn't output to lt
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
                           displayMessageInMessageSpace(0,1,wordsInput[1]);


                      }
                      else
                      {
                         temp = atoi(wordsInput[1]);
                         if(temp <= 7)
                         {
                             switch(temp)
                             {
                                 case 1:
                                {
                                    turtle->colorCode =3;
                                    strcat(outputToDisplay, "Blue pen ");
                                    break;
                                }
                            case 2:
                                {
                                    turtle->colorCode = 1;
                                    strcat(outputToDisplay, "Green pen ");
                                    break;
                                }
                            case 3:
                                {
                                    turtle->colorCode = 5;
                                    strcat(outputToDisplay, "Default Cyan pen  ");
                                    break;
                                }
                            case 4:
                                {
                                    turtle->colorCode = 2;
                                    strcat(outputToDisplay, "Red pen ");
                                    break;
                                }
                            case 5:
                                {
                                    turtle->colorCode = 6;
                                    strcat(outputToDisplay, "Magenta pen ");
                                    break;
                                }
                            case 6:
                                {
                                    turtle->colorCode = 4;
                                    strcat(outputToDisplay, "Yellow pen ");
                                    break;
                                }
                            case 7:
                                {
                                    turtle->colorCode = 7;
                                    strcat(outputToDisplay, "White pen ");
                                    break;
                                }


                             }
                               displayTurtleInDrawingSpace(turtle);
                               displayInfoInInfoSpace(turtle);

                                if(strlen(wordsInput[1])>53)
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
                         else
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
        case 11:   // setxy
            {
                 if(totalWords <= 1)
                 {
                        strcat(outputToDisplay, "Please provide x and y both the co ordinates ");
                        strcat(outputToDisplay, "");
                        strcpy(ptrOutput, outputToDisplay);
                        displayMessageInMessageSpace(0,2,wordsInput[0]);

                 }
                 else if(totalWords == 2)
                 {

                    if(atoi(wordsInput[1]) == 0)
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
                           displayMessageInMessageSpace(0,1,wordsInput[1]);
                      }
                      else if(atoi(wordsInput[2]) == 0)
                      {
                           strcat(outputToDisplay, "I don't know how to ");
                           if(strlen(wordsInput[2])>53)
                           {
                               strcat(outputToDisplay, " ");
                           }
                           else
                           {
                               strcat(outputToDisplay, wordsInput[2]);

                           }

                           strcpy(ptrOutput, outputToDisplay);
                           displayMessageInMessageSpace(0,1,wordsInput[1]);

                      }
                      else
                      {
                          tempNewPosY = (int)atoi(wordsInput[2]);
                          tempNewPosX = (int)atoi(wordsInput[1]);
                          //mvprintw(10,4,"%d",tempNewPosY);
                          //mvprintw(11,4,"%d",tempNewPosX);
                          if(((tempNewPosY < minY) || (tempNewPosY > maxY)) || ((tempNewPosX < minX) ||(tempNewPosX > maxX)))
                          {
                               strcat(outputToDisplay, "Please provide the range between X:(3 to 50), Y:(4 to 15)  ");
                               strcat(outputToDisplay,"");

                               strcat(outputMessageToDisplay,wordsInput[1]);
                               strcat(outputMessageToDisplay," ");
                               strcat(outputMessageToDisplay,wordsInput[2]);
                               strcpy(ptrOutput, outputToDisplay);
                               displayMessageInMessageSpace(0,7,outputMessageToDisplay);

                          }
                          else
                          {
                            currentPosY = turtle->posY;
                            currentPosX = turtle->posX;
                            // horizontal, vertical and diagonal

                            if(currentPosY == tempNewPosY)
                            {
                                tempTurtleVisibility = turtle->visibility;
                                hideTurtleFromDrawingSpace(turtle);
                                if(currentPosX<tempNewPosX)
                                {
                                    temp = tempNewPosX - currentPosX;
                                    for(i=currentPosX;i<tempNewPosX;i++)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(currentPosY, i, '-');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }
                                }
                                else
                                {
                                    temp = currentPosX -  tempNewPosX;
                                    for(i=currentPosX;i>tempNewPosX;i--)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(currentPosY, i, '-');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }

                                }
                            ++tempValid;

                            }
                            else if(currentPosX == tempNewPosX)
                            {
                                tempTurtleVisibility = turtle->visibility;
                                hideTurtleFromDrawingSpace(turtle);
                                if(currentPosY<tempNewPosY)
                                {
                                    temp = tempNewPosY - currentPosY;
                                    for(i=currentPosY;i<tempNewPosY;i++)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(i, currentPosX, '|');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }
                                }
                                else
                                {
                                    temp = currentPosY - tempNewPosY;
                                    for(i=currentPosY;i>tempNewPosY;i--)
                                    {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(i, currentPosX, '|');
                                         attroff(COLOR_PAIR(colorToUse));
                                    }

                                }

                                ++tempValid;

                            }
                            else
                            {
                                if(currentPosY > tempNewPosY)
                                {

                                    temp = currentPosY - tempNewPosY;
                                    if((currentPosX + temp) == tempNewPosX)
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for(i=currentPosY;i>tempNewPosY;i--)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(--currentPosY, ++currentPosX, '/');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }
                                    else if((currentPosX - temp) == tempNewPosX)
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for(i=currentPosY;i>tempNewPosY;i--)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(--currentPosY, --currentPosX, '\\');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }

                                }
                                else if(currentPosY < tempNewPosY)
                                {
                                    temp = tempNewPosY - currentPosY;
                                    if((currentPosX - temp) == tempNewPosX)
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for(i=currentPosY;i<tempNewPosY;i++)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(++currentPosY, --currentPosX, '/');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }
                                    else if((currentPosX + temp) == tempNewPosX)
                                    {
                                        tempTurtleVisibility = turtle->visibility;
                                        hideTurtleFromDrawingSpace(turtle);
                                        ++tempValid;

                                        for(i=currentPosY;i<tempNewPosY;i++)
                                        {
                                         attron(COLOR_PAIR(colorToUse));
                                         mvaddch(++currentPosY, ++currentPosX, '\\');
                                         attroff(COLOR_PAIR(colorToUse));
                                        }

                                    }


                                }



                            }


                          }

                          // check here
                          if(tempValid == 0)
                                {
                                   strcat(outputToDisplay, "It only supports Horizontal, Vertical and Diagonal movement ");
                                   strcat(outputToDisplay,"");
                                   strcat(outputMessageToDisplay,wordsInput[1]);
                                   strcat(outputMessageToDisplay," ");
                                   strcat(outputMessageToDisplay,wordsInput[2]);
                                   strcpy(ptrOutput, outputToDisplay);
                                   displayMessageInMessageSpace(0,7,outputMessageToDisplay);

                                }
                                else
                                {
                                    turtle->posY = atoi(wordsInput[2]);
                                    turtle->posX = atoi(wordsInput[1]);
                                    if(tempTurtleVisibility == 1)
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
        case 12:   // st
            {
               if(totalWords == 0)
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
        case 13:   // ht
            {
               if(totalWords == 0)
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
        case 14:   // clean
            {
               if(totalWords == 0)
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
        case 15:   // clearscreen
            {


            }
        case 16:   // cs
            {
               if(totalWords == 0)
               {
                   cleanDrawingSpace();
                   turtle->posY = TURTLEY;
                   turtle->posX = TURTLEX;
                   turtle->angle = 0;
                   turtle->visibility = 1;
                   turtle->linesVisibility = 1;
                   turtle->oldLocY = TURTLEY;
                   turtle->oldLocX = TURTLEX;
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
        case 17:   // print
            {

                 if(totalWords == 0)
                 {
                   strcat(outputToDisplay, "Not enough input to ");
                   strcat(outputToDisplay, wordsInput[0]);
                   strcpy(ptrOutput, outputToDisplay);
                   displayMessageInMessageSpace(0,2,wordsInput[0]);


                 }
                 else if(totalWords == 1)
                 {
                     if((strcmp(wordsInput[1],"word") == 0))
                     {
                           strcat(outputToDisplay, "Not enough input to ");
                           if(strlen(wordsInput[1])>53)
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
                     else if(wordsInput[1][0] == '"')
                     {

                        outputToDisplay[0] = ' ';
                        for(i=1;i<strlen(wordsInput[1]);i++)
                        {
                          outputToDisplay[i] = wordsInput[1][i];
                        }
                        outputToDisplay[strlen(wordsInput[1])] = '\0';

                        strcpy(ptrOutput, outputToDisplay);
                        displayMessageInMessageSpace(1,2,outputToDisplay);

                     }
                     else if(wordsInput[1][0] == '[')
                     {
                         if(wordsInput[1][strlen(wordsInput[1])-1] == ']')
                         {
                             outputToDisplay[0] = ' ';
                            for(i=1;i<strlen(wordsInput[1])-1;i++)
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
                     else
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
                 else if(totalWords >= 2)
                 {

                      if((strcmp(wordsInput[1],"word") == 0))
                      {
                          if(totalWords < 4)
                          {
                              if(wordsInput[2][0] == '"')
                              {
                                  if(wordsInput[3][0] == '"')
                                  {
                                        outputToDisplay[0] = ' ';
                                        for(i=1;i<strlen(wordsInput[2]);i++)
                                        {
                                            ++tempCount;
                                          outputToDisplay[tempCount] = wordsInput[2][i];
                                        }
                                        for(i=1;i<strlen(wordsInput[3]);i++)
                                        {
                                          ++tempCount;
                                          outputToDisplay[tempCount] = wordsInput[3][i];
                                        }
                                        outputToDisplay[++tempCount] = '\0';
                                        strcpy(ptrOutput, outputToDisplay);
                                        displayMessageInMessageSpace(1,2,outputToDisplay);


                                  }
                                  else
                                  {
                                      strcat(outputToDisplay, "I don't know how to ");
                                       if(strlen(wordsInput[3])>53)
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
                              else
                              {
                                    strcat(outputToDisplay, "I don't know how to ");
                                     if(strlen(wordsInput[2])>53)
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
                          else
                          {
                                strcat(outputToDisplay, "Support for word command is available for not more than 2 words ");
                                strcat(outputToDisplay, "");
                                strcpy(ptrOutput, outputToDisplay);
                                displayMessageInMessageSpace(0,8,"");

                          }


                      }
                      else if(wordsInput[1][0] == '[')
                      {
                        if(wordsInput[totalWords][strlen(wordsInput[totalWords])-1] == ']')
                        {

                            outputToDisplay[0] = ' ';
                            for(temp = 1; temp<totalWords+1;temp++)
                            {
                                for(i=0;i<strlen(wordsInput[temp]);i++)
                                {
                                   if(((temp==1)&&(i==0)) || ((temp == totalWords)&&(i==(strlen(wordsInput[temp])-1))))
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
                        else
                        {
                                strcat(outputToDisplay, "Please put the words inside [] ");
                                strcat(outputToDisplay, "");
                                strcpy(ptrOutput, outputToDisplay);
                                displayMessageInMessageSpace(0,7,wordsInput[totalWords]);

                        }


                      }
                      else if(atoi(wordsInput[1]) != 0)
                      {
                        if(totalWords == 3)
                        {

                            if(atoi(wordsInput[totalWords]) != 0)
                            {
                               if((strcmp(wordsInput[2],"+") != 0)&&(strcmp(wordsInput[2],"-") != 0)&&
                               (strcmp(wordsInput[2],"*") != 0)&&(strcmp(wordsInput[2],"/") != 0)&&
                               (strcmp(wordsInput[2],"%") != 0))
                                {
                                    strcat(outputToDisplay, "I don't know how to ");
                                     if(strlen(wordsInput[2])>53)
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
                                else
                                {
                                    if((atof(wordsInput[1]) < 999999999999999.99)&& (atof(wordsInput[2]) < 999999999999999.99))
                                    {
                                         num1 = atof(wordsInput[1]);
                                    //num1 = roundf(num1*10.00f)/10.00f;
                                    num2 = atof(wordsInput[3]);
                                    //num2 = roundf(num2*10.00f)/10.00f;
                                    if(strcmp(wordsInput[2],"+") == 0)
                                    {
                                        answer = num1 + num2;
                                       // answer = roundf(answer*10.0f)/10.0f;


                                    }
                                    else if(strcmp(wordsInput[2],"-") == 0)
                                    {
                                        answer = num1 - num2;
                                        //mvprintw(10,4,"%0.2f",answer);

                                    }
                                    else if(strcmp(wordsInput[2],"*") == 0)
                                    {
                                        answer = num1 * num2;

                                    }
                                    else if(strcmp(wordsInput[2],"/") == 0)
                                    {
                                        answer = num1 / num2;

                                    }
                                    else if(strcmp(wordsInput[2],"%") == 0)
                                    {
                                        temp = (int)num1 % (int)num2;
                                    }

                                    if(strcmp(wordsInput[2],"%") != 0)
                                    {
                                        temp = (int)answer;
                                        tempf = answer - temp;
                                        //mvprintw(10,4,"%0.2f",answer);
                                        if(tempf == 0.00f)
                                        {
                                            attron(A_BOLD);
                                            mvprintw(21,5,"%d",temp);
                                            attroff(A_BOLD);
                                        }
                                        else
                                        {
                                            attron(A_BOLD);
                                            mvprintw(21,5,"%0.2f",answer);
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
                                    else
                                    {
                                    strcat(outputToDisplay, "Numbers can not be greater than 9999999999999.99 ");

                                    strcpy(ptrOutput, outputToDisplay);
                                    displayMessageInMessageSpace(0,7,"");


                                    }


                                }


                            }
                            else
                            {

                              if((strcmp(wordsInput[3],"+") == 0)||(strcmp(wordsInput[3],"-") == 0)||
                               (strcmp(wordsInput[3],"*") == 0)||(strcmp(wordsInput[3],"/") == 0)||
                               (strcmp(wordsInput[3],"%") == 0))
                                {
                                    if((strlen(wordsInput[2])==1))
                                    {
                                        strcat(outputToDisplay, wordsInput[3]);
                                        strcat(outputToDisplay, " doesn't like [] as inputs ");
                                        strcpy(ptrOutput, outputToDisplay);
                                        displayMessageInMessageSpace(0,12,"");
                                    }



                                }
                                else
                                {

                                    strcat(outputToDisplay, "I don't know how to ");
                                     if(strlen(wordsInput[3])>53)
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
                        else if(totalWords == 2)
                        {

                            if(((strcmp(wordsInput[2],"+") == 0)||(strcmp(wordsInput[2],"-") == 0)||
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
                                 if(strlen(wordsInput[2])>53)
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


               break;
            }


        }




    //turtle->posY = TURTLEY;
    //turtle->posX = TURTLEX;
    //turtle->angle = 0;

    //mvprintw(21,4,"%d %d",turtle->posX,turtle->posY );


   // displayInfoInInfoSpace(&turtle);
    // displayTurtleInDrawingSpace(&turtle);
    //displayMessageInMessageSpace(1,0,"fowd");


}

void cleanDrawingSpace()
{
    // drawBoard(2,1,DRAWINGSPACEY + 2, DRAWINGSPACEX + 1,'|','-',4,1);
    int i = 0, j = 0;
    for(i=3;i<DRAWINGSPACEY + 1;i++)
    {
        for(j=2;j<DRAWINGSPACEX;j++)
        {
            mvaddch(i, j, ' ');
        }

    }



}

// handle negative numbers too
void drawLines(Turtle* turtle,char fwdOrbk, int stepsToMove)
{
    int currentPosY = turtle->posY, tempPosY = turtle->posY;
    int currentPosX = turtle->posX, tempPosX = turtle->posX;
    int currentAngle = turtle->angle;
    int colorToUse = turtle->colorCode;
    int currentLineVisibility = turtle->linesVisibility;

    // temp for testing
    //mvprintw(10,4,"%d",stepsToMove);


    //replace space with character while moving turtle
    // in 45 degree position, when entered higher values, turtle moves strangly
    int count = 0, tempCount = 0;
    int switchedCoY = 0, switchedCoX = 0,
        tempSwitchedY = 0, tempSwitchedX = 0;
    int minY = 4, maxY = 15, minX = 3, maxX = 50;
    char line = ' ';

    while(count <= stepsToMove)
    {

        switch(currentAngle)
        {
        case 45:
            {

                if(fwdOrbk == 'f')
                {
                        --currentPosY;
                        ++currentPosX;
                        tempCount++;
                        if(currentPosY < minY)
                        {
                           switchedCoY = ((tempPosX + (tempCount-1)) - minX);
                           switchedCoX = ((tempPosY - (tempCount-1)) - minY);
                           if(switchedCoY > (maxY-minY))
                           {
                               tempSwitchedY = (maxY-minY);
                               tempSwitchedX = switchedCoX + (switchedCoY - tempSwitchedY);
                           }
                           else
                           {
                               tempSwitchedY = switchedCoY;
                               tempSwitchedX = switchedCoX;
                           }
                           currentPosY = tempSwitchedY + minY;
                           currentPosX = tempSwitchedX + minX;
                           tempCount = 0;
                        }
                        else if((currentPosX > maxX))
                        {

                           tempCount = 0;
                        }


                }
                else if(fwdOrbk == 'b')
                {
                    ++currentPosY;
                    --currentPosX;
                    tempCount++;
                    if((currentPosY > maxY))
                    {
                       currentPosY = minY;
                       currentPosX = ((tempPosX - tempCount))+ ((maxY)- (minY))+1;
                       tempCount = 0;
                    }
                    else if(currentPosX < minX)
                    {
                        currentPosY = minY;
                        currentPosX = ((tempPosY + tempCount))- (minX)+1;
                        tempCount = 0;

                    }


                }
                line = '/';
                break;
            }
        case 90:
            {
                if(fwdOrbk == 'f')
                {
                    ++currentPosX;
                    if(currentPosX > maxX)
                    {
                       currentPosX = minX;
                    }

                }
                else if(fwdOrbk == 'b')
                {
                    --currentPosX;
                    if(currentPosX < minX)
                    {
                       currentPosX = maxX;
                    }

                }
                line = '-';

                break;
            }
        case 135:
            {

               if(fwdOrbk == 'f')
                {
                    ++currentPosY;
                    ++currentPosX;


                }
                else if(fwdOrbk == 'b')
                {
                    --currentPosY;
                    --currentPosX;

                }
                line = '\\';
                break;
            }
        case 180:
            {

                if(fwdOrbk == 'f')
                {
                    currentPosY++;
                    if(currentPosY > maxY)
                    {
                       currentPosY = minY;
                    }

                }
                else if(fwdOrbk == 'b')
                {
                    --currentPosY;
                    if(currentPosY < minY)
                    {
                       currentPosY = maxY;
                    }

                }
                line = '|';
                break;
            }
        case 225:
            {
                if(fwdOrbk == 'f')
                {
                    ++currentPosY;
                    --currentPosX;
                    tempCount++;
                    if((currentPosY > maxY))
                    {
                       currentPosY = minY;
                       currentPosX = ((tempPosX - tempCount))+ ((maxY)- (minY))+1;
                       tempCount = 0;
                    }
                    else if(currentPosX < minX)
                    {
                        currentPosY = minY;
                        currentPosX = ((tempPosY + tempCount))- (minX)+1;
                        tempCount = 0;

                    }


                }
                else if(fwdOrbk == 'b')
                {
                    --currentPosY;
                    ++currentPosX;
                    tempCount++;
                    if((currentPosY < minY) ||(currentPosX > maxX))
                    {
                       currentPosY = maxY;
                       currentPosX = ((tempPosX + tempCount)- (maxY)) + (minY)-1;
                       tempCount = 0;
                    }

                }
                line = '/';
                break;

            }
        case 270:
            {

                if(fwdOrbk == 'f')
                {
                    --currentPosX;
                    if(currentPosX < minX)
                    {
                       currentPosX = maxX;

                    }

                }
                else if(fwdOrbk == 'b')
                {
                    ++currentPosX;
                    if(currentPosX > maxX)
                    {
                       currentPosX = minX;

                    }

                }
                line = '-';
                break;
            }
        case 315:
            {

                if(fwdOrbk == 'f')
                {

                    --currentPosY;
                    --currentPosX;
                }
                else if(fwdOrbk == 'b')
                {
                    ++currentPosY;
                    ++currentPosX;

                }
                line = '\\';
                break;
            }
        case 0:
            {


            }
        case 360:
            {

                if(fwdOrbk == 'f')
                {
                    --currentPosY;
                    if(currentPosY < minY)
                    {
                        currentPosY = maxY;
                    }

                }
                else if(fwdOrbk == 'b')
                {
                    ++currentPosY;
                    if(currentPosY > maxY)
                    {
                        currentPosY = minY;
                    }

                }
                line = '|';
                break;
            }


        }

        if(currentLineVisibility == 1)
        {
            if((currentPosY >=minY && currentPosY <=maxY) && (currentPosX >=minX && currentPosX <=maxX))
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


}

void cacheCharAtTurtleLoc(int newPosY, int newPosX, int newPosAngle, Turtle* turtle)
{
    turtle->oldLocY = newPosY;
    turtle->oldLocX = newPosX;
    turtle->oldAngle = newPosAngle;
    int tempOldLocY = 0, tempOldLocX = 0;

    switch(newPosAngle)
       {

        case 45:
            {
               turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
               turtle->preCharAtHead = mvinch(tempOldLocY-1,tempOldLocX+1);


            }
            break;

        case 90:
            {
               turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
               turtle->preCharAtHead = mvinch(tempOldLocY,tempOldLocX+1);

            }
            break;

        case 135:
            {
              turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
              turtle->preCharAtHead = mvinch(tempOldLocY+1,tempOldLocX+1);


            }
            break;

        case 180:
            {
               turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
               turtle->preCharAtHead = mvinch(tempOldLocY+1,tempOldLocX);

            }
            break;

        case 225:
            {
               turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
               turtle->preCharAtHead = mvinch(tempOldLocY+1,tempOldLocX-1);

            }
            break;

        case 270:
            {
               turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
               turtle->preCharAtHead = mvinch(tempOldLocY,tempOldLocX-1);

            }
            break;

        case 315:
            {
               turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
               turtle->preCharAtHead = mvinch(tempOldLocY-1,tempOldLocX-1);

            }
            break;

        case 360:{}

        case 0:
            {
               turtle->preCharAtBody = mvinch(tempOldLocY,tempOldLocX);
               turtle->preCharAtHead = mvinch(tempOldLocY-1,tempOldLocX);

            }
            break;
       }



}

