#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

#define SIZEX 80
#define SIZEY 25
#define ALIVE 'O'
#define DEAD '.'

char **init_one_buf_array(void *buf, int sizey, int sizex);
char **one_buffer_alloc(int sizey, int sizex);
int input_data(char **arr, int sizey, int sizex);
void output(char **arr, int sizey, int sizex);
void calc_new_state(char **map, char **buf);
char get_new_state(char **map, int j, int i);
void copy(char **dest, char **src);

int main()
{
    int speed = 1;
    char **map = one_buffer_alloc(SIZEY, SIZEX);
    char **buf = one_buffer_alloc(SIZEY, SIZEX);

    if (input_data(map, SIZEY, SIZEX))
    {
        output(map, SIZEY, SIZEX);
        while (1)
        {
            calc_new_state(map, buf);
            output(map, SIZEY, SIZEX);
            usleep(200000 / speed);
        }
    }
    else
    {
        printf("Invalid map!\n");
    }
    free(map);
    free(buf);
    return 0;
}

void copy(char **dest, char **src)
{
    printf("copy");
    for (int j = 0; j < SIZEY; j++)
    {
        for(int i = 0; i < SIZEX; i++)
        {
            dest[j][i] = src[j][i];
        }
    }
}


char get_new_state(char **map, int j, int i)
{
    int y1 = j == 0 ? SIZEY - 1 : j - 1;
    int y2 = j == SIZEY - 1 ? 0 : j + 1;
    int x1 = i == 0 ? SIZEX - 1 : i - 1;
    int x2 = i == SIZEX - 1 ? 0 : i + 1;
    int alive_neighbours_count = 0;

    for(int y = y1; y != y2 + 1; y++)
    {
        y = y == SIZEY ? 0 : y;
        for(int x = x1; x != x2 + 1; x++)
        {
            x = x == SIZEX ? 0 : x;
            if (!(y == j && x == i) && map[y][x] == ALIVE)
                alive_neighbours_count++;
        }
    }
    return ((map[j][i] == ALIVE && (alive_neighbours_count == 2 || alive_neighbours_count == 3))
        || (map[j][i] == DEAD && alive_neighbours_count == 3) ? ALIVE : DEAD);
}

void calc_new_state(char **map, char **buf)
{
    for (int j = 0; j < SIZEY; j++)
    {
        for(int i = 0; i < SIZEX; i++)
        {
            buf[j][i] = get_new_state(map, j, i);
        }
    }
    output(buf, SIZEY, SIZEX);
    copy(map, buf);
}


void output(char **arr, int sizey, int sizex) {
    printf("\033[2J\033[1;1H");
    for (int i = 0; i < sizey ; i++) {
        for (int j = 0; j < sizex; j++) {
            printf("%c", arr[i][j]);
        }
        if (i != sizey - 1) printf("\n");
    }
}


char **init_one_buf_array(void *buf, int sizey, int sizex) {
    char *p = buf + sizey * sizeof(char *);
    for (int i = 0; i < sizey; i++) {
        ((char **)buf)[i] = p + i * sizex;
    }
    return buf;
}

char **one_buffer_alloc(int sizey, int sizex) {
    void *res = calloc((sizey * sizeof(char *)) + (sizex * sizey * sizeof(char)), sizeof(char));
    return init_one_buf_array(res, sizey, sizex);
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
        if (j != SIZEY -1 && getchar() != '\n')
        {
            break;
        }
    }
    return res;
}