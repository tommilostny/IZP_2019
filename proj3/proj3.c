/*	proj3.c
 *
 *	Projekt 3 - Prace s datovymi strukturami
 *	Pruchod bludistem
 *
 *	Tomas Milostny (xmilos02)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <ctype.h> //isspace

#define ERROR_END "\nRun program with the --help argument for more info.\n"

typedef struct {
	int rows;
	int cols;
	unsigned char *cells;
} Map;

typedef enum {
	TOP, BOTTOM
} Orientation; //TOP opens UP direction, BOTTOM opens DOWN direction

typedef enum {
	LEFT, RIGHT, UP, DOWN
} Direction;

typedef struct point {
	int row;
	int col;
} Point;

typedef struct {
	Point *points;
	int length;
} Path;

//map destructor
void map_dtor(Map *map)
{
	map->rows = map->cols = 0;
	free(map->cells);
	map->cells = NULL;
}

//get value of a map cell
unsigned char map_get_item(Map *map, int rows, int cols)
{
	return map->cells[rows * map->cols + cols];
}

//returns true if queried border is a wall
bool isborder(Map *map, int r, int c, int border)
{
	unsigned char curr_cell = map_get_item(map, r, c);
	switch (border)
	{
		case LEFT:			return curr_cell & 0b001;
		case RIGHT:			return curr_cell & 0b010;
		case UP: case DOWN:	return curr_cell & 0b100;
	}
	return false;
}

//returns orientation of a triangle cell to open up or down direction
Orientation get_triangle_orientation(int r, int c)
{
	if (((r + 1) & 1) ^ ((c + 1) & 1))
		return BOTTOM;
	return TOP;
}

//finds the next direction based on position, previous direction and left/right path rule
int find_direction(Map *map, int r, int c, int direction, int leftright)
{
	//error for invalid path rule
	if (leftright != LEFT && leftright != RIGHT)
		return -1;

	//check the next side of the triangle, search for an opening
	do
	{
		switch (direction)
		{
			case LEFT:
				if (get_triangle_orientation(r, c) == BOTTOM)
					direction = leftright == RIGHT ? DOWN : RIGHT;
				else
					direction = leftright == RIGHT ? RIGHT : UP;
				break;
			case RIGHT:
				if (get_triangle_orientation(r, c) == BOTTOM)
					direction = leftright == RIGHT ? LEFT : DOWN;
				else
					direction = leftright == RIGHT ? UP : LEFT;
				break;
			case UP:
				direction = leftright == RIGHT ? LEFT : RIGHT;
				break;
			case DOWN:
				direction = leftright == RIGHT ? RIGHT : LEFT;
				break;
		}
	}
	while (isborder(map, r, c, direction));

	return direction;
}

//sets the start direction based on queried part of the labyrint map
int start_border(Map *map, int r, int c, int leftright)
{
	int result = -1;

	//start from left
	if (c == 0 && !isborder(map, r, c, LEFT))
	{
		if (get_triangle_orientation(r, c) == TOP)
			result = leftright == RIGHT ? RIGHT : UP;
		else
			result = leftright == RIGHT ? DOWN : RIGHT;
	}

	//start from right
	else if (c == map->cols - 1 && !isborder(map, r, c, RIGHT))
	{
		if (get_triangle_orientation(r, c) == TOP)
			result = leftright == RIGHT ? UP : LEFT;
		else
			result = leftright == RIGHT ? LEFT : DOWN;
	}

	//start from top
	else if (r == 0 && get_triangle_orientation(r, c) == TOP && !isborder(map, r, c, UP))
	{
		if (c == 0)
			result = RIGHT;
		else if (c == map->cols - 1)
			result = LEFT;
		else
			result = leftright == RIGHT ? LEFT : RIGHT;
	}

	//start from bottom
	else if (r == map->rows - 1 && get_triangle_orientation(r, c) == BOTTOM && !isborder(map, r, c, DOWN))
	{
		if (c == 0)
			result = RIGHT;
		else if (c == map->cols - 1)
			result = LEFT;
		else
			result = leftright == RIGHT ? RIGHT : LEFT;
	}

	if (result != -1 && isborder(map, r, c, result))
		return find_direction(map, r, c, result, leftright);

	return result;
}

//make a move based on direction and determine next step based on the left/right path rule
//returns false if moved out of the labyrint map (found an exit)
bool make_move(Map *map, int *current_row, int *current_col, int *direction, int leftright)
{
	//make move
	switch (*direction)
	{
		case LEFT:	(*current_col)--;	*direction = RIGHT;	break;
		case RIGHT:	(*current_col)++;	*direction = LEFT;	break;
		case UP:	(*current_row)--;	*direction = DOWN;	break;
		case DOWN:	(*current_row)++;	*direction = UP;	break;
	}

	//check if found exit (out of map), if not get new direction and continue
	if (*current_row >= 0 && *current_row < map->rows && *current_col >= 0 && *current_col < map->cols)
	{
		*direction = find_direction(map, *current_row, *current_col, *direction, leftright);
		return true;
	}
	return false;
}

//path constructor to initialize
Path path_ctor()
{
	Path path;
	path.length = 0;
	path.points = NULL;
	return path;
}

//Path struct destructor
void path_dtor(Path *path)
{
	free(path->points);
	*path = path_ctor();
}

//path finding function returns found path
Path find_path(Map *map, int start_row, int start_col, int start_direction, int path_rule)
{
	Path path = path_ctor();
	int current_row = start_row;
	int current_col = start_col;
	int direction = start_direction;
	int i = 0;
	do {
		//resize rows and cols arrays for new values to be stored marking path points
		if ((path.points = realloc(path.points, ++path.length * sizeof(Point))) == NULL)
		{
			path_dtor(&path);
			break;
		}
		path.points[i].row = current_row;
		path.points[i++].col = current_col;
	}
	while (make_move(map, &current_row, &current_col, &direction, path_rule));

	return path;
}

void print_path(Path *path)
{
	for (int i = 0; i < path->length; i++)
		printf("%d,%d\n", path->points[i].row + 1, path->points[i].col + 1);
}

bool equal_points(Point *point1, Point *point2)
{
	if (point1->col != point2->col || point1->row != point2->row)
		return false;
	return true;
}

void cut_dead_ends(Path *path)
{
	//check all path points
	for (int i = 1; i < path->length; i++)
	{
		//go back to check for duplicates (returned to the same point -> entered a dead end)
		for (int j = i - 1; j >= 0; j--)
		{
			if (equal_points(&path->points[i], &path->points[j]))
			{
				int continue_point = j;
				int index_dif = i - j;

				//move the correct moves and overwrite the dead end
				for (int y = i; y < path->length; y++)
					path->points[j++] = path->points[y];

				i = continue_point;
				path->length -= index_dif;
				break;
			}
		}
	}
}

Path find_shortest_path(Map *map, int start_row, int start_col)
{
	Path lpath = path_ctor();
	Path rpath = path_ctor();
	int l_dir = start_border(map, start_row, start_col, LEFT);
	int r_dir = start_border(map, start_row, start_col, RIGHT);

	//check valid entrance (start_border returns -1 for invalid)
	if (l_dir != -1 && r_dir != -1)
	{
		//do both left and right hand ruled paths
		lpath = find_path(map, start_row, start_col, l_dir, LEFT);
		rpath = find_path(map, start_row, start_col, r_dir, RIGHT);

		//minimize both paths by cutting dead ends
		cut_dead_ends(&lpath);
		cut_dead_ends(&rpath);
	}
	else
		fprintf(stderr, "Error: Invalid entrance." ERROR_END);

	//choose the shorter path to be the shortest
	if (lpath.length < rpath.length)
		return lpath;
	return rpath;
}

//uses strtol to convert input string to int and check validity of the input format
int load_check_input(char *input)
{
	char *endptr;
	errno = 0;
	int convert = strtol(input, &endptr, 10);

	if (errno != 0 || *endptr != '\0' || convert <= 0)
 		return -1;

	return convert - 1;
}

//constructor tests the given file and returns map if the file is valid
Map map_ctor(char *map_file_name)
{
	Map test_map;
	test_map.cols = test_map.rows = -1;
	test_map.cells = NULL;

	FILE *file = fopen(map_file_name, "r");
	if (file == NULL)
	{
		fprintf(stderr, "Error loading file.\n");
		return test_map;
	}

	fscanf(file, "%d %d\n", &test_map.rows, &test_map.cols);

	if ((test_map.cells = malloc(test_map.rows * test_map.cols * sizeof(unsigned char))) == NULL)
	{
		fclose(file);
		return test_map;
	}

	char tmp;
	int rows_checked = 0, col_cnt = 0, index = 0;

	//check if map matrix has ok rows and cols formating
	for (bool exp_ws = false; (tmp = fgetc(file)) != EOF; exp_ws = !exp_ws) //exp_ws: expecting whitespace
	{
		//not expecting whitespace and is a 3bit number
		if (!exp_ws && tmp >= '0' && tmp <= '7')
		{
			col_cnt++;
			if (test_map.cells != NULL)
				test_map.cells[index++] = tmp - '0';
		}
		else if (!isspace(tmp)) //bad char error out of reach / expecting whitespace
		{
			map_dtor(&test_map);
			fclose(file);
			return test_map;
		}

		if (tmp == '\n')
		{
			rows_checked++;

			//ok if expecting whitespace and numbers on a line counted matches expected cols
			//or if 1st line and loaded rows and cols
			if (exp_ws && (col_cnt == test_map.cols || (rows_checked == 1 && col_cnt == 2)))
			{
				col_cnt = 0;
				continue;
			}
			fclose(file);
			map_dtor(&test_map);
			return test_map;
		}
	}
	fclose(file);

	//then load map and check cell neighbors validity
	if (test_map.rows == rows_checked)
	{
		for (int i = 0; i < test_map.rows; i++)
		{
			for (int j = 0; j < test_map.cols - 1; j++)
			{
				//equivalent borders with a neighbor in a direction

				bool updown = true;
				//check up direction
				if (get_triangle_orientation(i, j) == TOP && i > 0)
					updown = isborder(&test_map, i, j, UP) == isborder(&test_map, i - 1, j, DOWN);
				//check down direction
				else if (get_triangle_orientation(i, j) == BOTTOM && i < test_map.rows - 1)
					updown = isborder(&test_map, i, j, DOWN) == isborder(&test_map, i + 1, j, UP);

				//check cols from left to right
				bool leftright = isborder(&test_map, i, j, RIGHT) == isborder(&test_map, i, j + 1, LEFT);

				if (!(updown && leftright))
				{
					map_dtor(&test_map);
					return test_map;
				}
			}
		}
	}
	else
		map_dtor(&test_map);

	return test_map;
}

void print_help()
{
	//arguments format
	printf("Program can be run in these ways:\n ./proj3 --help\n ./proj3 --test [MAP FILE]\n ./proj3 [PATH RULE] [ROW] [COL] [MAP FILE]\n\n");
	//left/right path rule
	printf("PATH RULE:\n --lpath (left hand rule)\n --rpath (right hand rule)\n --shortest (shortest path)\n\n");
	//start row and col
	printf("ROW, COL:\n Start coordinates marking the entrance of the labyrint map.\n Must be set on the edge of the map and have an open border to be able to enter the labyrint.\n\n");
	//map file
	printf("MAP FILE:\n First line of the map file contains map dimensions (rows and cols with a whitespace in between).\n Next lines contain information about map cells (whitespace in between too) in a rows * cols matrix.\n Cell's format is a 3bit number (decimal from 0-7) (1 for wall, 0 for opening) representing triangle borders (1st bit for the left side, 2nd for the right side and 3rd for the up/down side).\n");
}

int main(int argc, char **argv)
{
	Map map;

	//print help
	if (argc == 2 && strcmp(argv[1], "--help") == 0)
		print_help();

	//run map file validity test
	else if (argc == 3 && strcmp(argv[1], "--test") == 0)
		printf("%s\n", (map_ctor(argv[2])).cells == NULL ? "Invalid" : "Valid");

	else if (argc != 5)
	{
		fprintf(stderr, "Error: Wrong arguments." ERROR_END);
		return 1;
	}

	//run labyrint path search
	else if ((map = map_ctor(argv[4])).cells != NULL)
	{
		//setting parth left/right/shortest rule
		Direction lr_rule;
		if (!strcmp(argv[1], "--rpath"))
			lr_rule = RIGHT;
		else if (!strcmp(argv[1], "--lpath"))
			lr_rule = LEFT;
		else if (!strcmp(argv[1], "--shortest"))
			lr_rule = UP;
		else
		{
			fprintf(stderr, "Error setting path rule." ERROR_END);
			map_dtor(&map);
			return 1;
		}

		//loading start row and col from arguments
		int current_row, current_col;
		if ((current_row = load_check_input(argv[2])) == -1)
		{
			fprintf(stderr, "Error: Invalid ROW argument." ERROR_END);
			map_dtor(&map);
			return 1;
		}
		if ((current_col = load_check_input(argv[3])) == -1)
		{
			fprintf(stderr, "Error: Invalid COL argument." ERROR_END);
			map_dtor(&map);
			return 1;
		}

		//getting the start direction
		int direction = start_border(&map, current_row, current_col, lr_rule);
		if (direction == -1 && lr_rule != UP)
		{
			fprintf(stderr, "Error: Invalid entrance." ERROR_END);
			map_dtor(&map);
			return 1;
		}

		Path path;
		if (lr_rule == UP)
			path = find_shortest_path(&map, current_row, current_col);
		else
			path = find_path(&map, current_row, current_col, direction, lr_rule);

		print_path(&path);

		path_dtor(&path);
		map_dtor(&map);
	}
	else
	{
		fprintf(stderr, "Error: Invalid file." ERROR_END);
		return 1;
	}
	return 0;
}