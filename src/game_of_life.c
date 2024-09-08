#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZEX 80
#define SIZEY 25
#define ALIVE 'o'
#define DEAD '.'
#define STARTSPEED 1

char **init_one_buf_array(void *buf, int sizey, int sizex);
int input_data(char **arr, int sizey, int sizex);
void output(char **arr, int sizey, int sizex, int speed, int first);
void calc_new_state(char **map, char **buf);
char get_new_state(char **map, int j, int i);
void copy(char **dest, char **src);
void set_non_blocking_input(int on);
void init_input_state();
void read_commands(int i, int *speed);

int main() {
    int speed = STARTSPEED;
    char arr1[(SIZEY * sizeof(char *)) + (SIZEX * SIZEY * sizeof(char))];
    char arr2[(SIZEY * sizeof(char *)) + (SIZEX * SIZEY * sizeof(char))];
    char **map = init_one_buf_array(arr1, SIZEY, SIZEX);
    char **buf = init_one_buf_array(arr2, SIZEY, SIZEX);

    if (input_data(map, SIZEY, SIZEX)) {
        output(map, SIZEY, SIZEX, speed, 1);
        init_input_state();
        int i = 0;
        while (1) {
            calc_new_state(map, buf);
            output(map, SIZEY, SIZEX, speed, 0);
            read_commands(i, &speed);
            usleep(200000 / speed);
        }
    } else {
        printf("Invalid map!\n");
    }
    set_non_blocking_input(0);
    return 0;
}

void read_commands(int i, int *speed)
{
    if (i++ % 20 == 0) {
        char c;
        while ((c = getchar()) == '+' || c == '-') {
            *speed = c == '+' ? *speed + 1 : *speed - 1;
        }
        if (*speed < 1) *speed = 1;
    }
}

void init_input_state()
{
    fclose(stdin);
    freopen("/dev/tty", "r", stdin);
    set_non_blocking_input(1);
}

void set_non_blocking_input(int on)
{
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (on) fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    else fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}


void copy(char **dest, char **src) {
    for (int j = 0; j < SIZEY; j++) {
        for (int i = 0; i < SIZEX; i++) {
            dest[j][i] = src[j][i];
        }
    }
}

char get_new_state(char **map, int j, int i) {
    int y1 = j == 0 ? SIZEY - 1 : j - 1;
    int y2 = j == SIZEY - 1 ? 0 : j + 1;
    int x1 = i == 0 ? SIZEX - 1 : i - 1;
    int x2 = i == SIZEX - 1 ? 0 : i + 1;
    int alive_neighbours_count = 0;

    for (int y = y1; y != y2 + 1; y++) {
        y = y == SIZEY ? 0 : y;
        for (int x = x1; x != x2 + 1; x++) {
            x = x == SIZEX ? 0 : x;
            if (!(y == j && x == i) && map[y][x] == ALIVE) alive_neighbours_count++;
        }
    }
    return ((map[j][i] == ALIVE && (alive_neighbours_count == 2 || alive_neighbours_count == 3)) ||
                    (map[j][i] == DEAD && alive_neighbours_count == 3)
                ? ALIVE
                : DEAD);
}

void calc_new_state(char **map, char **buf) {
    for (int j = 0; j < SIZEY; j++) {
        for (int i = 0; i < SIZEX; i++) {
            buf[j][i] = get_new_state(map, j, i);
        }
    }
    copy(map, buf);
}

void output(char **arr, int sizey, int sizex, int speed, int first) {
    if (first) printf("\033[2J\033[1;1H");
    printf("\033[H");
    for (int i = 0; i < sizey; i++) {
        for (int j = 0; j < sizex; j++) {
            printf("%c", arr[i][j]);
        }
        printf("\n");
    }
    printf("SPEED %d\n", speed);
}

char **init_one_buf_array(void *buf, int sizey, int sizex) {
    char *p = buf + sizey * sizeof(char *);
    for (int i = 0; i < sizey; i++) {
        ((char **)buf)[i] = p + i * sizex;
    }
    return buf;
}

int input_data(char **arr, int sizey, int sizex) {
    int res = 1;
    for (int j = 0; j < sizey; j++) {
        for (int i = 0; i < sizex; i++) {
            arr[j][i] = getchar();
            if (arr[j][i] != ALIVE && arr[j][i] != DEAD) {
                res = 0;
                break;
            }
        }
        if (j != SIZEY - 1 && getchar() != '\n') {
            break;
        }
    }
    return res;
}
