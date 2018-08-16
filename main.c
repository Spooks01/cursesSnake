#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>

struct bodyPart{
	int row;
	int col;
	struct bodyPart *prev;
	struct bodyPart *next;
} typedef bodyPart;

struct food{
	int y;
	int x;
} typedef food;


food *foo;
bodyPart *head, *end, *current;
int running = 1;
int input = KEY_RIGHT;
int score = 0;
void listenForInput(){
	while(running){
		int temp;
		temp = getch();
		if (temp == KEY_DOWN || temp == KEY_UP || temp == KEY_LEFT || temp == KEY_RIGHT){
			input = temp;
		} 
	}
}

void spawnFood(){
	int width, height;
	getmaxyx(stdscr, height, width);
	foo = malloc(sizeof(food));
	foo->y = rand() % (height - 2);
	foo->x = rand() % (width - 2);
	mvaddch(foo->y, foo->x, 'o');
	while(running){
		usleep(10000000);
		mvaddch(foo->y, foo->x, ' ');	
		foo->y = rand() % (height - 2);
		foo->x = rand() % (width - 2);
		mvaddch(foo->y, foo->x, 'o');
	}
}

void checkCollision(){
	bodyPart *currentCheck;
	currentCheck = head->prev;
	while(currentCheck != NULL){
		if (head->row == currentCheck->row && head->col == currentCheck->col){
			running = 0;
		}				
	currentCheck = currentCheck->prev;
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
	end = malloc(sizeof(bodyPart));
	head->prev = end;
	end->row = head->row;
	end->col = head->col - 1;
	end->next = head;
	end->prev = malloc(sizeof(bodyPart));
	end->prev->next = end;
	end->prev->prev = NULL;
	end = end->prev;
	end->row = head->row;
	end->col = head->col - 2;
	mvaddch(head->row, head->col, '$');
	mvaddch(head->prev->row, head->prev->col, '*');
	mvaddch(end->row, end->col, '*');
	refresh();
	int length = 3;
	//input listener thread
	pthread_t listener, foodSpawner, collisionCheck;
	pthread_create(&listener, NULL, listenForInput, NULL);
	pthread_create(&foodSpawner, NULL, spawnFood, NULL);
	//pthread_create(&collisionCheck, NULL, checkCollision, NULL);
	while(running){
		if ((foo != NULL) && (head->row == foo->y) && (head->col == foo->x)) {
			score++;
			length++;
			end->prev = malloc(sizeof(bodyPart));
			end->prev->next = end;
			end = end->prev;
			//add bodypart
		}
		//head move
		
		if (head->row < height - 1 && head->col < width - 1 && head->row > 0 && head->col > 0){
			//mvaddch(head->row, head->col, ' ');
			//work back from head, then move head along, then print everything and add blank after end piece
			current = end;
			mvaddch(end->row, end->col,' ');
			pthread_create(&collisionCheck, NULL, checkCollision, NULL);
			while (current->next != NULL){
				current->row = current->next->row;
				current->col = current->next->col;
				mvaddch(current->row, current->col, '*');
				current = current->next;
			}
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
			mvprintw(0, width - 40, "Length: %d", length);
			mvprintw(0, width - 20, "Score: %d", score);
			mvaddch(head->row, head->col, '$');
			pthread_join(collisionCheck, NULL);
		}
		else {running = 0;}
		refresh();
		usleep(20000);
	}
	clear();
	mvprintw(height/2 -1 , width/2 - 10, "Score: %d\0", score);
	mvprintw(height/2, width/2 - 10, "You Lose, CTRL+c exits\0");
	refresh();
	pthread_join(listener, NULL);
	pthread_join(foodSpawner, NULL);
	endwin();
	return 0;
}
