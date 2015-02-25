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

/* screen constants */
#define SCREEN_MAX_R         21
#define SCREEN_MAX_C         80
#define L_PER_COL         6
#define PACK_COLS        2    /* for display in pack select */
#define LEVEL_COLS        9    /* for display in level select */

#define TIME_BETWEEN_FRAMES     150     /* milliseconds */

/* board values */
#define BOARD_MAX_R         20
#define BOARD_MAX_C         50
#define EMPTY             0
#define WALL             1
#define GOAL             2
#define PLAYER             3
#define MOVING_BLOCK         8
#define HOLE             9
#define PLAYER_FALL_1         91
#define PLAYER_FALL_2         92

/* player inputs */
#define LEFT             'a'
#define UP             'w'
#define RIGHT             'd'
#define DOWN             's'
#define QUIT             'q'
#define RESTART         'r'

/* number of levels */
#define MAX_LEVELPACKS         10
#define MAX_LEVELS        50

/* file constants */
#define MAX_FILE_LEN         13
#define FILE_NAME        "slider"
#define SAVE_FILE        ".sav"
#define LEVEL_FILE         ".lvl"

/* other */
#define MAX_NAME_LEN        15
#define BEATEN            1
#define ACED            2
#define TRUE             1
#define FALSE             0

/* typedefines */
typedef int board_t[BOARD_MAX_R][BOARD_MAX_C];

typedef struct {
    board_t    board;            /* level layout */
    int     rows;            /* number of rows in level */
    int     cols;            /* number of columns in level */
    int    p_row;            /* player starting row */
    int     p_col;            /* player starting column */
    int    moves;            /* min number of moves to beat level */
    int    nmoves;            /* current moves */
    int    moving_block_check;    /* true if the player is stationary */
} level_t;

typedef struct {
    int     data[MAX_LEVELS];
    char    sav_file[MAX_FILE_LEN];
    int    nlevels;
} save_t;

typedef struct {
    level_t level[MAX_LEVELS];    /* array holding levels */
    int     nlevels;        /* number of levels */
    char    name[MAX_NAME_LEN];    /* name of levelpack */
    save_t    save;            /* save state */
} levelpack_t;

typedef struct {
    levelpack_t     pack[MAX_LEVELPACKS];
    int        npacks;
} all_packs_t;

/* gameplay functions */
void menu(all_packs_t *all_packs);
void pack_select(all_packs_t *all_packs);
void level_select(levelpack_t *levelpack);
int play(level_t *level, save_t *save, int level_num);
int move(level_t *lvl, char move);
void hole(level_t *stored_lvl, level_t *lvl);
void moving_block(level_t *lvl, char direction);

/* display functions */
void title_screen(void);
void how_to_play(void);
void disp_board(level_t *level);
void victory_screen(void);
void clear_screen(void);
void print_message_screen(char *msg[]);
void print_level_select(char *name, save_t save);
void print_pack_select(all_packs_t *all_packs);

/* saving functions */
void read_save(save_t *save);
void write_save(save_t save);
void clear_save(all_packs_t *all_packs);

/* level pack functions */
void get_levels(all_packs_t *all_packs);
int get_pack(levelpack_t *levelpack, FILE *fp);
int set_board(levelpack_t *levelpack);
void level_to_board(board_t board, board_t level);
int all_beaten(save_t save);

/* general functions */
void int_swap(int *p1, int *p2);
void clear(void);
void level_load_error(void);
void set_zero(int array[], int n);
void itoa_2digit(int i, char *s);

/*---------------------------------------------------------------------------*/

int
main(int argc, char *argv[]) {
    
    /* set levelpack */
    all_packs_t all_packs;
    
    get_levels(&all_packs);
    
    title_screen();
    
    menu(&all_packs);
    
    return 0;
}

/*---------------------------------------------------------------------------*/
/* menu screen */

#define HOW_TO_PLAY 'h'
#define LEVEL_SELECT 's'
#define CLEAR_SAVE 'c'

void
menu(all_packs_t *all_packs) {
    
    int junk;
    char player_choice = '\0';
    
    /* pointer to string array for menu screen */
    char *menu[] = {
" ",
" ",
"     MENU:",
" ",
" ",
"       h:  How to play",
" ",
"       s:  Level select",
" ",
"       c:  Clear completion data",
" ",
"       q:  Quit game",
    NULL};
    
    while(TRUE) {
        
        print_message_screen(menu);
        
        /* get player input */
        junk = scanf("%c", &player_choice);
        junk++;
        
        if (player_choice == HOW_TO_PLAY) {
            how_to_play();
        }
        else if (player_choice == LEVEL_SELECT) {
            pack_select(all_packs);
        }
        else if (player_choice == CLEAR_SAVE) {
            clear_save(all_packs);
        }
        else if (player_choice == QUIT) {
            return;
        }
        
    }
}

/*---------------------------------------------------------------------------*/
/* levelpack select screen */

void
pack_select(all_packs_t *all_packs) {

    char player_quit;
    int junk, pack_sel;
    
    while(TRUE) {
                
        print_pack_select(all_packs);
        
        /* set values to zero after every game */
        pack_sel = 0;
        player_quit = '\0';
        
        /* get player input */
        if(scanf("%d", &pack_sel)!=1) {
            junk = scanf("%c", &player_quit);
            junk++;
            if (player_quit == QUIT) {
                return;
            }
        }
        
        /* play game if correct input given */
        if (pack_sel>0 && pack_sel<=(all_packs->npacks)) {
            level_select(&all_packs->pack[pack_sel-1]);
        }
    }    
}


/*---------------------------------------------------------------------------*/
/* level select screen */

void
level_select(levelpack_t *levelpack) {
    
    char player_quit;
    int junk, level_sel;
    
    while(TRUE) {
                
        print_level_select(levelpack->name, levelpack->save);
        
        /* set values to zero after every game */
        level_sel = 0;
        player_quit = '\0';
        
        /* get player input */
        if(scanf("%d", &level_sel)!=1) {
            junk = scanf("%c", &player_quit);
            junk++;
            if (player_quit == QUIT) {
                return;
            }
        }
        
        /* play game if correct input given */
        if (level_sel>0 && level_sel<=(levelpack->nlevels)) {
            play(&levelpack->level[level_sel-1], 
                &levelpack->save, level_sel);
        }
    }
}

/*---------------------------------------------------------------------------*/
/* opens all levelpacks */

void 
get_levels(all_packs_t *all_packs) {
    
    int i, pack = 0;
    char     lvl_name[MAX_FILE_LEN],
        sav_name[MAX_FILE_LEN],
        pack_num[] = "00";
    FILE *fp;
    
    for (i=0; i<MAX_LEVELPACKS; i++) {
        
        /* make .lvl and .sav filenames */
        strcpy(lvl_name, FILE_NAME);
        itoa_2digit(i, pack_num);
        strcat(lvl_name, pack_num);
        
        strcpy(sav_name, lvl_name);
        
        strcat(lvl_name, LEVEL_FILE);
        strcat(sav_name, SAVE_FILE);
                
        /* open file */
        if ((fp = fopen(lvl_name, "r")) != NULL) {
            
            if (!get_pack(&all_packs->pack[pack], fp)) {
                level_load_error();
                exit(EXIT_FAILURE);
            }    
            
            /* close file */
            fclose(fp);
            
            /* copy save file name to levelpack */
            strcpy(all_packs->pack[pack].save.sav_file, sav_name);
            
            /* copy nlevels */
            all_packs->pack[pack].save.nlevels = 
                all_packs->pack[pack].nlevels;
            
            /* set save file */
            set_zero(all_packs->pack[pack].save.data,
                all_packs->pack[pack].nlevels);
            
            read_save(&all_packs->pack[pack].save);
            
            pack++;
        }
    }
    
    /* set total number of levelpacks */
    all_packs->npacks = pack;

    /* exit if no levels have been loaded */
    if (!pack) {
        level_load_error();
        exit(EXIT_FAILURE);
    }    
}

/*---------------------------------------------------------------------------*/
/* opens a levelpack and copies the levels */

int
get_pack(levelpack_t *levelpack, FILE *fp) {
    
    int i, j, level = 0;
    int     rows = 0, 
        cols = 0, 
        p_row = 0, 
        p_col = 0, 
        moves = 0,
        moving_block = 0;
            
    /* get levelpack name */    
    fscanf(fp, "%s", levelpack->name);    
        
    /* loop while data is available */
    while (fscanf(fp, "%d%d%d%d%d%d", &rows, &cols, &p_row, &p_col,
        &moves, &moving_block) == 6) {
        
        /* board too big */
        if (rows>BOARD_MAX_R || cols>BOARD_MAX_C) {
            return FALSE;
        }
        
        /* too many levels */
        if (level >= MAX_LEVELS) {
            break;
        }
        
        /* copy values into level */
        levelpack->level[level].rows = rows;
        levelpack->level[level].cols = cols;
        levelpack->level[level].p_row = p_row;
        levelpack->level[level].p_col = p_col;
        levelpack->level[level].moves = moves;
        levelpack->level[level].nmoves = 0;
        levelpack->level[level].moving_block_check = moving_block;
        
        for (i=0; i<rows; i++) {
            for (j=0; j<cols; j++) {
                fscanf(fp, "%d", 
                    &levelpack->level[level].board[i][j]);
            }
        }
        
        /* increment level */
        level++;
    }
        
    /* set number of levels */
    levelpack->nlevels = level;
    
    return TRUE;
}
    
/*---------------------------------------------------------------------------*/
/* plays the game. Gets player input, and calls move() for movement,
set_board() for restart, or returns if quitting. */

int
play(level_t *level, save_t *save, int level_num) {
    
    char direction = '\0';
    int val, check;
    level_t currentlvl = *level;     /* current playing board */
        
    disp_board(&currentlvl);
    clear();
    
    /* get input for player */
    while ((direction = getch())) {
    
        check = TRUE;
        
        /* another move */
        currentlvl.nmoves++;
        
        /* check if player has quit */
        if (direction == QUIT) {
            return 0;
        }
    
        /* check if player restarted */
        if (direction == RESTART) {
            currentlvl = *level;
        }
            
        /* move until wall (or goal) is reached */
        while (TRUE) {
            val = move(&currentlvl, direction);
            
            /* check for valid move */
            if (val == 0) {
                currentlvl.moving_block_check = TRUE;
                break;
            }
            
            /* reset moving block check if there has been a
            successful move */
            currentlvl.moving_block_check = FALSE;
            
            /* check for goal */
            if (val == GOAL) {
                disp_board(&currentlvl);
                Sleep(1000);
                
                /* edit save file */
                if (currentlvl.nmoves <= level->moves) {
                    save->data[level_num-1] = ACED;
                }
                else if (save->data[level_num-1] != ACED) {
                    save->data[level_num-1] = BEATEN;
                }
                write_save(*save);
                
                victory_screen();
                return GOAL;
            }
            /* check for hole */
            if (val == HOLE) {
                hole(level, &currentlvl);
                check = FALSE;
                break;
            }
            
            /* check for moving block */
            if (val == MOVING_BLOCK) {
                moving_block(&currentlvl, direction);
                check = FALSE;
                currentlvl.moving_block_check = TRUE;
                break;
            }
                        
            /* display moves */
            disp_board(&currentlvl);
            Sleep(TIME_BETWEEN_FRAMES);
            
            /* do not need to display screen again */
            check = FALSE;
        }
        /* clear board and display again if no valid move */
        if (check) {
            disp_board(&currentlvl);
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*/
/* takes input direction and checks if player can move. swaps positions if
move is possible - FALSE return means no move */

int
move(level_t *lvl, char move) {
    
    /* move up */
    if (move == UP) {
        /* check for space */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == EMPTY) {
            int_swap(&lvl->board[lvl->p_row-1][lvl->p_col],
                &lvl->board[lvl->p_row][lvl->p_col]);
            lvl->p_row -= 1;
            return TRUE;
        }
        /* check for wall */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == WALL) {
            return FALSE;
        }
        /* check for goal */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == GOAL) {
            lvl->board[lvl->p_row-1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            return GOAL;
        }
        /* check for end of board */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == HOLE) {
            lvl->board[lvl->p_row-1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            lvl->p_row -= 1;
            return HOLE;
        }
        /* check for moving block */
        if (lvl->board[lvl->p_row-1][lvl->p_col] == MOVING_BLOCK) {
            if (lvl->p_row-2 >= 0 && 
                lvl->board[lvl->p_row-2][lvl->p_col] == EMPTY
                && lvl->moving_block_check == FALSE) {
                
                int_swap(&lvl->board[lvl->p_row-2][lvl->p_col],
                &lvl->board[lvl->p_row-1][lvl->p_col]);
                return MOVING_BLOCK;
            }
            else {
                return FALSE;
            }
        }
    }
    /* move down */
    if (move == DOWN) {
        /* check for space */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == EMPTY) {
            int_swap(&lvl->board[lvl->p_row+1][lvl->p_col],
                &lvl->board[lvl->p_row][lvl->p_col]);
            lvl->p_row += 1;
            return TRUE;
        }
        /* check for wall */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == WALL) {
            return FALSE;
        }
        /* check for goal */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == GOAL) {
            lvl->board[lvl->p_row+1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            return GOAL;
        }
        /* check for end of board */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == HOLE) {
            lvl->board[lvl->p_row+1][lvl->p_col] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            lvl->p_row += 1;
            return HOLE;
        }
        /* check for moving block */
        if (lvl->board[lvl->p_row+1][lvl->p_col] == MOVING_BLOCK) {
            if (lvl->p_row+2 < lvl->rows && 
                lvl->board[lvl->p_row+2][lvl->p_col] == EMPTY
                && lvl->moving_block_check == FALSE) {
                
                int_swap(&lvl->board[lvl->p_row+2][lvl->p_col],
                &lvl->board[lvl->p_row+1][lvl->p_col]);
                return MOVING_BLOCK;
            }
            else {
                return FALSE;
            }
        }
    }
    /* move left */
    if (move == LEFT) {
        /* check for space */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == EMPTY) {
            int_swap(&lvl->board[lvl->p_row][lvl->p_col-1],
                &lvl->board[lvl->p_row][lvl->p_col]);
            lvl->p_col -= 1;
            return TRUE;
        }
        /* check for wall */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == WALL) {
            return FALSE;
        }
        /* check for goal */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == GOAL) {
            lvl->board[lvl->p_row][lvl->p_col-1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            return GOAL;
        }
        /* check for end of board */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == HOLE) {
            lvl->board[lvl->p_row][lvl->p_col-1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            lvl->p_col -= 1;
            return HOLE;
        }
        /* check for moving block */
        if (lvl->board[lvl->p_row][lvl->p_col-1] == MOVING_BLOCK) {
            if (lvl->p_col-2 >= 0 && 
                lvl->board[lvl->p_row][lvl->p_col-2] == EMPTY
                && lvl->moving_block_check == FALSE) {
                
                int_swap(&lvl->board[lvl->p_row][lvl->p_col-2],
                &lvl->board[lvl->p_row][lvl->p_col-1]);
                return MOVING_BLOCK;
            }
            else {
                return FALSE;
            }
        }
    }
    /* move right */
    if (move == RIGHT) {
        /* check for space */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == EMPTY) {
            int_swap(&lvl->board[lvl->p_row][lvl->p_col+1],
                &lvl->board[lvl->p_row][lvl->p_col]);
            lvl->p_col += 1;
            return TRUE;
        }
        /* check for wall */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == WALL) {
            return FALSE;
        }
        /* check for goal */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == GOAL) {
            lvl->board[lvl->p_row][lvl->p_col+1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            return GOAL;
        }
        /* check for end of board */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == HOLE) {
            lvl->board[lvl->p_row][lvl->p_col+1] = PLAYER;
            lvl->board[lvl->p_row][lvl->p_col] = EMPTY;
            lvl->p_col += 1;
            return HOLE;
        }
        /* check for moving block */
        if (lvl->board[lvl->p_row][lvl->p_col+1] == MOVING_BLOCK) {
            if (lvl->p_col+2 < lvl->cols && 
                lvl->board[lvl->p_row][lvl->p_col+2] == EMPTY
                && lvl->moving_block_check == FALSE) {
                
                int_swap(&lvl->board[lvl->p_row][lvl->p_col+2],
                &lvl->board[lvl->p_row][lvl->p_col+1]);
                return MOVING_BLOCK;
            }
            else {
                return FALSE;
            }
        }
    }
    /* if no move valid, return FALSE */
    return FALSE;
}

/*---------------------------------------------------------------------------*/
/* special animation for player falling off the board, then resets the level */

#define FALL_TIME 500 /* milliseconds */

void
hole(level_t *stored_lvl, level_t *lvl) {
    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    /* change player icon */
    lvl->board[lvl->p_row][lvl->p_col] = PLAYER_FALL_1;
    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    /* change player icon */
    lvl->board[lvl->p_row][lvl->p_col] = PLAYER_FALL_2;
    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    /* remove player icon */
    lvl->board[lvl->p_row][lvl->p_col] = HOLE;
    
    disp_board(lvl);
    Sleep(FALL_TIME);
    
    *lvl = *stored_lvl;
    disp_board(lvl);
    Sleep(TIME_BETWEEN_FRAMES);
}

/*---------------------------------------------------------------------------*/
/* special board movements for moving block */


void
moving_block(level_t *lvl, char direction) {

    disp_board(lvl);
    Sleep(TIME_BETWEEN_FRAMES);
    
    /* move player again */
    move(lvl, direction);
    disp_board(lvl);
    Sleep(TIME_BETWEEN_FRAMES);
}

/*---------------------------------------------------------------------------*/
/* title screen */

void
title_screen(void) {
    
    int i;
    
    /* ASCII art from patorjk.com, font name: AMC Slider */
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
    
    for(i=0;i<3;i++) {
        clear_screen();
    }
    
    print_message_screen(title);

    /* wait for enter */
    getch();
}

/*---------------------------------------------------------------------------*/
/* shows controls for the game */

void
how_to_play(void) {
    
    /* pointer to string array for how to play screen */
    char *how_to_play[] = {
" ",
" ",
"     HOW TO PLAY:",
" ",
"       You are Slider, Get X!",
" ",
"     CONTROLS:",
" ",
"           w",
"       a    s   d   :   MOVE",
" ",
"       q:   QUIT     r:   RESTART LEVEL",
" ",
" ",
" ",
" ",
" ",
"       Press any key to return to menu",
    NULL};
    
    print_message_screen(how_to_play);
    
    getch();
}

/*---------------------------------------------------------------------------*/
/* prints the current board */

void 
disp_board(level_t *level) {
    
    /* clear screen and initialise variables */
    int i, j;
    clear_screen();
    
    for(i=0; i<level->rows; i++) {
        
        /* centre board columns */
        for(j=0; j<((SCREEN_MAX_C - level->cols)/4); j++) {
            printf(" ");
        }
        /* print board */
        for(j=0; j< level->cols; j++) {
            /* empty */
            if(level->board[i][j] == EMPTY) {
                printf(" ");
            } /* wall */ 
            else if(level->board[i][j] == WALL) {
                putchar(219);
            } /* goal */
            else if(level->board[i][j] == GOAL) {
                printf("X");
            } /* player */
            else if(level->board[i][j] == PLAYER) {
                printf("O");
            } /* moving block */
            else if(level->board[i][j] == MOVING_BLOCK) {
                printf("#");
            } /* end of board */
            else if(level->board[i][j] == HOLE) {
                putchar(176);
            } /* player fall 1 */
            else if(level->board[i][j] == PLAYER_FALL_1) {
                printf("o");
            } /* player fall 2 */
            else if(level->board[i][j] == PLAYER_FALL_2) {
                putchar(250);
            }
        }
        
        if (i == 0) {
            printf("   %d", level->nmoves); 
        }
        if (i == level->rows-5 && level->rows > 6) {
            printf("   MOVE    = wasd");
        }
        if (i == level->rows-3) {
            printf("   RESTART = r");
        }
        if (i == level->rows-1) {
            printf("   QUIT    = q");
        }
        printf("\n");
    }
    
    /* centre board rows */
    for(i=0; i<((SCREEN_MAX_R - level->rows)/2); i++) {
            printf("\n");
    }
}

/*---------------------------------------------------------------------------*/
/* prints victory screen - treasure chest art taken from 
ascii.co.uk/art/treasure, Simon was made myself */

void 
victory_screen(void) {
    
    /* ASCII art from patorjk.com, font name: Slant */
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
    
    /* wait for enter */
    getch();
}
    
/*---------------------------------------------------------------------------*/
/* clears the screen by printing new lines */

void
clear_screen(void) {
    int i;
    for(i=1;i<=SCREEN_MAX_R;i++) {
        printf("\n");
    }
}

/*---------------------------------------------------------------------------*/
/* prints an array of strings, and then prints remaining new lines. Used with
permission from Alistair Moffatt */

void
print_message_screen(char *msg[]) {
    int i, new_lines = 0;
    
    clear_screen();
    
    while(*msg) {
        printf("%s\n", *msg);
        msg++;
        new_lines++;
    }
    
    for(i=1; i<=(SCREEN_MAX_R - new_lines); i++) {
        printf("\n");
    }
}

/*---------------------------------------------------------------------------*/
/* prints pack select screen */

void 
print_pack_select(all_packs_t *all_packs) {
    
    int i, j, k, beaten = 0;
    
    clear_screen();
    printf("\n\n");
    printf("     LEVEL SELECT:\n\n\n");
    
    for (i=1; i<=L_PER_COL; i++) {        
        printf("    ");
        
        for (j=0; j<PACK_COLS; j++) {
            
            
            if (all_packs->npacks >= i+j*L_PER_COL) {
                
                beaten = all_beaten(all_packs->
                    pack[i+j*L_PER_COL-1].save);
                
                if (beaten == ACED) {
                    printf(" *");
                } else if (beaten == BEATEN) {
                    printf(" ");
                    putchar(248);    /* degrees symbol */
                } else {
                    printf("  ");
                }
                
                printf("%2d:  ", i+j*L_PER_COL);
                
                printf("%s", all_packs->pack[i+j*L_PER_COL-1].
                    name);
                k = strlen(all_packs->pack[i+j*L_PER_COL-1].
                    name);
                
                while (k < MAX_NAME_LEN) {
                    printf(" ");
                    k++;
                }                
            }
        }
        printf("\n\n");
    }
    printf("\n       q:  Back to Menu\n\n\n");
}

/*---------------------------------------------------------------------------*/
/* prints level select screen depending on the number of available levels */

void
print_level_select(char *name, save_t save) {

    int i, j;    

    clear_screen();
    printf("\n\n");
    printf("     %s:\n\n\n", name);
    
    for (i=1; i<=L_PER_COL; i++) {
        for (j=0; j<LEVEL_COLS; j++) {
            
            if (save.nlevels >= i+j*L_PER_COL) {
                if (save.data[i+(j*L_PER_COL)-1] == ACED) {
                    printf("     *");
                } 
                else if (save.data[i+(j*L_PER_COL)-1] 
                    == BEATEN) {
                    printf("     ");
                    putchar(248);    /* degrees symbol */
                }
                else {
                    printf("      ");
                }
                printf("%-2d", i+j*L_PER_COL);
            }
        }
        printf("\n\n");
    }
    printf("\n       q:  Back to Menu\n\n\n");
}

/*---------------------------------------------------------------------------*/
/* returns BEATEN if all levels in levelpack have been beaten, returns ACED
if all levels have been aced */

int all_beaten(save_t save) {
    
    int i;
    
    for (i=0; i<save.nlevels; i++){
        if (save.data[i] == 0) {
            return FALSE;
        }
    }
    
    for (i=0; i<save.nlevels; i++){
        if (save.data[i] == BEATEN) {
            return BEATEN;
        }
    }
    
    return ACED;
}

/*---------------------------------------------------------------------------*/
/* reads the save file with crappy encryption*/

void
read_save(save_t *save) {
    
    int i = 0, d =0;
    FILE *fp;
    
    fp = fopen(save->sav_file, "r");
    
    while(fscanf(fp, "%d", &d) != EOF ) {
        if (i<save->nlevels) {
            
            if (d == ACED) {
                save->data[i] = ACED;
                i++;
            }
            if (d == BEATEN) {
                save->data[i] = BEATEN;
                i++;
            }
            
            if (d == FALSE) {
                save->data[i] = FALSE;
                i++;
            }
        }
        
    }
    fclose(fp);
}

/*---------------------------------------------------------------------------*/
/* replaces the save file with what is currently on the game. Includes crappy
encryption */

#define MAX_RUBBISH 100

void
write_save(save_t save) {
    int i;
    FILE *fp;
        
    fp = fopen(save.sav_file, "w");
    
    for (i=0; i<save.nlevels; i++) {
        
        if (save.data[i] == ACED) {
            fprintf(fp, "%d", ACED);
        }
        else if (save.data[i] == BEATEN) {
            fprintf(fp, "%d", BEATEN);
        }
        else {
            fprintf(fp, "%d", FALSE);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

/*---------------------------------------------------------------------------*/
/* prints prompt if player actually wants to erase data, removes save data
if yes */

#define YES 'y'
#define NO 'n'

void 
clear_save(all_packs_t *all_packs) {
    
    int junk, i;
    char player_choice = '\0';
    
    /* pointer to string array for menu screen */
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
    
    while(TRUE) {
        
        print_message_screen(clear_save);
        
        /* get player input */
        junk = scanf("%c", &player_choice);
        junk++;
        
        if (player_choice == YES) {
            
            /* clear all packs */
            for (i=0; i<all_packs->npacks; i++) {
                
                set_zero(all_packs->pack[i].save.data,
                    all_packs->pack[i].nlevels);
                write_save(all_packs->pack[i].save);
            }
            
            print_message_screen(clear_success);
            Sleep(2000);  /* 2 seconds */    
            return;
        }
        else if (player_choice == NO) {
            return;
        }
    }
}

/*---------------------------------------------------------------------------*/
/* swaps two integers */

void
int_swap(int *p1, int *p2) {
    int temp;
    temp = *p1;
    *p1 = *p2;
    *p2 = temp;
}

/*---------------------------------------------------------------------------*/
/* copies level chosen to board[][] */

void 
level_to_board(board_t board, board_t level) {
    int i, j;
    
    /* active board becomes stored level */
    for(i=0; i<BOARD_MAX_R; i++) {
        for(j=0; j<BOARD_MAX_C; j++) {
            board[i][j] = level[i][j];
        }
    }
}

/*---------------------------------------------------------------------------*/
/* prints level load error */

void
level_load_error(void) {
    
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
}

/*---------------------------------------------------------------------------*/
/* clears input buffer */

void
clear (void){ 
    while (getchar() != '\n');
}

/*---------------------------------------------------------------------------*/
/* sets interger array to be zero */

void 
set_zero(int array[], int n) {
    int i;
    for (i=0; i<n; i++) {
        array[i] = 0;
    }
}

/*---------------------------------------------------------------------------*/
/* converts a positive two digit number into a string */

void
itoa_2digit(int n, char *s) {

    int i = 0;
    char tens = '\0', ones = '\0';
    
    ones = n%10 + '0';    /* add interger value to zero character */
    n /= 10;
    tens = n%10 + '0';
    
    if (tens != '0') {
        s[i++] = tens;
    }
    s[i++] = ones;
    
    /* close string */
    s[i] = '\0';
}

/*-----------------------------------END-------------------------------------*/
