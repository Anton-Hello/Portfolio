#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

void handle_interrupt()
{}

typedef struct cell
{
    char x;
    char y;
    struct cell *tail;
} cell;

typedef struct snake
{
    char direction;
    int length;
    cell *head;
} snake;

void delay(int n){
    while(n!=0){
        for (int i = 0; i<15000; i++){
            
        }
        n--;
    }
}

bool check_apple(bool *grid, int width, int x, int y){
    if (x < 0 || y < 0 || x >= width || y >= width) {
        return false;
    }
    return grid[y * width + x];
}

void remove_tail(snake *snk){

}

void move_snake(snake *snk, bool *grid, int grid_size) {
    int new_x = snk->head->x;
    int new_y = snk->head->y;

    switch (snk->direction)
    {
        case 0: // right
            new_x += 1;
            break;
        case 1: // up
            new_y -= 1;
            break;
        case 2: // left
            new_x -= 1;
            break;
        case 3: // down
            new_y += 1;
            break;
        default:
            // invalid direction
        break;
    }

    if (!check_apple(grid, grid_size, new_x, new_y)) 
    {
        //Remove the tail if no apple if tail not removed it will "grow"
    }
    //Always change snakes head to new_x new_y
    
}

int main() {
    volatile char *VGA = (volatile char*)0x08000000;

    for (int i = 0; i < 320 * 480; i++) {
        VGA[i] = i / 320;
    }

    unsigned int y_ofs = 0;
    volatile int *VGA_CTRL = (volatile int*)0x04000100;

    int grid_size = 23;
    bool *grid = (bool *)malloc(grid_size * grid_size * sizeof(bool));
    if (grid == NULL) {
        return 1;
    }

    // Initialize grid to false
    for (int i = 0; i < grid_size * grid_size; i++) {
        grid[i] = false;
    }

    // Example: place an apple at (10, 13)
    grid[13 * grid_size + 10] = true;

    while (1) {
        *(VGA_CTRL + 1) = (unsigned int)(VGA + y_ofs * 320);
        *(VGA_CTRL + 0) = 0;
        y_ofs = (y_ofs + 1) % 240;

        for (int i = 0; i < 1000000; i++)
            asm volatile("nop");

        delay(250); // Delay 1/4 sec
    }

    free(grid);
    return 0;
}
