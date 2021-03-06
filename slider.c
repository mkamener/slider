/* 
 * Slider. 
 * Puzzle game made by Mischka Kamener, 2014.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

/* Screen constants. */
#define SCREEN_MAX_R        21
#define SCREEN_MAX_C        80
#define L_PER_COL           6
#define PACK_COLS           2   /* Number of cols in pack select. */
#define LEVEL_COLS          9   /* Number of cols in level select. */

#define TIME_BETWEEN_FRAMES     100     /* Milliseconds. */

/* Board values. */
#define BOARD_MAX_R         20  /* Maximum rows on the board */  
#define BOARD_MAX_C         50  /* Maximum cols on the board */ 
#define EMPTY               0   /* TODO: Replace this with enumerated type */
#define WALL                1
#define GOAL                2
#define PLAYER              3
#define WEAK_WALL           6
#define BOMB_VAL            7
#define MOVING_BLOCK        8
#define HOLE                9

/* Player inputs. */
#define LEFT                'a'
#define UP                  'w'
#define RIGHT               'd'
#define DOWN                's'
#define BOMB_INPUT          'x'
#define QUIT                'q'
#define RESTART             'r'
#define PLAY                'p'
#define SAVE_LEVEL          'v'
#define CLEAR_EDITOR        'c'
#define YES                 'y'
#define NO                  'n'

/* Screen symbols. */
#define EMPTY_SYMBOL        ' '
#define WALL_SYMBOL         219
#define WEAK_WALL_SYMBOL    178
#define GOAL_SYMBOL         'X'
#define PLAYER_SYMBOL       'O'
#define MOVING_BLOCK_SYMBOL '#'
#define HOLE_SYMBOL         176
#define BOMB_SYMBOL         'B'
#define FALL_1_SYMBOL       'o'
#define FALL_2_SYMBOL       250

#define BOMB1_00            '\\'
#define BOMB1_01            '|'
#define BOMB1_02            '/'
#define BOMB1_10            '~'
#define BOMB1_11            0
#define BOMB1_12            '~'
#define BOMB1_20            '/'
#define BOMB1_21            '|'
#define BOMB1_22            '\\'

#define BOMB2_00            '`'
#define BOMB2_01            ' '
#define BOMB2_02            '\''
#define BOMB2_10            ' '
#define BOMB2_11            0
#define BOMB2_12            ' '
#define BOMB2_20            ','
#define BOMB2_21            ' '
#define BOMB2_22            '.'

/* Message constants. */
#define MAX_MSG             30
#define BOMB_DESTROYED_MSG  "Bomb was destroyed"

/* Number of levels. */
#define MAX_LEVELPACKS      10
#define MAX_LEVELS          50

/* File constants. */
#define MAX_FILE_LEN        13
#define FILE_NAME           "slider"
#define SAVE_FILE           ".sav"
#define LEVEL_FILE          ".lvl"

/* Level editor constants. */
#define CUSTOM_LEVEL_FILE   "custom"
#define CUSTOM_LEVEL_NAME   "CUSTOM"
#define CURSOR_SYMBOL       '+'
#define PADDING_COLS        3       /* Whitespace between border and first */
#define PADDING_ROWS        2       /* element. */

#define INCOMPLETE_CODE     1
#define UNBEATEN_CODE       2
#define SUCCESSFUL_CODE     3
#define QUIT_EDITOR_CODE    4
#define CLEAR_EDITOR_CODE   5

/* Other. */
#define MAX_NAME_LEN        15
#define BEATEN              1
#define ACED                2
#define TRUE                1
#define FALSE               0

/* Typedefines. */
typedef int board_t[BOARD_MAX_R][BOARD_MAX_C];

typedef struct 
{
    board_t board;              /* Level layout. */
    int     rows;               /* Number of rows in level. */
    int     cols;               /* Number of columns in level. */
    int     p_row;              /* Player starting row. */
    int     p_col;              /* Player starting column. */
    int     moves;              /* Min number of moves to beat level. */
    int     nmoves;             /* Current moves. */
    int     moving_block_check; /* True if the player is stationary, and
                                 * shouldn't be able to push a block. */
    int     bomb;               /* Bombs inventory spot. */
    int     message_available;  /* True or false variable. */
    char    message[MAX_MSG];   /* Message to be printed. */
} level_t;

typedef struct 
{
    int     data[MAX_LEVELS];
    char    sav_file[MAX_FILE_LEN];
    int     nlevels;
} save_t;

typedef struct 
{
    level_t level[MAX_LEVELS];  /* Array holding levels. */
    int     nlevels;            /* Number of levels. */
    char    name[MAX_NAME_LEN]; /* Name of levelpack. */
    save_t  save;               /* Save state. */
} levelpack_t;

typedef struct 
{
    levelpack_t pack[MAX_LEVELPACKS];
    int         npacks;
} all_packs_t;

typedef struct
{
    int     row;                  /* row on board. */
    int     col;                  /* col on board. */
} coord_t;

/*
 * Function Prototypes.
 */

/* Gameplay functions. */
void menu(all_packs_t *all_packs);
void pack_select(all_packs_t *all_packs);
void level_select(levelpack_t *levelpack);
int play(level_t *level, save_t *save, int level_num, int edit_mode);
int move(level_t *lvl, char move);
void hole(level_t *stored_lvl, level_t *lvl);
void moving_block(level_t *lvl, char direction);
void use_bomb(level_t *lvl);
void bomb_animation(level_t lvl);

/* Special screens. */
void title_screen(void);
void how_to_play(void);
void victory_screen(void);

/* Display functions. */
void disp_board_element(int board_value);
void disp_board(level_t *level);
void disp_editor(level_t *level, coord_t cursor);
void clear_screen(void);
void print_message_screen(char *msg[]);
void print_level_select(char *name, save_t save);
void print_pack_select(all_packs_t *all_packs);

/* Saving functions. */
void read_save(save_t *save);
void write_save(save_t save);
void clear_save(all_packs_t *all_packs);

/* Level pack functions. */
void get_levels(all_packs_t *all_packs);
int get_pack(levelpack_t *levelpack, FILE *fp);
int set_board(levelpack_t *levelpack);
int all_beaten(save_t save);
int moving_block_check(level_t *lvl);

/* General functions. */
void int_swap(int *p1, int *p2);
void clear(void);
void level_load_error(void);
void set_zero(int array[], int n);
void itoa_2digit(int i, char *s);

/* Level editor functions. */
void level_editor(void);
level_t create_empty_lvl(void);
void move_cursor(coord_t *cursor, char direction);
level_t crop_lvl(level_t *src_lvl);
int is_player_and_goal_valid(level_t *lvl, coord_t goal);
void write_level(level_t lvl);
void editor_message_screen(int message_code);
char editor_decision_screen(int message_code);

/*---------------------------------------------------------------------------*/
/*
 * Main Function.
 */

int
main(int argc, char *argv[])
{
    /* Set output buffer to full buffering. The size of the buffer can hold at
     * least a full screen. */
    setvbuf(stdout, NULL, _IOFBF, (SCREEN_MAX_R + 3)*(SCREEN_MAX_C + 3));
    
    /* Set levelpack. */
    all_packs_t all_packs;
    
    /* Get levels from file. */
    get_levels(&all_packs);
    
    /* Display the title screen. */
    title_screen();
    
    /* Go to menu to start gameplay. */
    menu(&all_packs);

    return 0;
}

/*---------------------------------------------------------------------------*/
/*
 * Menu screen.
 */

#define HOW_TO_PLAY     'h'
#define CLEAR_SAVE      'c'
#define LEVEL_EDITOR    'e'

void
menu(all_packs_t *all_packs) 
{
    int junk;
    char player_choice = '\0';
    
    /* Pointer to string array for menu screen. */
    char *menu[] = {
" ",
" ",
"     MENU:",
" ",
" ",
"       p:  Play game",
" ",
" ",
" ",
"       h:  How to play",
" ",
"       e:  Level editor",
" ",
"       c:  Clear completion data",
" ",
"       q:  Quit game",
    NULL};
    
    while(TRUE) 
    {
        /* Display message screen. */
        print_message_screen(menu);
        
        /* Get player input. */
        junk = scanf("%c", &player_choice);
        junk++;
        
        /* Enter sub-menu depending on choice. */ 
        if (player_choice == HOW_TO_PLAY)
        {
            how_to_play();
        }
        else if (player_choice == PLAY)
        {
            pack_select(all_packs);
        }
        else if (player_choice == LEVEL_EDITOR)
        {
            level_editor();
        }
        else if (player_choice == CLEAR_SAVE)
        {
            clear_save(all_packs);
        }
        else if (player_choice == QUIT)
        {
            return;
        }
        
    }
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Levelpack select screen.
 */

void
pack_select(all_packs_t *all_packs)
{
    char player_quit;
    int junk, pack_sel;
    
    /* Refresh level list. */
    get_levels(all_packs);
    
    while(TRUE)
    {    
        /* Display the packs available. */
        print_pack_select(all_packs);
        
        /* Set values to zero after every game. */
        pack_sel = 0;
        player_quit = '\0';
        
        /* Get player input. */
        if(scanf("%d", &pack_sel)!= 1)
        {
            junk = scanf("%c", &player_quit);
            junk++;
            
            if (player_quit == QUIT) 
            {
                return;
            }
        }
        
        /* Play game if correct input given. */
        if (   pack_sel > 0 
            && pack_sel <= (all_packs->npacks)) 
        {
            /* Move to level select screen. */
            level_select(&all_packs->pack[pack_sel-1]);
        }
    }
    
    return;
}


/*---------------------------------------------------------------------------*/
/* 
 * Level select screen. Player returns here once the current level is
 * finished.
 */

void
level_select(levelpack_t *levelpack) 
{
    char player_quit;
    int junk, level_sel;
    
    while(TRUE)
    {
        /* Display the levels available. */                
        print_level_select(levelpack->name, levelpack->save);
        
        /* Set values to zero after every game. */
        level_sel = 0;
        player_quit = '\0';
        
        /* Get player input. */
        if(scanf("%d", &level_sel)!= 1)
        {
            junk = scanf("%c", &player_quit);
            junk++;
            
            if (player_quit == QUIT) 
            {
                return;
            }
        }
        
        /* Play game if correct input given. */
        if (   level_sel > 0 
            && level_sel <= (levelpack->nlevels)) 
        {
            /* Use level_sel-1 as levels are listed to player starting from
             * 1, rather than starting from 0 as they are in the arrays. */
            play(&levelpack->level[level_sel-1], 
                &levelpack->save, level_sel-1, FALSE);
        }
    }
    
    return;
}

/*---------------------------------------------------------------------------*/
/* 
 * Opens all levelpacks. 
 */

void 
get_levels(all_packs_t *all_packs)
{
    int i, pack = 0;
    char    lvl_name[MAX_FILE_LEN],
            sav_name[MAX_FILE_LEN],
            pack_num[] = "00";
    FILE *fp;
    
    for (i = 0; i < MAX_LEVELPACKS; i++)
    {
        /* Make .lvl and .sav filenames. */
        strcpy(lvl_name, FILE_NAME);
        itoa_2digit(i, pack_num);
        strcat(lvl_name, pack_num);
        
        strcpy(sav_name, lvl_name);
        
        strcat(lvl_name, LEVEL_FILE);
        strcat(sav_name, SAVE_FILE);
        
        if (i == (MAX_LEVELPACKS - 1))
        {
            strcpy(lvl_name, CUSTOM_LEVEL_FILE);
            strcpy(sav_name, CUSTOM_LEVEL_FILE);
            strcat(lvl_name, LEVEL_FILE);
            strcat(sav_name, SAVE_FILE);
        }
        
        /* Open file. */
        if ((fp = fopen(lvl_name, "r")) != NULL)
        {
            /* Check if files are loaded successfully. */
            if (!get_pack(&all_packs->pack[pack], fp))
            {
                level_load_error();
                exit(EXIT_FAILURE);
            }    
            
            /* Close file pointer. */
            fclose(fp);
            
            /* Copy save file name to levelpack. */
            strcpy(all_packs->pack[pack].save.sav_file, sav_name);
            
            /* Copy nlevels. */
            all_packs->pack[pack].save.nlevels = 
                all_packs->pack[pack].nlevels;
            
            /* Set save file. First assume there is no save data, then look
             * to see if save data exists. */
            set_zero(all_packs->pack[pack].save.data,
                all_packs->pack[pack].nlevels);
            
            read_save(&all_packs->pack[pack].save);
            
            pack++;
        }
    }
    
    /* Set total number of levelpacks. */
    all_packs->npacks = pack;

    /* Exit if no levels have been loaded. */
    if (!pack) 
    {
        level_load_error();
        exit(EXIT_FAILURE);
    }    
}

/*---------------------------------------------------------------------------*/
/*
 * Opens a levelpack and copies the levels.
 */

int
get_pack(levelpack_t *levelpack, FILE *fp)
{
    int i, j, level = 0;
    int rows = 0, 
        cols = 0, 
        moves = 0;
            
    /* Get levelpack name. */    
    fscanf(fp, "%s", levelpack->name);
        
    /* Loop while data is available. The information about the board is
     * contained in the first row. */
    while (fscanf(fp, "%d%d%d", &rows, &cols, &moves) == 3) 
    {
        /* Check if the board is too big. */
        if (   rows > BOARD_MAX_R
            || cols > BOARD_MAX_C)
        {
            return FALSE;
        }
        
        /* Check if there are too many levels. */
        if (level >= MAX_LEVELS)
        {
            /* Previous levels can still be used. Skip reading the following
             * levels. */
            break;
        }
        
        /* Copy values into level. */
        levelpack->level[level].rows = rows;
        levelpack->level[level].cols = cols;
        levelpack->level[level].moves = moves;
        levelpack->level[level].nmoves = 0;
        levelpack->level[level].moving_block_check = 
            moving_block_check(&levelpack->level[level]);
        levelpack->level[level].bomb = 0;
        levelpack->level[level].message_available = FALSE;        
        
        for (i = 0; i < rows; i++)
        {
            for (j = 0; j < cols; j++)
            {   
                /* Get each individual level feature and copy into board. */
                fscanf(fp, "%d", 
                    &levelpack->level[level].board[i][j]);
                
                /* Get player location. */
                if (levelpack->level[level].board[i][j] == PLAYER)
                {
                    levelpack->level[level].p_row = i;
                    levelpack->level[level].p_col = j;
                }
            }
        }
        
        /* Increment counter that tracks level number. */
        level++;
    }
        
    /* Set number of levels in the level pack. */
    levelpack->nlevels = level;
    
    return TRUE;
}
    
/*---------------------------------------------------------------------------*/
/* 
 * Plays the game. First makes a local copy of the level that gets edited
 * while playing. It looks for input from the player, calls move() to move the
 * player accordingly.
 */

int
play(level_t *level, save_t *save, int level_num, int edit_mode)
{
    char direction = '\0';
    int val, check;
    
    /* Make a local copy of the chosen level, so that it can be edited
     * without changing the actual level. */
    level_t currentlvl = *level;
        
    disp_board(&currentlvl);
    
    /* Clear the input buffer. Not necessary if in edit mode. */
    if (!edit_mode)
    {
        clear();
    }
    
    /* Get input for player without displaying to the screen. */
    while ( (direction = getch()) )
    {
        check = TRUE;
        
        /* Remove messages that were previously displayed. */
        currentlvl.message_available = FALSE;        
        
        /* Another move has occurred, so increment move counter. Using a bomb
         * is not counted as a move. */
        if(    direction == LEFT
            || direction == RIGHT
            || direction == UP
            || direction == DOWN )
        {
            currentlvl.nmoves++;
        }
        
        /* Check if player has quit. */
        if (direction == QUIT)
        {
            return 0;
        }
    
        /* Check if player restarted. */
        if (direction == RESTART)
        {
            currentlvl = *level;
        }
        
        /* Check to see if player wants to usee a bomb. */
        if (direction == BOMB_INPUT)
        {
            /* Check if bomb is in the inventory. */
            if (currentlvl.bomb == TRUE)
            {
                /* Player can use bomb. */
                use_bomb(&currentlvl);
            }
        }
        
        /* Move until wall (or goal) is reached. */
        while (TRUE)
        {
            /* Insert code for using a bomb here. */
            
            val = move(&currentlvl, direction);
            
            /* Check to see if movement has ended. */
            if (val == 0)
            {
                /* Player is stationary, so moving block check is true. */
                currentlvl.moving_block_check = TRUE;
                break;
            }
            
            /* Player is moving, so they are allowed to push a moving block.
             * Therefore set moving_block_check to true. */
            currentlvl.moving_block_check = FALSE;
            
            /* Check to see if the goal has been reached */
            if (val == GOAL)
            {
                disp_board(&currentlvl);
                Sleep(1000);
                
                /* Edit save file. */
                if (!edit_mode)
                {
                    if (currentlvl.nmoves <= level->moves)
                    {
                        save->data[level_num] = ACED;
                    }
                    else if (save->data[level_num] != ACED)
                    {
                        save->data[level_num] = BEATEN;
                    }
                    write_save(*save);
                }
                    
                /* Display victory screen. */
                victory_screen();
                
                return currentlvl.nmoves;
            }
            
            /* Check to see if player has fallen in a hole. */
            if (val == HOLE)
            {   
                hole(level, &currentlvl);
                check = FALSE;
                
                break;
            }
            
            /* Check for moving block */
            if (val == MOVING_BLOCK)
            {
                moving_block(&currentlvl, direction);
                check = FALSE;
                
                /* Player is now stationary next to moved block, therefore
                 * moving_block_check is true */
                currentlvl.moving_block_check = TRUE;
                
                break;
            }
                        
            /* Display moves */
            disp_board(&currentlvl);
            Sleep(TIME_BETWEEN_FRAMES);
            
            /* Do not need to display screen again. */
            check = FALSE;
        }
        
        /* Display again if no valid move. */
        if (check)
        {
            disp_board(&currentlvl);
        }
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*/
/*
 * Takes input direction and checks if the player can move. Swaps positions if
 * a move is possible - FALSE return means no move occurred. */

int
move(level_t *lvl, char move)
{
    /* Check to see if the player has moved up. */
    if (move == UP)
    {
        /* Check to see if there is space next to player. */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == EMPTY)
        {
            /* As there is empty space next to the player, move player by
             * swapping the player and the empty space. */
            int_swap(&lvl->board[lvl->p_row-1][lvl->p_col],
                &lvl->board[lvl->p_row][lvl->p_col]);
            
            /* Adjust players position. */
            lvl->p_row -= 1;
            
            return TRUE;
        }
        
        /* Check to see if there is a wall next to the player. */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == WALL ||
            lvl->board[lvl->p_row-1][lvl->p_col] == WEAK_WALL)
        {
            return FALSE;
        }
        
        /* Check to see if the player has made it to the goal. */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == GOAL)
        {
            /* Move the player onto the goal. */
            lvl->board[lvl->p_row-1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_row -= 1;
            
            return GOAL;
        }
        
        /* Check to see if the player has fallen into a hole. */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == HOLE)
        {   
            /* Move player onto hole. */
            lvl->board[lvl->p_row-1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_row -= 1;
            
            return HOLE;
        }
        
        /* Check for a moving block. */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == MOVING_BLOCK)
        {
            /* To push a moving block, there must be space on the board,
             * and that space must be empty. */
            if (   lvl->p_row-2 >= 0
                && lvl->board[lvl->p_row-2][lvl->p_col] == EMPTY
                && lvl->moving_block_check == FALSE)
            {
                /* Push the block back by swapping its position. */
                int_swap(&lvl->board[lvl->p_row-2][lvl->p_col],
                    &lvl->board[lvl->p_row-1][lvl->p_col]);
                
                return MOVING_BLOCK;
            }
            
            /* If there is no space to push the block, it acts like a wall. */
            else
            {
                return FALSE;
            }
        }
        
        /* Check to see if the player picks up a bomb. */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == BOMB_VAL)
        {
            /* Player moves over the bomb */
            lvl->board[lvl->p_row-1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;

            /* Adjust players position. */
            lvl->p_row -= 1;            
            
            /* Can only hold maximum of one bomb. Check if bomb already in
             * inventory, and increment otherwise. */
            if (lvl->bomb == FALSE)
            {
                lvl->bomb = TRUE;
            }
            /* If bomb is already in inventory, extra bomb was destoryed so
             * display message. */
            else
            {
                lvl->message_available = TRUE;
                strcpy(lvl->message, BOMB_DESTROYED_MSG);
            }
            
            return BOMB_VAL;
        }
    }
    
    /* Check to see if the player has moved down. */
    if (move == DOWN)
    {
        /* Check to see if there is space next to player. */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == EMPTY)
        {
            /* As there is empty space next to the player, move player by
             * swapping the player and the empty space. */
            int_swap(&lvl->board[lvl->p_row+1][lvl->p_col],
                &lvl->board[lvl->p_row][lvl->p_col]);
            
            /* Adjust players position. */
            lvl->p_row += 1;
            
            return TRUE;
        }

        /* Check to see if there is a wall next to the player. */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == WALL ||
            lvl->board[lvl->p_row+1][lvl->p_col] == WEAK_WALL)
        {
            return FALSE;
        }
        
        /* Check to see if the player has made it to the goal. */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == GOAL)
        {
            /* Move the player onto the goal. */
            lvl->board[lvl->p_row+1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_row += 1;
            
            return GOAL;
        }
        
        /* Check to see if the player has fallen into a hole. */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == HOLE)
        {
            /* Move player onto hole. */
            lvl->board[lvl->p_row+1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_row += 1;
            return HOLE;
        }
        
        /* Check for a moving block. */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == MOVING_BLOCK)
        {
            /* To push a moving block, there must be space on the board,
             * and that space must be empty. */
            if (   lvl->p_row+2 < lvl->rows
                && lvl->board[lvl->p_row+2][lvl->p_col] == EMPTY
                && lvl->moving_block_check == FALSE)
            {
                /* Push the block back by swapping its position. */                
                int_swap(&lvl->board[lvl->p_row+2][lvl->p_col],
                    &lvl->board[lvl->p_row+1][lvl->p_col]);
                
                return MOVING_BLOCK;
            }
            
            /* If there is no space to push the block, it acts like a wall. */
            else
            {
                return FALSE;
            }
        }
        
        /* Check to see if the player picks up a bomb. */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == BOMB_VAL)
        {
            /* Player moves over the bomb */
            lvl->board[lvl->p_row+1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;

            /* Adjust players position. */
            lvl->p_row += 1;            
            
            /* Can only hold maximum of one bomb. Check if bomb already in
             * inventory, and increment otherwise. */
            if (lvl->bomb == FALSE)
            {
                lvl->bomb = TRUE;
            }
            /* If bomb is already in inventory, extra bomb was destoryed so
             * display message. */
            else
            {
                lvl->message_available = TRUE;
                strcpy(lvl->message, BOMB_DESTROYED_MSG);
            }
            
            return BOMB_VAL;
        }
    }
    
    /* Check to see if the player has moved left. */
    if (move == LEFT)
    {
        /* Check to see if there is space next to player. */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == EMPTY)
        {
            /* As there is empty space next to the player, move player by
             * swapping the player and the empty space. */
            int_swap(&lvl->board[lvl->p_row][lvl->p_col-1],
                &lvl->board[lvl->p_row][lvl->p_col]);
            
            /* Adjust players position. */
            lvl->p_col -= 1;
            return TRUE;
        }
        
        /* Check to see if there is a wall next to the player. */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == WALL ||
            lvl->board[lvl->p_row][lvl->p_col-1] == WEAK_WALL)
        {
            return FALSE;
        }
        
        /* Check to see if the player has made it to the goal. */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == GOAL)
        {
            /* Move the player onto the goal. */
            lvl->board[lvl->p_row][lvl->p_col-1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_col -= 1;
                        
            return GOAL;
        }
        
        /* Check to see if the player has fallen into a hole. */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == HOLE)
        {
            /* Move player onto hole. */
            lvl->board[lvl->p_row][lvl->p_col-1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_col -= 1;
            
            return HOLE;
        }
        
        /* Check for a moving block. */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == MOVING_BLOCK)
        {
            /* To push a moving block, there must be space on the board,
             * and that space must be empty. */
            if (   lvl->p_col-2 >= 0 
                && lvl->board[lvl->p_row][lvl->p_col-2] == EMPTY
                && lvl->moving_block_check == FALSE)
            {
                /* Push the block back by swapping its position. */
                int_swap(&lvl->board[lvl->p_row][lvl->p_col-2],
                    &lvl->board[lvl->p_row][lvl->p_col-1]);
                
                return MOVING_BLOCK;
            }
            
            /* If there is no space to push the block, it acts like a wall. */
            else
            {
                return FALSE;
            }
        }
        
        /* Check to see if the player picks up a bomb. */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == BOMB_VAL)
        {
            /* Player moves over the bomb */
            lvl->board[lvl->p_row][lvl->p_col-1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_col -= 1;
            
            /* Can only hold maximum of one bomb. Check if bomb already in
             * inventory, and increment otherwise. */
            if (lvl->bomb == FALSE)
            {
                lvl->bomb = TRUE;
            }
            /* If bomb is already in inventory, extra bomb was destoryed so
             * display message. */
            else
            {
                lvl->message_available = TRUE;
                strcpy(lvl->message, BOMB_DESTROYED_MSG);
            }
            
            return BOMB_VAL;
        }        
    }
    
    /* Check to see if the player has moved right. */
    if (move == RIGHT)
    {
        /* Check to see if there is space next to player. */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == EMPTY)
        {
            /* As there is empty space next to the player, move player by
             * swapping the player and the empty space. */
            int_swap(&lvl->board[lvl->p_row][lvl->p_col+1],
                &lvl->board[lvl->p_row][lvl->p_col]);
            
            /* Adjust players position. */
            lvl->p_col += 1;
            
            return TRUE;
        }
        
        /* Check to see if there is a wall next to the player. */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == WALL ||
            lvl->board[lvl->p_row][lvl->p_col+1] == WEAK_WALL)
        {
            return FALSE;
        }

        /* Check to see if the player has made it to the goal. */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == GOAL)
        {
            /* Move the player onto the goal. */
            lvl->board[lvl->p_row][lvl->p_col+1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_col += 1;
            
            return GOAL;
        }

        /* Check to see if the player has fallen into a hole. */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == HOLE)
        {
            /* Move player onto hole. */
            lvl->board[lvl->p_row][lvl->p_col+1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_col += 1;
            
            return HOLE;
        }
        
        /* Check for a moving block. */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == MOVING_BLOCK)
        {
            /* To push a moving block, there must be space on the board,
             * and that space must be empty. */
            if (lvl->p_col+2 < lvl->cols && 
                lvl->board[lvl->p_row][lvl->p_col+2] == EMPTY
                && lvl->moving_block_check == FALSE)
            {   
                /* Push the block back by swapping its position. */
                int_swap(&lvl->board[lvl->p_row][lvl->p_col+2],
                    &lvl->board[lvl->p_row][lvl->p_col+1]);
                
                return MOVING_BLOCK;
            }
            
            /* If there is no space to push the block, it acts like a wall. */
            else
            {
                return FALSE;
            }
        }
        
        /* Check to see if the player picks up a bomb. */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == BOMB_VAL)
        {
            /* Player moves over the bomb */
            lvl->board[lvl->p_row][lvl->p_col+1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            
            /* Adjust players position. */
            lvl->p_col += 1;
            
            /* Can only hold maximum of one bomb. Check if bomb already in
             * inventory, and increment otherwise. */
            if (lvl->bomb == FALSE)
            {
                lvl->bomb = TRUE;
            }
            /* If bomb is already in inventory, extra bomb was destoryed so
             * display message. */
            else
            {
                lvl->message_available = TRUE;
                strcpy(lvl->message, BOMB_DESTROYED_MSG);
            }
            
            return BOMB_VAL;
        }
    }
    
    /* If no move is valid, return FALSE */
    return FALSE;
}

/*---------------------------------------------------------------------------*/
/* 
 * Play special animation for player falling off the board, then reset the
 * level */

#define FALL_TIME 500 /* Milliseconds. */

void
hole(level_t *stored_lvl, level_t *lvl)
{
    /* Display level. This shows that the player has moved onto the hole. */    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    /* Change player icon and display the board again. */
    lvl->board[lvl->p_row][lvl->p_col] = FALL_1_SYMBOL;
    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    /* Change player icon again, and display the board again. */
    lvl->board[lvl->p_row][lvl->p_col] = FALL_2_SYMBOL;
    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    /* Replace player icon with the hole, as the player has now fallen in.
     * Display the board again. */
    lvl->board[lvl->p_row][lvl->p_col] = HOLE;
    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    /* Reset the level, and display again. */
    *lvl = *stored_lvl;
    
    disp_board(lvl);
    Sleep(TIME_BETWEEN_FRAMES);
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Special board movements for moving block.
 */

void
moving_block(level_t *lvl, char direction)
{
    /* Display the board. This shows that the player has pushed the block back
     * one spot. */
    disp_board(lvl);
    Sleep(TIME_BETWEEN_FRAMES);
    
    /* Move player again and display the level. The moving_block_check ensures
     * that the block now acts as a wall, and that the player doesn't push it
     * again. */
    move(lvl, direction);
    disp_board(lvl);
    Sleep(TIME_BETWEEN_FRAMES);
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Checks if the player is next to a moving block.
 */

int
moving_block_check(level_t *lvl)
{
    int i, j;
    
    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            if (lvl->board[lvl->p_row + i][lvl->p_col + j] == MOVING_BLOCK)
            {
                /* Moving block found. */
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*/
/*
 * Use bomb. Checks blocks surrounding player, and destorys them if possible.
 */

void
use_bomb(level_t *lvl)
{
    int i, j;
    
    /* Search 3x3 kernel centered around Player. Remove blocks if breakable. */
    for (i = -1; i <= 1; i++)
    {
        for (j = -1; j <= 1; j++)
        {
            /* Check if location on board is breakable. */
            if (lvl->board[lvl->p_row+j][lvl->p_col+i] == WEAK_WALL ||
                lvl->board[lvl->p_row+j][lvl->p_col+i] == MOVING_BLOCK)
            {
                /* Set location in level to empty. */
                lvl->board[lvl->p_row+j][lvl->p_col+i] = EMPTY;
            }
        }
    }
    
    /* Show animation of bomb */
    bomb_animation(*lvl);
    
    /* Bomb has been used, so remove it from the inventory. */
    lvl->bomb = FALSE;
    
    return;
} 

/*---------------------------------------------------------------------------*/
/*
 * Shows bomb animation. Local copy of level so that animation can be drawn
 * over level without changing board values.
 */
 
void
bomb_animation(level_t lvl)
{
    /* Animations are 3x3 grids centred around player. */
    char animation[][3][3]  = {
        { {BOMB1_00, BOMB1_10, BOMB1_20},
          {BOMB1_01, BOMB1_11, BOMB1_21},
          {BOMB1_02, BOMB1_12, BOMB1_22} },
        
        { {BOMB2_00, BOMB2_10, BOMB2_20},
          {BOMB2_01, BOMB2_11, BOMB2_21},
          {BOMB2_02, BOMB2_12, BOMB2_22} }
    };

    int n, i, j, n_animations = 2;
 
    /* Store level copy for clearing animation. */
    level_t lvl_copy = lvl;
    
    for (n = 0; n < n_animations; n++)
    {
        /* Clear animation. */
        lvl = lvl_copy;
        
        /* Search 3x3 kernel centered around Player.  */
        for (i = -1; i <= 1; i++)
        {
            for (j = -1; j <= 1; j++)
            {
                /* Check if explosion should only be displayed if squares are 
                 * empty or have holes. */
                if (lvl.board[lvl.p_row+j][lvl.p_col+i] == EMPTY ||
                    lvl.board[lvl.p_row+j][lvl.p_col+i] == HOLE)
                {
                    /* Background shouldn't be replaced if animation is the
                     * null character. */
                    if (animation[n][1+i][1+j] != 0)
                    {
                        /* Replace location with relevant explosion
                         * animation. */
                        lvl.board[lvl.p_row+j][lvl.p_col+i] 
                            = animation[n][1+i][1+j];
                    }
                }
            }
        }
    
        /* Display the board. */
        disp_board(&lvl);
    
        /* Wait so that animation can be seen. */
        Sleep(TIME_BETWEEN_FRAMES);
    }
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Title screen.
 */

void
title_screen(void)
{
    int i;
    
    /* ASCII art from patorjk.com, font name: AMC Slider. */
    char *title[] = {    
"-----------------------------------------------------------------------------",
" ",
" ",
" ",
"                                               ____                         ",
"                  ..'''' |        | |``````.  |            |`````````,      ",
"               .''       |        | |       | |______      |'''|'''''       ",
"            ..'          |        | |       | |            |    `.          ",
"      ....''             |_______ | |......'  |___________ |      `.        ",
" ",
"                                             By Mischka Kamener, 2014",
" ",
" ",
"      Adjust screen height, then press any key to play",
    NULL};
    
    /* Create enough new lines so that the termainal buffer is erased. */
    for(i = 0; i < 15; i++)
    {
        clear_screen();
    }

    /* Display the title. */
    print_message_screen(title);

    /* Wait for input. */
    getch();
    
    return;
}

/*---------------------------------------------------------------------------*/
/* 
 * Shows controls for the game.
 */

void
how_to_play(void)
{
    /* Pointer to string array for how to play screen. */
    char *how_to_play[] = {
" ",
" ",
"     HOW TO PLAY:",
" ",
"       You are Slider (O), get the tresure (X)!",
" ",
"     CONTROLS:",
" ",
"           w",
"       a    s   d   :   MOVE",
" ",
"       x:   USE BOMB",
"       q:   QUIT     r:   RESTART LEVEL",
" ",
" ",
" ",
" ",
"       Press any key to return to menu.",
    NULL};
    
    /* Display the how to play screen. */
    print_message_screen(how_to_play);
    
    /* Wait for input to exit screen. */
    getch();
}

/*---------------------------------------------------------------------------*/
/*
 * Checks board value against symbol type, and prints the appropriate one.
 */

void
disp_board_element(int board_value)
{
    /* Empty space. */
    if(board_value == EMPTY)
    {
        putchar(EMPTY_SYMBOL);
    } 
            
    /* Wall. */ 
    else if(board_value == WALL)
    {
        putchar(WALL_SYMBOL);
    }
            
    /* Goal. */
    else if(board_value == GOAL)
    {
        putchar(GOAL_SYMBOL);
    }
            
    /* Player. */
    else if(board_value == PLAYER)
    {
        putchar(PLAYER_SYMBOL);
    }
            
    /* Moving block. */
    else if(board_value == MOVING_BLOCK)
    {
        putchar(MOVING_BLOCK_SYMBOL);
    } 
            
    /* Hole. */
    else if(board_value == HOLE)
    {
        putchar(HOLE_SYMBOL);
    }
            
    /* Weak wall. */
    else if(board_value == WEAK_WALL)
    {
        putchar(WEAK_WALL_SYMBOL);
    }
            
    /* Bomb. */
    else if(board_value == BOMB_VAL)
    {
        putchar(BOMB_SYMBOL);
    }
                      
    /* All other values are printed using their character value. */
    else
    {
        putchar(board_value);
    }

    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Displays the current board.
 */

void 
disp_board(level_t *level)
{
    int i, j;

    /* Clear the screen. */
    clear_screen();
    
    /* If there is a message, display it here above the board. */
    if (level->message_available == TRUE)
    {
        /* Centre board columns. Pad screen on the left depending on the board
         * size. */
        for(j = 0; j < ((SCREEN_MAX_C - level->cols) / 4); j++)
        {
            printf(" ");
        }
        
        /* Print message. */
        printf("%s\n", level->message);
    }
    
    /* Loop through all screen elements */ 
    for(i = 0; i < level->rows; i++)
    {    
        /* Centre board columns. Pad screen on the left depending on the board
         * size. */
        for(j = 0; j < ((SCREEN_MAX_C - level->cols) / 4); j++)
        {
            printf(" ");
        }
        
        /* Print board elements. */
        for(j = 0; j < level->cols; j++)
        {
            disp_board_element( level->board[i][j] );                        
        }
        
        /* Display move count on first row. */
        if (i == 0)
        {
            printf(" %2d", level->nmoves); 
            
            /* Display bomb symbol if bomb is in inventory. */
            if (level->bomb)
            {
                printf("  %c", BOMB_SYMBOL);
            }
        }
        
        /* Display game instructions on right of board, only if the board is
         * big enough. */
        if(level->rows > 6)
        {
            if (   i == level->rows - 5)
            {
                printf("  MOVE     = %c%c%c%c", UP, LEFT, DOWN, RIGHT);
            }
            /* Only display bomb help when bomb is available. */
            if (i == level->rows-4 && level->bomb)
            {
                printf("  USE BOMB = %c", BOMB_INPUT);
            }
            
            if (i == level->rows-2)
            {
                printf("  RESTART  = %c", RESTART);
            }
        
            if (i == level->rows-1)
            {
                printf("  QUIT     = %c", QUIT);
            }
        }
        
        printf("\n");
    }
    
    /* Centre board rows. */
    for(i = 0; i < ((SCREEN_MAX_R - (level->rows)) / 2); i++)
    {
            printf("\n");
    }
    
    /* Flush output to screen. */
    fflush( stdout );
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Prints victory screen - treasure chest art taken from 
 * ascii.co.uk/art/treasure.
 */

void 
victory_screen(void)
{
    /* ASCII art from patorjk.com, font name: Slant. */
    char *success[] = {
" ",
" ",
"         _____ __  ______________________________ __",
"        / ___// / / / ____/ ____/ ____/ ___/ ___// /",
"        \\__ \\/ / / / /   / /   / __/  \\__ \\\\__ \\/ /",
"       ___/ / /_/ / /___/ /___/ /___ ___/ /__/ /_/",
"      /____/\\____/\\____/\\____/_____//____/____(_)",
" ",
"      Press any key to continue              _",
"                                            ( )         ()     __________",
"                                            ||  ,-**-,  ||    /\\____;;___\\",
"                                            \\\\*'      '*//   | /         /",
"                                             *    $  $  *    `. ())oo() .",
"                                             *     __   *     |\\(%()*^^()^\\",
"                                              *,   \\/ ,*     %| |-%-------|",
"                                                `*`*`*      % \\ | %  ))   |",
"                                                            %  \\|%________|",
"                                                             %%%%",
    NULL};

    print_message_screen(success);
    
    /* Wait for player to enter key. */
    getch();
    
    return;
}
    
/*---------------------------------------------------------------------------*/
/*
 * Clears the screen by printing new lines.
 */

void
clear_screen(void)
{
    int i;
    for(i = 0; i < SCREEN_MAX_R; i++)
    {
        printf("\n");
    }
    
    /* Flush output to screen. */
    fflush( stdout );
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Prints an array of strings, and then prints remaining new lines. Used with
 * permission from Alistair Moffatt. */

void
print_message_screen(char *msg[])
{
    int i, new_lines = 0;
    
    /* Clear the screen */
    clear_screen();
    
    /* Array of pointers has NULL as final element. This is used as a sentinel
     * to determine when to finish printing. */
    while(*msg)
    {
        printf("%s\n", *msg);
        
        /* Move along in the array. */
        msg++;
        
        /* Count the number of lines printed. */
        new_lines++;
    }
    
    for(i = 1; i <= (SCREEN_MAX_R - new_lines); i++)
    {
        printf("\n");
    }
    
    /* Flush output to screen. */
    fflush( stdout );
    
    return;
}

/*---------------------------------------------------------------------------*/
/* 
 * Prints pack select screen.
 */

void 
print_pack_select(all_packs_t *all_packs)
{
    int i, j, k, beaten = 0;

    /* Clear the screen. */
    clear_screen();
    
    /* TODO: Size of level pack select screen is hard coded, needs to adjust
     * according to SCREEN_MAX_R. */
    printf("\n\n");
    printf("     LEVEL SELECT:\n\n\n");
    
    for (i = 0; i < L_PER_COL; i++)
    {
        /* Start each row with a blank space. */
        printf("    ");
        
        for (j = 0; j < PACK_COLS; j++)
        {
            /* Check if the number of packs means that the pack list will flow
             * over to the jth column. */
            if (all_packs->npacks > i + j*L_PER_COL)
            {
                /* Get level pack beaten status. */
                beaten = all_beaten(all_packs->
                    pack[i + j*L_PER_COL].save);
                
                if (beaten == ACED)
                {
                    printf(" *");
                } 
                else if (beaten == BEATEN)
                {
                    printf(" ");
                    putchar(248);    /* Degrees symbol. */
                }
                else
                {
                    printf("  ");
                }
                
                /* Print level pack number. Add one due to array notation
                 * starting at zero. */
                printf("%2d:  ", i + j*L_PER_COL + 1);
                
                /* Print level pack name, and get length of the name. */
                printf("%s", all_packs->pack[i + j*L_PER_COL].
                    name);
                k = strlen(all_packs->pack[i + j*L_PER_COL].
                    name);
                
                /* Pad the name with remaining spaces. */
                while (k < MAX_NAME_LEN)
                {
                    printf(" ");
                    k++;
                }                
            }
        }
        
        printf("\n\n");
    }

    printf("\n       q:  Back to Menu\n\n\n");
    
    /* Flush output to screen. */
    fflush( stdout );
    
    return;
}

/*---------------------------------------------------------------------------*/
/* 
 * Prints level select screen depending on the number of available levels.
 */

void
print_level_select(char *name, save_t save)
{
    int i, j;    

    /* Clears the screen. */
    clear_screen();
    
    /* TODO: Size of level select screen is hard coded, needs to adjust
     * according to SCREEN_MAX_R. */ 
    printf("\n\n");
    
    /* Print name of level pack. */
    printf("     %s:\n\n\n", name);
    
    for (i = 0; i < L_PER_COL; i++)
    {
        for (j = 0; j < LEVEL_COLS; j++)
        {
            /* Check if there are enough levels that one appears in the jth
             * column of the ith row. */
            if (save.nlevels > i + j*L_PER_COL)
            {
                /* Print level beaten status. */
                if (save.data[i + (j*L_PER_COL)] == ACED)
                {
                    printf("     *");
                } 
                else if (save.data[i + (j*L_PER_COL)] 
                    == BEATEN)
                {
                    printf("     ");
                    putchar(248);    /* Degrees symbol. */
                }
                else
                {
                    printf("      ");
                }
                
                /* Print the level number, right alligned. Add one due to
                 * array notation starting at zero. */
                printf("%-2d", i + j*L_PER_COL + 1);
            }
        }
        
        printf("\n\n");
    }
    
    printf("\n       q:  Back to Menu\n\n\n");
    
    /* Flush output to screen. */
    fflush( stdout );
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Returns BEATEN if all levels in levelpack have been beaten, returns ACED
 * if all levels have been aced.
 */

int all_beaten(save_t save)
{
    int i;
    
    /* Check all levels to see if they have been beaten. */
    for (i = 0; i < save.nlevels; i++)
    {
        /* If any level hasn't been beaten, then levelpack hasn't been beaten
         * yet. Return FALSE */
        if (save.data[i] == 0)
        {
            return FALSE;
        }
    }
    
    /* All levels have been beaten. Now check all levels again to see if all
     * of them have been ACED. */
    for (i = 0; i < save.nlevels; i++)
    {
        /* If a level hasn't been ACED, then the levelpack as a whole has only
         * been beaten. */
        if (save.data[i] == BEATEN)
        {
            return BEATEN;
        }
    }
    
    /* All levels have been checked, and they have all been ACED. */
    return ACED;
}

/*---------------------------------------------------------------------------*/
/*
 * Reads the save file, and loads information.
 */

void
read_save(save_t *save)
{    
    int i = 0, d =0;
    FILE *fp = NULL;

    /* Open save file. */
    fp = fopen(save->sav_file, "r");
    
    /* Return early if save file didn't open correctly. */
    if(fp == NULL)
    {
        return;
    }
    
    /* Keep loading save information until file is empty. */
    while(fscanf(fp, "%d", &d) != EOF )
    {
        /* Only keep save information if it can be attributed to a level. */ 
        if (i < save->nlevels)
        {
            if (d == ACED)
            {
                save->data[i] = ACED;
                i++;
            }
            else if (d == BEATEN)
            {
                save->data[i] = BEATEN;
                i++;
            }
            else if (d == FALSE)
            {
                save->data[i] = FALSE;
                i++;
            }
        }
        
    }
    
    fclose(fp);
    
    return;
}

/*---------------------------------------------------------------------------*/
/* 
 * Replaces the save file with what is currently on the game.
 */

void
write_save(save_t save)
{
    int i;
    FILE *fp = NULL;
        
    fp = fopen(save.sav_file, "w");
    
    /* Return early if save file didn't open correctly. */
    if(fp == NULL)
    {
        return;
    }
    
    /* Loop through all save levels and print their completion state. */
    for (i = 0; i < save.nlevels; i++)
    {
        if (save.data[i] == ACED)
        {
            fprintf(fp, "%d", ACED);
        }
        else if (save.data[i] == BEATEN)
        {
            fprintf(fp, "%d", BEATEN);
        }
        else
        {
            fprintf(fp, "%d", FALSE);
        }
        
        /* New line allows each value to be read individually. */
        fprintf(fp, "\n");
    }
    
    fclose(fp);
    
    return;
}

/*---------------------------------------------------------------------------*/
/* 
 * Prints prompt if player actually wants to erase data, removes save data
 * if yes */

void 
clear_save(all_packs_t *all_packs)
{
    int junk, i;
    char player_choice = '\0';
    
    /* Pointer to string array for menu screen. */
    char *clear_save[] = {
" ",
" ",
"     CLEAR COMPLETION DATA:",
" ",
" ",
"       Are you sure you want to clear all completion data? (y/n)",
" ",
"       Completion status cannot be recovered",
"  ",
    NULL};
    
    char *clear_success[] = {
" ",
" ",
" ",
" ",
" ",
" ",
"                      COMPLETION DATA SUCCESSFULLY CLEARED",
    NULL};
    
    while(TRUE)
    {
        /* Display the screen. */    
        print_message_screen(clear_save);
        
        /* Get player input. */
        junk = scanf("%c", &player_choice);
        junk++;
        
        if (player_choice == YES)
        {
            /* Clear all packs. */
            for (i = 0; i < all_packs->npacks; i++)
            {
                set_zero(all_packs->pack[i].save.data,
                    all_packs->pack[i].nlevels);
                
                write_save(all_packs->pack[i].save);
            }
            
            /* Clearing save is always successful, display success. */
            print_message_screen(clear_success);
            
            Sleep(2000);  /* 2 seconds. */    
            
            return;
        }
        else if (player_choice == NO)
        {
            return;
        }
    }
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Swaps two integers passed through as pointers.
 */

void
int_swap(int *p1, int *p2)
{
    int temp;
    temp = *p1;
    *p1 = *p2;
    *p2 = temp;
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Prints level load error.
 */

void
level_load_error(void)
{
    
    char *error[] = {
" ",
" ",
" ",
" ",
" ",
" ",
"                      ERROR IN LOADING LEVELS",
" ",
" ",
    NULL};

    print_message_screen(error);
    
    return;
}

/*---------------------------------------------------------------------------*/
/* clears input buffer */

void
clear(void)
{ 
    while (getchar() != '\n');
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Sets interger array to be zero.
 */

void 
set_zero(int array[], int n)
{
    int i;

    for (i = 0; i < n; i++)
    {
        /* Set each element up to n to be zero. */
        array[i] = 0;
    }
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Converts a positive two digit number into a string.
 */

void
itoa_2digit(int n, char *s)
{
    int i = 0;
    char tens = '\0', ones = '\0';
    
    /* Add interger value to zero character. */
    ones = n%10 + '0';    
    n /= 10;
    tens = n%10 + '0';
    
    /* Only use tens column if it is non zero. Incrememtning position in array
     * is skipped if tens column is zero, and therefore leading zeros are
     * ignored. */
    if (tens != '0')
    {
        s[i++] = tens;
    }
    
    /* Add ones and increment. */
    s[i++] = ones;
    
    /* Close string. */
    s[i] = '\0';
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Level editor interface. User can navigate around level, place board
 * elements, test level, and then save the level.
 */
 
void
level_editor(void)
{
    char input;
    int number_input;
    int has_saved = TRUE;       /* True when no blocks have been placed since
                                 * last save, or when level is empty. */
    save_t dummy_save;
    
    /* Create empty level, with maximum size. */
    level_t lvl = create_empty_lvl();
    level_t cropped_lvl;
    
        /* Set cursor inside boundary. */
    coord_t cursor = {lvl.rows / 2, lvl.cols / 2}, goal = {0, 0};
    
    /* Display screen. */
    disp_editor(&lvl, cursor);
        
    /* Clear the input buffer. */
    clear();
    
    /* Get input from user. */
    while( (input = getch()) )
    {
        /* Get the int value from input (if applicable). */
        number_input = input - '0';
        
        /* Check for quit. */
        if (input == QUIT)
        {
            /* If the level has not beed changed since last save, do not need
             * to ask if user wants to quit. */
            if (has_saved)
            {
                return;
            }
            /* Check if user really wants to quit, as progress will be lost. */
            else if (editor_decision_screen(QUIT_EDITOR_CODE) == YES)
            {
                /* Exit the editor. */
                return;
            }
        }
        
        /* Move the cursor if a direction is input  */
        move_cursor(&cursor, input);
              
        /* Place a board elements. */
        if (number_input == PLAYER)
        {
            has_saved = FALSE;
            
            /* Remove old player icon if aplicable. */
            if (lvl.board[lvl.p_row][lvl.p_col] == PLAYER)
            {
                lvl.board[lvl.p_row][lvl.p_col] = EMPTY;
            }
            
            /* Set new player icon. */
            lvl.p_row = cursor.row;
            lvl.p_col = cursor.col;
            lvl.board[lvl.p_row][lvl.p_col] = PLAYER;            
        }
        
        if (number_input == GOAL)
        {
            has_saved = FALSE;
            
            /* Remove old goal icon if aplicable. */
            if (lvl.board[goal.row][goal.col] == GOAL)
            {
                lvl.board[goal.row][goal.col] = EMPTY;
            }
            
            /* Set new goal icon. */
            goal.row = cursor.row;
            goal.col = cursor.col;
            lvl.board[goal.row][goal.col] = GOAL;  
        }
        
        /* All other inputs. */
        if (number_input == EMPTY ||
            number_input == WALL ||
            number_input == WEAK_WALL ||
            number_input == BOMB_VAL ||
            number_input == MOVING_BLOCK ||
            number_input == HOLE)
        {
            has_saved = FALSE;
            
            lvl.board[cursor.row][cursor.col] = number_input;
        }
            
        /* Test the level. */
        if (input == PLAY)
        {
            if (is_player_and_goal_valid(&lvl, goal))
            {
                cropped_lvl = crop_lvl(&lvl);
                play(&cropped_lvl, &dummy_save, 0, TRUE);
            }
            else
            {
                /* Level was incomplete, display error. */
                editor_message_screen(INCOMPLETE_CODE);
            }
        }
        
        /* Save the level. */
        if (input == SAVE_LEVEL)
        {
            if (is_player_and_goal_valid(&lvl, goal))
            {
                /* Player first needs to beat the level, this sets moves. */
                cropped_lvl = crop_lvl(&lvl);
                cropped_lvl.moves = play(&cropped_lvl, &dummy_save, 0, TRUE);
            
                if (cropped_lvl.moves > 0) 
                {
                    write_level(cropped_lvl);
                    editor_message_screen(SUCCESSFUL_CODE); 
                    
                    has_saved = TRUE;
                }
                else
                {
                    /* Print message saying that levels needs to be
                     * beaten to be saved. */
                    editor_message_screen(UNBEATEN_CODE);
                }
            }
            else
            {
                /* Level was incomplete, display error. */
                editor_message_screen(INCOMPLETE_CODE);
            }
        }
        
        /* Check if the user wants to clear the current level. */
        if (input == CLEAR_EDITOR)
        {
            /* If the level has not beed changed since last save, do not need
             * to ask if user wants to clear it. */
            if (has_saved)
            {
                lvl = create_empty_lvl();
                has_saved = TRUE;
            }
            /* Check if user really wants to quit, as progress will be lost. */
            else if (editor_decision_screen(CLEAR_EDITOR_CODE) == YES)
            {
                lvl = create_empty_lvl();
                has_saved = TRUE;
            }
        }
        
        disp_editor(&lvl, cursor);
        Sleep(TIME_BETWEEN_FRAMES);
    }
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Creates an empty level ready to be edited.
 */

level_t
create_empty_lvl(void)
{
    int i, j;
    
    level_t lvl;
    
    /* Loop through all the board elements */
    for (i = 0; i < BOARD_MAX_R; i++)
    {
        for (j = 0; j < BOARD_MAX_C; j++)
        {
            /* Boarder values set as holes for default. */
            if (i == 0 ||
                i == BOARD_MAX_R - 1 ||
                j == 0 ||
                j == BOARD_MAX_C - 1)
            {
                lvl.board[i][j] = HOLE;
            }
            /* All inner board values set to empty by default. */
            else
            {
                lvl.board[i][j] = EMPTY;
            }
        }
    }
    
    /* Set board values. */
    lvl.rows = BOARD_MAX_R;
    lvl.cols = BOARD_MAX_C;
    lvl.p_row = 0;
    lvl.p_col = 0;
    lvl.moves = 0;
    lvl.nmoves = 0;
    lvl.moving_block_check = FALSE;
    lvl.bomb = FALSE;
    lvl.message_available = FALSE;
    
    return lvl;
}

/*---------------------------------------------------------------------------*/
/*
 * Moves the cursor in the level editor. The cursor cannot move onto the
 * boundary of the level.
 */
 
void
move_cursor(coord_t *cursor, char direction)
{
    /* Need space above to move cursor UP. */
    if (direction == UP &&
        cursor->row > 1)
    {
        /* Subtract a row. */
        cursor->row--;
        return;
    }
    
    /* Need space below to move cursor DOWN. */
    if (direction == DOWN &&
        cursor->row < BOARD_MAX_R - 2)
    {
        /* Add a row. */
        cursor->row++;
        return;
    }
    
    /* Need space left to move cursor LEFT. */
    if (direction == LEFT &&
        cursor->col > 1)
    {
        /* Subtract a column. */
        cursor->col--;
        return;
    }
    
    /* Need space right to move cursor RIGHT. */
    if (direction == RIGHT &&
        cursor->col < BOARD_MAX_C - 2)
    {
        /* Add a column. */
        cursor->col++;
        return;
    }
        
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Displays the level editor interface.
 */
 
void
disp_editor(level_t *level, coord_t cursor)
{
    int i, j;

    /* Clear the screen. */
    clear_screen();
    
    /* If there is a message, display it here above the board. */
    if (level->message_available == TRUE)
    {
        /* Centre board columns. Pad screen on the left depending on the board
         * size. */
        for(j = 0; j < ((SCREEN_MAX_C - level->cols) / 4); j++)
        {
            printf(" ");
        }
        
        /* Print message. */
        printf("%s\n", level->message);
    }
    
    /* Loop through all screen elements */ 
    for(i = 0; i < level->rows; i++)
    {    
        /* Centre board columns. Pad screen on the left depending on the board
         * size. */
        for(j = 0; j < ((SCREEN_MAX_C - level->cols) / 4); j++)
        {
            printf(" ");
        }
        
        /* Print board elements. */
        for(j = 0; j < level->cols; j++)
        {
            /* Display cursor over the board. */
            if( i == cursor.row && j == cursor.col )
            {
                putchar(CURSOR_SYMBOL);
            }
            else
            {
                disp_board_element( level->board[i][j] );
            }
        }
        
        /* Display editor instructions on right of board, only if the board is
         * big enough. */
        if(level->rows >= 15)
        {
            if (i == 1)
            {
                printf("  %c = %d", EMPTY_SYMBOL, EMPTY);
                printf("    %c = %d", WALL_SYMBOL, WALL);
            }
            
            if (i == 3)
            {
                printf("  %c = %d", GOAL_SYMBOL, GOAL);
                printf("    %c = %d", PLAYER_SYMBOL, PLAYER);
            }
            
            if (i == 5)
            {
                printf("  %c = %d", WEAK_WALL_SYMBOL, WEAK_WALL);
                printf("    %c = %d", BOMB_SYMBOL, BOMB_VAL);
            }
            
            if (i == 7)
            {
                printf("  %c = %d", MOVING_BLOCK_SYMBOL, MOVING_BLOCK);
                printf("    %c = %d", HOLE_SYMBOL, HOLE);
            }
            
            if (i == level->rows - 7)
            {
                printf("  MOVE     = %c%c%c%c", UP, LEFT, DOWN, RIGHT);
            }
        
            if (i == level->rows-5)
            {
                printf("  PLAY     = %c", PLAY);
            }
            
            if (i == level->rows-4)
            {
                printf("  SAVE     = %c", SAVE_LEVEL);
            }

            if (i == level->rows-2)
            {
                printf("  CLEAR    = %c", CLEAR_EDITOR);
            }
            
            if (i == level->rows-1)
            {
                printf("  QUIT     = %c", QUIT);
            }
        }
        
        printf("\n");
    }
    
    /* Centre board rows. */
    for(i = 0; i < ((SCREEN_MAX_R - (level->rows)) / 2); i++)
    {
            printf("\n");
    }
    
    /* Flush output to screen. */
    fflush( stdout );
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Crop level. This crops the maximum sized level from the level editor, to the
 * minimum size needed to play the level, with padding around the sides if
 * possible.
 */
level_t
crop_lvl(level_t *src_lvl)
{
    int i, j;
    int min_row = src_lvl->rows - 2, min_col = src_lvl->cols - 2;
    int max_row = 1, max_col = 1;
    int element_rows, element_cols;
    int padding_rows = PADDING_ROWS, padding_cols = PADDING_COLS;
    level_t dst_lvl;
    
    /* Find the minimum and maximum row and column where a block exists.
     * Ignore the boundary of the level, so add and subtract 1 to the initial
     * and final values. */
    for (i = 1; i < src_lvl->rows - 1; i++)
    {
        for (j = 1; j < src_lvl->cols - 1; j++)
        {
            /* Check if there is a board element. */
            if (src_lvl->board[i][j] != EMPTY)
            {
                /* Check if a new minimum row/col has been found. */
                if (i < min_row)
                {
                    min_row = i;
                }
                if (j < min_col)
                {
                    min_col = j;
                }
                /* Check if a new maximum row/col has been found. */
                if (i > max_row)
                {
                    max_row = i;
                }
                if (j > max_col)
                {
                    max_col = j;
                }
            }
        }
    }
    
    /* If either the rows or the cols are negative, it means there was an empty
     * board. Just return the input level. */
    element_rows = (max_row + 1) - min_row;
    element_cols = (max_col + 1) - min_col;
    
    if ( element_rows < 0 || element_cols < 0)
    {
        return *src_lvl;
    }
    
    /* Determine padding size. */
    
    /* Padding is whitespace between border and first element, but BOARD_MAX_R
     * includes the border, so subtract 2. */
    if ( (element_rows + 2*PADDING_ROWS) > BOARD_MAX_R - 2 )
    {
        /* Calculate the available white space, and divide by 2. Use integer
         * division to round down. */
        padding_rows = ((BOARD_MAX_R - 2) - element_rows) / 2;
    }
    if ( (element_cols + 2*PADDING_COLS) > BOARD_MAX_C - 2 )
    {
        /* Calculate the available white space, and divide by 2. Use integer
         * division to round down. */
        padding_cols = ((BOARD_MAX_C - 2) - element_cols) / 2;
    }
        
    /* Create a new board with a border, using the dimensions found. */
    dst_lvl.rows = element_rows + 2*padding_rows + 2;
    dst_lvl.cols = element_cols + 2*padding_cols + 2;   
    
    for (i = 0; i < dst_lvl.rows; i++)
    {
        for (j = 0; j < dst_lvl.cols; j++)
        {
            /* Boarder values set as holes for default. */
            if (i == 0 ||
                i == dst_lvl.rows - 1 ||
                j == 0 ||
                j == dst_lvl.cols - 1)
            {
                dst_lvl.board[i][j] = HOLE;
            }
            /* All inner board values set to empty by default. */
            else
            {
                dst_lvl.board[i][j] = EMPTY;
            }
        }
    }
        
    /* Copy board values into the centre of the new board. */
    for (i = 0; i < element_rows; i++)
    {
        for (j = 0; j < element_cols; j++)
        {
            /* Copy in board values from input board. */
            dst_lvl.board[i + padding_rows + 1][j + padding_cols + 1] =
                src_lvl->board[i + min_row][j + min_col];
        }
    }
    
    /* Get the additional inital level values for the board. */
    dst_lvl.p_row = src_lvl->p_row - min_row + padding_rows + 1;
    dst_lvl.p_col = src_lvl->p_col - min_col + padding_cols + 1;
    
    dst_lvl.moves = src_lvl->moves;
    dst_lvl.nmoves = 0;
    dst_lvl.moving_block_check = moving_block_check(&dst_lvl);
    dst_lvl.bomb = 0;
    dst_lvl.message_available = 0;
    
    return dst_lvl;
}

/*---------------------------------------------------------------------------*/
/* 
 * Checks if level has a player and a goal. Does not check if the level can
 * be beaten.
 */

int
is_player_and_goal_valid(level_t *lvl, coord_t goal)
{
    if (lvl->board[lvl->p_row][lvl->p_col] == PLAYER &&
        lvl->board[goal.row][goal.col] == GOAL)
    {
        return TRUE;
    }
    
    return FALSE;    
}
 
/*---------------------------------------------------------------------------*/
/*
 * Appends level to end of level editor pack.
 */
 
void
write_level(level_t lvl)
{
    int i, j;
    char file_name[MAX_FILE_LEN];
    FILE *fp = NULL;
    
    strcpy(file_name, CUSTOM_LEVEL_FILE);
    strcat(file_name, LEVEL_FILE);

    fp = fopen(file_name, "r");
    
    /* If the file doesn't exist already, create it with the title. */ 
    if (fp == NULL)
    {
        fp = fopen(file_name, "w");
        fprintf(fp, "%s\n", CUSTOM_LEVEL_NAME);
    }
    fclose(fp);
    
    /* Open file to append new level on the end. */
    fp = fopen(file_name, "a");
    
    if (fp == NULL)
    {
        /* Failed opening file, display error and return. */
        
        /* TODO: display error. */
        return;
    }
    
    fprintf(fp, "\n");
    fprintf(fp, "%d %d %d\n", lvl.rows, lvl.cols, lvl.moves);
    
    for (i = 0; i < lvl.rows; i++)
    {
        for (j = 0; j < lvl.cols; j++)
        {
            fprintf(fp, "%d ", lvl.board[i][j]);
        }
        fprintf(fp, "\n");
    }
    
    fclose(fp);
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Prints error screens for unbeaten level and incomplete level.
 */
 
void
editor_message_screen(int message_code)
{
    char *incomplete_level[] = {
" ",
" ",
"     ERROR: LEVEL IS INCOMPLETE",
" ",
" ",
"       Level must have player icon and a goal icon to be playable.",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
"       Press any key to return to level editor",
        NULL};

    char *unbeaten_level[] = {
" ",
" ",
"     SAVE UNSUCCESSFUL",
" ",
" ",
"       Level must be beaten before it can be saved.",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
"       Press any key to return to level editor",
        NULL};
    
    char *save_successful[] = {
" ",
" ",
"     CONGRATULATIONS!",
" ",
"       Your level has been sucessfully saved!",
" ",
" ",
"       The target number of moves has been set to the number of moves you",
"       completed the level in.",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
"       Press any key to return to level editor",
        NULL};

    if (message_code == INCOMPLETE_CODE)
    {
        print_message_screen(incomplete_level);
    }
    else if (message_code == UNBEATEN_CODE)
    {
        print_message_screen(unbeaten_level);
    }
    else if (message_code == SUCCESSFUL_CODE)
    {
        print_message_screen(save_successful);
    }
    
    /* Wait for player to enter key. */
    getch();
    
    return;
}

/*---------------------------------------------------------------------------*/
/*
 * Displays the yes or no screens for the level editor.
 */
 
char
editor_decision_screen(int message_code)
{
    char input = '\0';
    
    char *quit_editor[] = {
" ",
" ",
"     QUIT LEVEL EDITOR:",
" ",
" ",
"       Are you sure you want to quit the level editor? (y/n)",
" ",
"       Progress on current level will not be saved",
"  ",
    NULL};

    char *clear_editor[] = {
" ",
" ",
"     CLEAR LEVEL:",
" ",
" ",
"       Are you sure you want to clear the current level? (y/n)",
" ",
"       Progress on current level will not be saved",
"  ",
    NULL};
    
    while(TRUE)
    {
        /* Display the screen. */
        if (message_code == QUIT_EDITOR_CODE)
        {
            print_message_screen(quit_editor);
        }
        else if (message_code == CLEAR_EDITOR_CODE)
        {
            print_message_screen(clear_editor);
        }
        
        /* Get player input. */
        input = getch();
        
        if (input == YES)
        {
            return YES;
        }
        else if (input == NO)
        {
            return NO;
        }
    }
 
    return NO;
}

/*-----------------------------------END-------------------------------------*/
