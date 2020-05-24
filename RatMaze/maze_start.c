//-----------------------------------------
// NAME: Risto Zimbakov
//
// REMARKS: Using a backtracking algorithm to see if a
// mouse can escape from a rectangular maze. It should run
// as long as the mouse does not reach the exit or tries
// all paths possible and concludes that the mouse is trapped
//----------------------------------------

#include <stdio.h>//libraries needed
#include <stdlib.h>
#include <assert.h>


//-------------------------------------------------------------------------------------
// CONSTANTS and TYPES
//-------------------------------------------------------------------------------------

#define MAX_DIMENSION 20//largest size of maze we can take in is 20x20

// constant definitions for the different cell states
const char WALL    = '1';
const char SPACE   = '0';
const char VISITED = '.';
const char MOUSE   = 'm';
const char EXIT    = 'e';

typedef enum BOOL { false, true } Boolean;

//defines struct for a cell and CellNode as needed for program
struct CELL
{
  int row;
  int column;
};
typedef struct CELL Cell;

typedef struct CELL_NODE CellNode;
struct CELL_NODE
{
  Cell     cell;//position in list
  CellNode *next;//link to next node
};

//-------------------------------------------------------------------------------------
// VARIABLES
//-------------------------------------------------------------------------------------

CellNode *top = NULL;//setting list to initially be empty

// a 2D array used to store the maze
char maze[MAX_DIMENSION][MAX_DIMENSION];
int mazeRows;
int mazeCols;

// holds the location of the mouse and escape hatch
Cell mouse;
Cell escape;

//-------------------------------------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------------------------------------

// basic cell manipulation

// returns true if the cells are at the same position in our maze
Boolean equalCells(const Cell cell1, const Cell cell2);//added

// returns a new cell object
Cell makeCell(const int row, const int col);

// returns true if the cell is within our maze
Boolean validCell(const Cell theCell);

// routines for managing our backtracking

// returns true if there are no more cells to try
Boolean noMoreCells();

// returns the next cell to try for a path out of the maze
Cell nextCell();

// introduces a new cell to try
void addCell(const Cell cell);

void printMaze();

void loadMaze();

// returns true if there's a solution to the maze
Boolean solveMaze();

// our invariant checker
void checkState();

//-------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------

//----------------------------------------
//main
//
// PURPOSE: read input from the user which sets up dimensions
// of array that will be store maze and then tries to solve
// maze by using a backtracking algorithm to get the mouse reach
// the exit
// INPUT PARAMETERS:
//int argc - will be the number of strings pointed to by argv.
//This will be 1 plus the number of arguments,
//as virtually all implementations will prepend the
//name of the program to the array.
//char *argv[] - works with argc to determine the number
//of arguements in the program
// OUTPUT PARAMETERS:
// EXIT_SUCCESS returns 0 to show that main has come
// to the end of the file and has reached so successfully
//----------------------------------------

int main( int argc, char *argv[] )
{
    loadMaze();//first sets up the maze

    if ( solveMaze() )
      printf( "The mouse is free!!!!\n" );
    else
      printf( "The mouse is trapped!!!!\n" );

    printf( "\nEnd of processing\n" );

    return EXIT_SUCCESS;
}

//////////////////////////////////////////////
// Cell routines

//----------------------------------------
//validCell
//
// PURPOSE: Checks if cell is in within our maze at least
// so when making program later we are only performing actions
// on valid cells
// INPUT PARAMETERS:
// theCell - The cell we are checking to see if it is
// valid or not
// OUTPUT PARAMETERS:
// validCell returns a boolean that checks if
// the cell is valid or not
//----------------------------------------

Boolean validCell(const Cell theCell)
{
    assert(theCell.row>=0);//assertions that check each boundary
    assert(theCell.row<mazeRows);//if fail somewhere we will know where
    assert(theCell.column>=0);
    assert(theCell.column<mazeCols);
    Boolean isVal = false;
    if(theCell.row <= mazeRows && theCell.column <=
       mazeCols && theCell.row >= 0 && theCell.column >= 0)
    {
           isVal = true;
    }
    return isVal;
}

//----------------------------------------
//checkState
//
// PURPOSE: Important as it checks the state of our program
// at anytime and also has the ability if user types in DDEBUG
// to turn off assertions to see if our program still runs properly
// INPUT PARAMETERS:
// None
// OUTPUT PARAMETERS:
// None
//----------------------------------------
void checkState()
{
    #ifndef NDEBUG
    CellNode *current = top;
    #endif

    assert(mazeRows > 0);
    assert(mazeRows <= MAX_DIMENSION);
    assert(mazeCols > 0);
    assert(mazeCols <= MAX_DIMENSION);

    validCell(mouse);//makes sure key chars are inside maze and proper chars
    validCell(escape);

    #ifndef NDEBUG
    while (current)
    {
        validCell(current->cell);
        current = current->next;
    }

    //makes sure every cell is one of five chars so dont have random cells
    for (int row=0; row<mazeRows; row++)
    {
        for (int col=0; col<mazeCols; col++)
        {
            assert(maze[row][col]==WALL || maze[row][col]==MOUSE ||
                   maze[row][col]==EXIT || maze[row][col]==SPACE ||
                   maze[row][col]==VISITED);
        }
    }
    #endif
}

//----------------------------------------
//addCell
//
// PURPOSE: introduces new cell to list and
// by dynamically sizing the cell and adding
// it to the front of our linked list like a stack
// INPUT PARAMETERS:
// cell - The cell we are trying to add and where we are pulling the
// information from
// OUTPUT PARAMETERS:
// addCell returns nothing
//----------------------------------------
void addCell(const Cell cell)
{
    CellNode *new_node = NULL;//precaution to set null first
    new_node = malloc(sizeof(CellNode));//dynamically sizing the array
    (*new_node).cell = cell;//takes in cell info so row and col
    (*new_node).next = NULL;//precaution to set null first

    //important precondition to ensure malloc was successful
    assert(new_node != NULL);

    if(top == NULL)//if first cell
    {
        top = new_node;//top is now first cell
    }
    else
    {
        (*new_node).next = top;//make new cell point to old top
        top = new_node;//and make new cell top so add to front
    }
    checkState();//post condition to see state is in check
}

//----------------------------------------
//makeCell
//
// PURPOSE: creates a brand new Cell that
// has to be within maze and remain valid as well
// INPUT PARAMETERS:
// row - what row number we will assign to the new cell
// col - what col number we will assign to the new cell
// OUTPUT PARAMETERS:
// makeCell returns the new Cell we have created
//----------------------------------------
Cell makeCell(const int row, const int col)
{
    Cell newCell;

    //pre conditions
    assert(row>=0);
    assert(row<mazeRows);
    assert(col>=0);
    assert(col<mazeCols);

    if (row>=0 && row<mazeRows && col>=0 && col<mazeCols)
    {
        newCell.row = row;
        newCell.column = col;
    }
    else
    {
        printf("You are out of bounds when creating Cell");
    }
    //important post condition to ensure it is still valid
    validCell(newCell);
    return newCell;

}


//////////////////////////////////////////////


//////////////////////////////////////////////
// List routines

//----------------------------------------
//displayList
//
// PURPOSE: to show the contents of our linked list
// Used for debugging
// INPUT PARAMETERS:
// first - the top of the list will be needed for
// reading list
// OUTPUT PARAMETERS:
// displayList returns nothing
//----------------------------------------
void displayList(CellNode * first)
{
    CellNode* print = first;

    while(print != NULL)
    {
        printf("\t%d %d",(*print).cell.row, (*print).cell.column);
        print = (*print).next;
    }
}

//----------------------------------------
//nextCell
//
// PURPOSE: To retrieve the next Cell in
// our list and try it as a potential path to the exit
// INPUT PARAMETERS:
// no input parameters
// OUTPUT PARAMETERS:
// nextCell returns the removed first cell of our list
//----------------------------------------
Cell nextCell()
{
    checkState();//pre condition to ensure state
    CellNode* oldTop = top;//will need to ensure no memory is wasted
    Cell nextCell = (*top).cell;
    top = (*top).next;
    free(oldTop);//frees memory no longer being used
    oldTop = NULL;//makes sure it is no longer pointing to any node in list
    checkState();//state is still proper and program is functioning
    validCell(nextCell);//the cell we have created is valid
    return nextCell;
}

//----------------------------------------
//noMoreCells
//
// PURPOSE: To check if linked list still has cells
// INPUT PARAMETERS:
// no input parameters
// OUTPUT PARAMETERS:
// noMoreCells returns if linked list empty or nor
//----------------------------------------
Boolean noMoreCells()
{
    checkState();//ensures status of program
    Boolean noCells = false;
    if(top == NULL)
    {
        noCells = true;
    }
    return noCells;
}

//----------------------------------------
//equalCells
//
// PURPOSE: Used to determine if two cells are in the same spot
// in our maze. Important for finding if reached exit or not
// INPUT PARAMETERS:
// cell1 - valid cell will be used for comparison
// cell2 - also cell that will be checked to see if same spot as another
// OUTPUT PARAMETERS:
// equalCells returns if cells are in same spot or not
//----------------------------------------
Boolean equalCells(const Cell cell1, const Cell cell2)
{
    checkState();
    validCell(cell1);
    validCell(cell2);
    Boolean equality = false;

    //might have to be like this
    if(cell1.row == cell2.row && cell1.column == cell2.column)
    {
        equality = true;
    }
    return equality;

}
//////////////////////////////////////////////


//////////////////////////////////////////////
// Maze routines

//----------------------------------------
//solveMaze
//
// PURPOSE: Trying to determine if mouse can find path to
// exit using our backtracking algorithm
// INPUT PARAMETERS:
// none
// OUTPUT PARAMETERS:
// solveMaze returns boolean that checks if mouse can find any possible
// exit of given maze from starting position to exit
//----------------------------------------
Boolean solveMaze()
{
    top = NULL;//initializes  list
    Cell goalCell;
    Cell startCell;
    Cell currentCell;
    Boolean mouseFree = true;

    int i;
    int k;
    //make sure maze is valid
    assert(mazeRows>0);
    assert(mazeRows<=MAX_DIMENSION);
    assert(mazeCols>0);
    assert(mazeCols<=MAX_DIMENSION);

    //needed for when assertions are turned off
    if(mazeRows<0 || mazeRows>=MAX_DIMENSION ||
       mazeCols<0 || mazeCols>=MAX_DIMENSION)
    {
        printf("trying to solve inValid Maze");
    }

    for(i = 0; i < mazeRows; i++)
    {
        for(k = 0; k < mazeCols; k++)
        {
            if( (maze[i][k]) == EXIT )
            {
                goalCell.row = i;
                goalCell.column = k;
                validCell(goalCell);//cells created are valid
            }
            if( (maze[i][k]) == MOUSE)
            {
                startCell.row = i;
                startCell.column = k;
                validCell(startCell);
            }
        }
        checkState();//state of program ensured
    }
    currentCell = startCell;
    validCell(currentCell);//cell is still proper
    printMaze();
    do
    {
        maze[currentCell.row][currentCell.column] = VISITED;
        printMaze();
        if(maze[currentCell.row+1][currentCell.column] != VISITED &&//down
           maze[currentCell.row+1][currentCell.column] != WALL)
        {
            validCell(makeCell(currentCell.row+1,currentCell.column));//cell exists there
            addCell(makeCell(currentCell.row+1,currentCell.column));

        }

        if(maze[currentCell.row][currentCell.column+1] != VISITED &&//right
           maze[currentCell.row][currentCell.column+1] != WALL)
        {
            validCell(makeCell(currentCell.row+1,currentCell.column));
            addCell(makeCell(currentCell.row,currentCell.column+1));
        }

        if(maze[currentCell.row-1][currentCell.column] != VISITED &&//up
           maze[currentCell.row-1][currentCell.column] != WALL)
        {
            validCell(makeCell(currentCell.row+1,currentCell.column));
            addCell(makeCell(currentCell.row-1,currentCell.column));
        }

        if(maze[currentCell.row][currentCell.column-1] != VISITED &&//left
           maze[currentCell.row][currentCell.column-1] != WALL)
        {
            validCell(makeCell(currentCell.row+1,currentCell.column));
            addCell(makeCell(currentCell.row,currentCell.column-1));
        }

        if(noMoreCells())
        {
            mouseFree = false;
        }
        else
        {
            currentCell = nextCell();
            validCell(currentCell);//next cell in list is valid
        }
        checkState();//state is still ensured
    }
    while(!equalCells(currentCell,goalCell) && mouseFree);
    printf("\n");//extra blank before mouse is free or mouse trapped printed
    return mouseFree;
}

//----------------------------------------
//loadMaze
//
// PURPOSE:reads in the input from the user and initializes
// maze as it should loading all valid characters
// INPUT PARAMETERS:
// none
// OUTPUT PARAMETERS:
// loadMaze returns nothing
//----------------------------------------
void loadMaze()
{
    scanf("%d %d", &mazeRows, &mazeCols);
    //dimensions are within limitations
    assert(mazeRows>0);
    assert(mazeRows<=MAX_DIMENSION);
    assert(mazeCols>0);
    assert(mazeCols<=MAX_DIMENSION);

     //needed for when assertions are turned off
    if(mazeRows<0 || mazeRows>=MAX_DIMENSION ||
       mazeCols<0 || mazeCols>=MAX_DIMENSION)
    {
        printf("trying to load inValid Maze");
    }
    int i;
    int k;
    for(i = 0; i < mazeRows; i++)
    {
        for(k = 0; k < mazeCols; k++)
        {
            scanf(" %c", &(maze[i][k]));    // save value in array

            if(maze[i][k]!=WALL && maze[i][k]!=MOUSE &&
                maze[i][k]!=EXIT && maze[i][k]!=SPACE &&
                maze[i][k]!=VISITED)
            {
                printf("invalid char so will now be visited char instead\n");
                maze[i][k] = VISITED;//change to visited;
            }
        }
    }
}

//----------------------------------------
//printMaze
//
// PURPOSE:to output the maze after each move the mouse makes
// INPUT PARAMETERS:
// none
// OUTPUT PARAMETERS:
// printMaze returns nothing
//----------------------------------------
void printMaze()
{
    printf("\n");
    //ensures state before and after each print
    checkState();
    assert(mazeRows>0);//dimensions are valid
    assert(mazeRows<=MAX_DIMENSION);
    assert(mazeCols>0);
    assert(mazeCols<=MAX_DIMENSION);

    //when assertions turned off
    if(mazeRows<0 || mazeRows>=MAX_DIMENSION ||
       mazeCols<0 || mazeCols>=MAX_DIMENSION)
    {
        printf("trying to print inValid Maze");
    }

    int i;
    int k;
    for(i = 0; i < mazeRows; i++)
    {
        for(k = 0; k < mazeCols; k++)
        {
            printf("%c", maze[i][k]);    // print array.
        }
        printf("\n");
        checkState();
    }

}
//////////////////////////////////////////////

