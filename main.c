#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>

struct bodyPart{
	int row;
	int col;
	struct bodyPart *prev;
	struct bodyPart *next;
	char toPrint;
} typedef bodyPart;

struct food{
	int row;
	int col;
} typedef food;


food foo;
bodyPart *head, *end, *current;
int running = 1;
int input = KEY_RIGHT;

void listenForInput(){
	while(running){
		int temp;
		temp = getch();
		if (temp == KEY_DOWN || temp == KEY_UP || temp == KEY_LEFT || temp == KEY_RIGHT){
			input = temp;
		} 
	}
}

int main() {
	//screen setup
	initscr();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	int width, height;
	getmaxyx(stdscr, height, width);

	for (int i = 0; i < width; i++){
		mvaddch(0, i, '%');
		mvaddch(height - 1, i, '%');
	}
	for (int i = 0; i < height; i++){
		mvaddch(i, width - 1, '%');
		mvaddch(i, 0, '%');
	}
	refresh();

	//create the snake
	head = malloc(sizeof(bodyPart));
	head->row = height/2;
	head->col = width/2;
	head->next = NULL;
	head->prev = NULL;
	head->toPrint = '$';
	mvaddch(head->row, head->col, head->toPrint);
	refresh();
	current = head;
	//input listener thread
	pthread_t listener;
	pthread_create(&listener, NULL, listenForInput, NULL);
	while(running){
		//head move
		if (head->row < height - 1 && head->col < width - 1 && head->row > 0 && head->col > 0){
			mvaddch(head->row, head->col, ' ');
			switch (input){
				case KEY_UP:
					head->row = head->row - 1;
				break;
				case KEY_DOWN:
					head->row = head->row + 1;
				break;
				case KEY_LEFT:
					head->col = head->col - 1;
				break;
				case KEY_RIGHT:
					head->col = head->col + 1;
				break;	
			}
			mvprintw(0, width - 20, "%d", input);
			mvaddch(head->row, head->col, head->toPrint);
		}
		else {running = 0;}
		refresh();
		usleep(20000);
	}
	pthread_join(listener, NULL);
	refresh();
	endwin();
	return 0;
}
