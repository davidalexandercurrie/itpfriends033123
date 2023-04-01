#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <lo/lo.h>
#define PORT 1234

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int color;
    const char *osc_address;
    int player_inside;
} Box;

// Function prototypes
void init_ncurses();
void game_loop();
void draw();
void input();
void cleanup();
void nextValue(int *x);
void animatePlayer();
void init_colors();
void send_osc_message(int x, int y); 
void draw_room(int y, int x, int height, int width);

void check_bounds_and_send_osc(int playerX, int playerY, Box *box);

void send_osc_message_box(int value, const char *osc_address);

int playerX = 5;
int playerY = 5;
int box1 = 0; // False

// Global variables
int running = 1;
int ch;
unsigned long counter = 0;



Box boxes[] = {
    {72, 31, 20, 10, 3, "box1"},
    {1, 31, 20, 10, 4, "box2"},
    {50, 4, 20, 10, 5, "box3"},
    {60, 20, 20, 10, 6, "box4"},
    {15, 15, 20, 10, 7, "box5"},
};
const int num_boxes = sizeof(boxes) / sizeof(Box);


int main() {
    init_ncurses();
    init_colors();

    game_loop();

    cleanup();

    return 0;
}

void init_ncurses() {
    initscr(); // Initialize the library and screen
    raw();     // Disable line buffering
    noecho();  // Do not display input on the screen
    curs_set(0); // Hide the cursor
    // Remove the timeout function
    keypad(stdscr, FALSE); 
    mouseinterval(0); 
    scrollok(stdscr, FALSE);
    nodelay(stdscr, TRUE);
    if (has_colors()) {
        start_color(); // Enable color support
    }
    
}

int offsetsPlayer[] = {-1, 1, 0, 0, 1, -1, 0, 0};

void game_loop() {
    while (running) {
        draw();
        input();
        // Remove the usleep function
        counter++;
        usleep(10000);
    }
}

void draw() {
    clear(); // Clear the screen
    // animate player every 10 frames
    if (counter % 10 == 0){
        animatePlayer();
    }
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(2, 10, "bit beat is dead");
    attroff(COLOR_PAIR(2) | A_BOLD);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(playerY, playerX,     "+-------+");
    mvprintw(playerY + 1, playerX + 1, " .   . ");
    mvprintw(playerY + 1, playerX + offsetsPlayer[0], "|");
    mvprintw(playerY + 1, playerX + offsetsPlayer[1] + 8, "|");
    mvprintw(playerY + 2, playerX + 1, "  \\ /  ");
    mvprintw(playerY + 2, playerX + offsetsPlayer[2], "|");
    mvprintw(playerY + 2, playerX + offsetsPlayer[3] + 8, "|");

    mvprintw(playerY + 3, playerX + 1, "       ");
    mvprintw(playerY + 3, playerX + offsetsPlayer[4], "|");
    mvprintw(playerY + 3, playerX + offsetsPlayer[5] + 8, "|");
    mvprintw(playerY + 4, playerX + 1, " \\\\_/  ");
    mvprintw(playerY + 4, playerX + offsetsPlayer[6], "|");
    mvprintw(playerY + 4, playerX + offsetsPlayer[7] + 8, "|");
    mvprintw(playerY + 5, playerX, "+-------+");

    attroff(COLOR_PAIR(1) | A_BOLD);
    for (int i = 0; i < num_boxes; i++) {
    Box box = boxes[i];
    attron(COLOR_PAIR(box.color) | A_BOLD);
    draw_room(box.y, box.x, box.height, box.width);
    attroff(COLOR_PAIR(box.color) | A_BOLD);
}
    refresh(); // Refresh the screen to show the updates
}

void input() {
    ch = getch();

    switch (ch) {
        case 'w':
            playerY-=2;
            send_osc_message(playerX, playerY);
            break;
        case 'a':
            playerX-=2;
            send_osc_message(playerX, playerY);
            break;
        case 's':
            playerY+=1;
            send_osc_message(playerX, playerY);
            break;  
        case 'd':
            playerX+=1;
            send_osc_message(playerX, playerY);
            break;
        case 'q':
            running = 0;
            break;
        // Add game input handling here
        default:
            break;
    }

    for (int i = 0; i < num_boxes; i++) {
    check_bounds_and_send_osc(playerX, playerY, &boxes[i]);
    }

}


void cleanup() {
    endwin(); // Restore terminal settings and close the library

}

void nextValue(int *x) {
    if (*x == 1) {
        *x = -1;
    } else if (*x == -1) {
        *x = 0;
    } else if (*x == 0) {
        *x = 1;
    }
}

void animatePlayer() {
    nextValue(&offsetsPlayer[0]);
    nextValue(&offsetsPlayer[1]);
    nextValue(&offsetsPlayer[2]);
    nextValue(&offsetsPlayer[3]);
    nextValue(&offsetsPlayer[4]);
    nextValue(&offsetsPlayer[5]);
    nextValue(&offsetsPlayer[6]);
    nextValue(&offsetsPlayer[7]);
}

void init_colors() {
    if (has_colors()) {
        init_pair(1, COLOR_RED, COLOR_BLACK); // Define color pair 1 (red on black)
        init_pair(2, COLOR_GREEN, COLOR_BLACK); // Define color pair 2 (green on black)
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Define color pair 3 (yellow on black)
        init_pair(4, COLOR_BLUE, COLOR_BLACK);    // Define color pair 4 (blue on black)
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Define color pair 5 (magenta on black)
        init_pair(6, COLOR_CYAN, COLOR_BLACK);    // Define color pair 6 (cyan on black)
        init_pair(7, COLOR_WHITE, COLOR_BLACK);   // Define color pair 7 (white on black)
    }
}

void send_osc_message(int x, int y) {
    lo_address t = lo_address_new(NULL, "1234");
    lo_message msg = lo_message_new();
    lo_message_add_int32(msg, x);
    lo_message_add_int32(msg, y);

    if (lo_send_message(t, "movement", msg) == -1) {
        printf("OSC error %d: %s\n", lo_address_errno(t), lo_address_errstr(t));
    }

    lo_message_free(msg);
    lo_address_free(t);
}


void draw_room(int y, int x, int height, int width) {
    int thickness = 2;

    // Draw the top and bottom borders
    for (int i = 0; i < thickness; ++i) {
        mvhline(y + i, x + thickness, ACS_HLINE, width - thickness * 2);
        mvhline(y + height - 1 - i, x + thickness, ACS_HLINE, width - thickness * 2);
    }

    // Draw the left and right borders
    for (int i = 0; i < thickness; ++i) {
        mvvline(y + thickness, x + i, ACS_VLINE, height - thickness * 2);
        mvvline(y + thickness, x + width - 1 - i, ACS_VLINE, height - thickness * 2);
    }

    // Draw the corners
    for (int i = 0; i < thickness; ++i) {
        for (int j = 0; j < thickness; ++j) {
            mvaddch(y + i, x + j, ACS_ULCORNER);
            mvaddch(y + i, x + width - 1 - j, ACS_URCORNER);
            mvaddch(y + height - 1 - i, x + j, ACS_LLCORNER);
            mvaddch(y + height - 1 - i, x + width - 1 - j, ACS_LRCORNER);
        }
    }
}



// ...

void check_bounds_and_send_osc(int playerX, int playerY, Box *box) {
    int withinBounds = (playerX >= box->x && playerX <= (box->x + box->width)) &&
                       (playerY >= box->y && playerY <= (box->y + box->height));

    if (withinBounds && !box->player_inside) {
        box->player_inside = 1; // True
        send_osc_message_box(1, box->osc_address);
    } else if (!withinBounds && box->player_inside) {
        box->player_inside = 0; // False
        send_osc_message_box(-1, box->osc_address);
    }
}

void send_osc_message_box(int value, const char *osc_address) {
    lo_address t = lo_address_new(NULL, "1234");
    lo_message msg = lo_message_new();
    lo_message_add_int32(msg, value);

    if (lo_send_message(t, osc_address, msg) == -1) {
        printf("OSC error %d: %s\n", lo_address_errno(t), lo_address_errstr(t));
    }

    lo_message_free(msg);
    lo_address_free(t);
}