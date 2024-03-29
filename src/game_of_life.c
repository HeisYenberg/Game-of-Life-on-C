#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 27
#define COLS 82

int **read_file();
int is_alive(int **grid);
int control(int *game_speed);
void start_game(int **grid);
void grid_collision(int **grid, int i, int j);
void change_grid(int **grid, int *non_stable);
void print_grid(int **grid, int gen_count);

int main() {
    int **grid = read_file();
    if (grid)
        start_game(grid);
    else
        printf("Invalid file :(");
}

void start_game(int **grid) {
    int game_speed = 150000, gen_count = 0, non_stable = 1;
    initscr();
    clear();
    noecho();
    while (1) {
        nodelay(stdscr, TRUE);
        clear();
        print_grid(grid, gen_count);
        change_grid(grid, &non_stable);
        if (!is_alive(grid) || !non_stable) {
            for (int i = 0; i < ROWS; i++) free(grid[i]);
            free(grid);
            endwin();
            printf("Your colony lasted for %d years.\n", gen_count);
            if (!non_stable) printf("And now continues to live in peace.\n");
            break;
        }
        if (!control(&game_speed)) {
            for (int i = 0; i < ROWS; i++) free(grid[i]);
            free(grid);
            endwin();
            break;
        }
        usleep(game_speed);
        gen_count++;
    }
}

int **read_file() {
    printf(
        "To start the game, enter the name of one of the following starting "
        "points:\n");
    printf(
        "- \"../data-samples/centinal.txt\"\n- \"../data-samples/crabs.txt\"\n- "
        "\"../data-samples/diamonds.txt\"\n- \"../data-samples/long.txt\"\n- "
        "\"../data-samples/swan.txt\"\n");
    FILE *file;
    int **grid = 0;
    char file_name[100];
    scanf("%99s", file_name);
    file = fopen(file_name, "r");
    if (file) {
        int flag = 1;
        grid = calloc(ROWS, sizeof(int *));
        for (int i = 0; i < ROWS; i++) grid[i] = calloc(COLS, sizeof(int));
        for (int i = 1; i < ROWS - 1; i++)
            for (int j = 1; j < COLS - 1; j++) {
                char c = fgetc(file);
                if (c == '1')
                    grid[i][j] = 1;
                else if (c == '0')
                    grid[i][j] = 0;
                else
                    flag = 0;
            }
        if (fgetc(file) != EOF) flag = 0;
        fclose(file);
        if (flag == 0) {
            for (int i = 0; i < ROWS; i++) free(grid[i]);
            free(grid);
            grid = 0;
        }
    }
    return grid;
}

int is_alive(int **grid) {
    int temp = 0;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) temp += grid[i][j];
    if (temp == 0)
        return 0;
    else
        return 1;
}

int control(int *game_speed) {
    char input = getch();
    int flag = 1;
    if ((input == 's' || input == 'S') && *game_speed < 300000)
        *game_speed += 30000;
    else if ((input == 'w' || input == 'W') && *game_speed > 30000)
        *game_speed -= 30000;
    else if (input == 'q' || input == 'Q')
        flag = 0;
    return flag;
}

void print_grid(int **grid, int gen_count) {
    printw(
        "\t\t\t\t\t     #####                                           #\n"
        "\t\t\t\t\t    #     #   ##   #    # ######     ####  ######    #       "
        "# ###### ######\n"
        "Press \"W\" to speed up\t\t\t    #        #  #  ##  ## #         #    # "
        "#         #       # #      "
        "#\n"
        "the game,\t\t\t\t    #  #### #    # # ## # #####     #    # #####     # "
        "      # #####  "
        "#####\t\t\t\tYour colony lasting\n"
        "or \"S\" to slow down.\t\t\t    #     # ###### #    # #         #    # "
        "#         #       # #      "
        "#\t\t\t\t\tfor %d years\n"
        "Press \"Q\" to quit.\t\t\t    #     # #    # #    # #         #    # #  "
        "       #       # #      #\n"
        "\t\t\t\t\t     #####  #    # #    # ######     ####  #         ####### "
        "# #      ######\n",
        gen_count);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1)
                printw("#");
            else if (grid[i][j] == 1)
                printw("@");
            else
                printw(" ");
            printw(" ");
        }
        printw("\n");
    }
}

void grid_collision(int **grid, int i, int j) {
    if (grid[1][j]) grid[ROWS - 3][j] = 1;
    if (grid[ROWS - 2][j]) grid[2][j] = 1;
    if (grid[i][1]) grid[i][COLS - 2] = 1;
    if (grid[i][COLS - 3]) grid[i][2] = 1;
}

void change_grid(int **grid, int *non_stable) {
    int **new_grid = calloc(ROWS, sizeof(int *)), count = 0;
    for (int i = 0; i < ROWS; i++) new_grid[i] = calloc(COLS, sizeof(int));
    for (int i = 1; i < ROWS - 1; i++)
        for (int j = 1; j < COLS - 1; j++) {
            int sum = grid[i + 1][j] + grid[i - 1][j] + grid[i][j + 1] + grid[i][j - 1] + grid[i - 1][j - 1] +
                      grid[i + 1][j + 1] + grid[i + 1][j - 1] + grid[i - 1][j + 1];
            grid_collision(new_grid, i, j);
            if (sum == 2)
                new_grid[i][j] = grid[i][j];
            else if (sum == 3)
                new_grid[i][j] = 1;
            else
                new_grid[i][j] = 0;
        }
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == new_grid[i][j]) count++;
            grid[i][j] = new_grid[i][j];
        }
    if (count == ROWS * COLS) *non_stable = 0;
    for (int i = 0; i < ROWS; i++) free(new_grid[i]);
    free(new_grid);
}