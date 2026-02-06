#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


#define VGA_BASE ((volatile uint8_t*) 0x8000000)
#define WIDTH 320
#define HEIGHT 240
#define TILE 10
#define SNAKE_COLOR 0b00011100
#define APPLE_COLOR 0b11100000
#define BACKGROUND_COLOR 0x0

#define A 1664525
#define C 1013904223
#define M 4294967296

int _rand(int seed) {
    seed = (A * (seed) + C) % M;
    return seed;
}
     
void handle_interrupt()
{}

typedef struct cell
{
    int x;
    int y;
} cell;

//Alexander Tunik
void handle_pixel(int x, int y, uint8_t color) {
    VGA_BASE[y * WIDTH + x] = color;
}

//Alexander Tunik och Anton Carlsson
void set_tile(int width, int height, uint8_t color) {
    for (int y = height * TILE; y < height * TILE+10; y++) {
        for (int x = width * TILE; x < width * TILE+10; x++) {
            handle_pixel(x, y, color);
        }
    }
}

//Alexander Tunik
static inline void print_char(char c)
{
    register int a0 asm("a0") = c;
    register int a7 asm("a7") = 11;  // syscall 11 = print char
    asm volatile("ecall" : : "r"(a0), "r"(a7));
}

//Alexander Tunik
void print_str(const char *s)
{
    while (*s)
    {
        print_char(*s++);
    }
}

//Alexander Tunik
void print_int(int value)
{
    char buffer[12];
    int i = 10;
    buffer[11] = '\0';

    if (value == 0) {
        print_char('0');
        return;
    }

    int neg = value < 0;
    if (neg) value = -value;

    while (value > 0 && i >= 0)
    {
        buffer[i--] = '0' + (value % 10);
        value /= 10;
    }

    if (neg) buffer[i--] = '-';

    print_str(&buffer[i + 1]);
}
//Alexander Tunik
int get_btn(){
    volatile unsigned int* btn_addr = (volatile unsigned int*) 0x040000D0;
    return !(*btn_addr & 0x1);  // Bit 0 is active-low
}

int get_switch(){
    volatile unsigned int* sw_addr = (volatile unsigned int*) 0x04000010;
    return (*sw_addr & 0x1);  // Bit 0 is active-low
}

void add_cell(int x, int y, cell* snk, int tail, int head){ //Anton Carlsson
    snk[head+1].x = x;
    snk[head+1].y = y;
}

void draw_snake(cell *snk, int tail, int head){
    
    for(int i = tail; i<= head; i++){
        set_tile(snk[i].x, snk[i].y, (uint8_t)SNAKE_COLOR);
    }
}

//Anton Carlsson
void move_snake(cell* snk, int tail, int head, int direction){
    head = (head+1)%768;
    switch(direction){
        case 0:
        snk[head].x = snk[head-1].x+1;
        snk[head].y = snk[head-1].y;
        break;

        case 1:
        snk[head].x = snk[head-1].x;
        snk[head].y = snk[head-1].y+1;
        break;

        case 2:
        snk[head].x = snk[head-1].x-1;
        snk[head].y = snk[head-1].y;
        break;

        default:
        snk[head].x = snk[head-1].x;
        snk[head].y = snk[head-1].y-1;
        break;
    }
}

//Alexander TUnik
void delay(volatile int count) {
    count = count *1500000;
    while (count-- > 0) {
        
    }
}
//Anton Carlsson
int remove_tail(cell* snk, int tail){
    set_tile(snk[tail].x, snk[tail].y, (uint8_t)BACKGROUND_COLOR);
    return tail +=1;
}

bool got_apple(cell *snk, int* apple, int head){
    if (snk[head].x == apple[0] && snk[head].y == apple[1])
        return true;
    return false;
}

void draw_apple(int* apple){
    set_tile(apple[0], apple[1], (uint8_t)APPLE_COLOR);  
    return;
}

bool snake_dead(cell *snk, int tail, int head){
    for(int i = tail; i < head; i++){
        if((snk[i].x == snk[head].x) && (snk[i].y == snk[head].y)){
            return true;
        }
    }
    if((snk[head].x>=WIDTH/TILE) || (snk[head].x<0) || (snk[head].y>=HEIGHT/TILE)|| (snk[head].y<0)){{
        return true;
    }
    }
    return false;
}
//Alexander Tunik
const uint8_t segment_encoding[10] =
{
  0b1000000,
  0b1111001,
  0b0100100,
  0b0110000,
  0b0011001,
  0b0010010,
  0b0000010,
  0b1111000,
  0b0000000,
  0b0010000
};

//Alexander Tunik
void set_displays(int display_number, int value)
{
  volatile uint8_t* display_address = (volatile uint8_t*)(0x04000050 + (display_number * 0x10));
  *display_address = segment_encoding[value];
}

int main() {
    int direction = 0;
    cell cells[768];
    int head = 0;
    int tail = 0;
    cells[0].x =(WIDTH/TILE)/2;
    cells[0].y =(HEIGHT/TILE)/2;
    int points = 0;

    head = (head+1)%768;
    cells[head].x = cells[head-1].x+1;
    cells[head].y = cells[head-1].y;
    head = (head+1)%768;
    cells[head].x = cells[head-1].x+1;
    cells[head].y = cells[head-1].y;

    int seed = 0;

    int apple[2] = {((WIDTH/TILE)/2)+5, (HEIGHT/TILE)/2};

    for(int x = 0; x<WIDTH/TILE; x++){
       for(int y = 0; y<WIDTH/TILE; y++){
           set_tile(x, y, (uint8_t)BACKGROUND_COLOR);
      } 
    }


    draw_snake(cells, tail, head);

    set_tile((WIDTH/TILE)/2 + 5, (HEIGHT/TILE)/2, (uint8_t)APPLE_COLOR);
    
    set_displays(3, 0);
    set_displays(4, 0);
    set_displays(5, 0);
    while (1) {

        if (!get_btn())
        {
            if (get_switch())
                direction = (direction + 1) % 4;
            else
                direction = (direction + 3) % 4;
        }

        move_snake(cells, tail, head, direction);
        head = (head+1)%768;
        
        //direction = (direction+1)%4;
        if(got_apple(cells, apple, head)){
            points++;
            apple[1] = _rand(seed)%24;
            apple[0] = _rand(seed)%32;
        }else{
           tail = remove_tail(cells, tail)%768;
        }
        
        draw_snake(cells, tail, head);
        
        draw_apple(apple);
        if(snake_dead(cells, tail, head)){
            for(int x = 0; x<WIDTH/TILE; x++){
                for(int y = 0; y<WIDTH/TILE; y++){
                    set_tile(x, y, (uint8_t)APPLE_COLOR);
                } 
            }
            print_str("You got ");
            print_int(points);
            print_str(" apples before you died");
            return points;
        }

        set_displays(2, points/100);
        set_displays(1, (points/10)-((points/100)*10));
        set_displays(0, points-((points/10)*10)-((points/100)*100));
        seed++;
        delay(1);
    }
    return 0;
}