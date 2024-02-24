#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "calendar.h"

//returns a pointer to the new item array or NULL if something went wrong
item *addItem(char* content, char* month_s, char* day_s, char* hour_s, item* calendar) {

	//convert to integers
	int month = atoi(month_s);
	int day	  = atoi(day_s);
	int hour  = atoi(hour_s);

	//check for invalid values
	if (month > 12 || month < 0) {
		printf("Invalid month.\n");
		return NULL;
	}
	else if (day > 31 || day < 0) {
		printf("Invalid day.\n");
		return NULL;
	}
	else if (hour > 23 || hour < 0 ){
		printf("Invalid hour.\n");
		return NULL;
	}

	//check if the hour is already reserved while counting the elements
	int i;
	for (i = 0; calendar[i].month != -1; i++) {
		if (calendar[i].month == month && calendar[i].day == day && calendar[i].hour == hour){
			printf("The selected time contains another item.\n");
			return NULL;
		}
	}
	
	//allocate memory for new calendar
	item* newCalendar = realloc(calendar, sizeof(item) * (i + 3));
	if (newCalendar == NULL) {
		return NULL; //realloc failed
	}
	
	//add the item as the second last item
	newCalendar[i].month = month;
	newCalendar[i].day = day;
	newCalendar[i].hour = hour;
	strcpy(newCalendar[i].content, content);
	//add -1 as the last items month
	newCalendar[i + 1].month = -1;

	return newCalendar;
}

//returns a pointer to the new calendar if item was found and deleted else null
item* deleteItem(char* month_s, char* day_s, char* hour_s, item* calendar) {

	//convert to int
	int month = atoi(month_s);
	int day = atoi(day_s);
	int hour = atoi(hour_s);

	int i;
	int found = 0;	//for marking the item found

	//look for the desired hour, if found shift the rest of the items back one space
	for (i = 0; calendar[i].month != -1; i++) {
		if (calendar[i].month == month && calendar[i].day == day && calendar[i].hour == hour) {
			found = 1;
		}
		else if (found) {
			calendar[i - 1] = calendar[i];
		}
	}

	if (found) {
		printf("Item found.\n");
		//mark the last items month as -1
		calendar[i - 1].month = -1;
		//reallocate accordingly
		item *newCalendar = realloc(calendar, sizeof(item) * i);
		if (newCalendar == NULL) {
			return NULL;
		}
		return newCalendar;
	}
	else {
		printf("Item not found.\n");
		return NULL;
	}

}

//lazy implementation by just iterating over all the possible hours of the year and
//comparing them to the items array
void printCalendar(item* calendar) {
	for (int month = 1; month <= 12; month++) {
		for (int day = 1; day <= 31; day++) {
			for (int hour = 1; hour <= 24; hour++) {
				for (int i = 0; calendar[i].month != -1; i++) {
					if (calendar[i].month == month && calendar[i].day == day && calendar[i].hour == hour) {
						printf("%s %d.%d. klo %d\n", calendar[i].content, calendar[i].day, calendar[i].month, calendar[i].hour);
					}
				}
			}
		}
	}
}

//returns 1 if the write was succesful else 0
int saveCalendar(item* calendar, const char* filename) {
	
	FILE* f = fopen(filename, "w");
	if (!f) {
		printf("Failed to create the file.\n");
		return 0;
	}
	//write the contents of an item into a single line in the file
	for (int i = 0; calendar[i].month != -1; i++) {
		item c = calendar[i];
		fputc(c.month, f);
		fputc(c.day, f);
		fputc(c.hour, f);
		fputs(c.content, f);
		fputc('\n', f);
	}
	fclose(f);
	return 1;
}

//returns a pointer to the new calendar if succesful, else null
item* loadCalendar(item* calendar, const char* filename) {

	FILE* f = fopen(filename, "r");
	if (!f) {
		printf("Failed to open the file.\n");
		return NULL;
	}
	//allocate memory for the new calendar
	item* newCalendar = malloc(sizeof(item));
	if (newCalendar == NULL) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	char line[80];
	int  i = 0;
	while (fgets(line, sizeof(line), f)) {
		//strip the newline
		line[strcspn(line, "\n")] = 0;		

		item temp;								
		temp.month = line[0];
		temp.day   = line[1];
		temp.hour  = line[2];
		strcpy(temp.content, &line[3]);
		newCalendar[i] = temp;

		//reallocate memory and check for failure
		item *new_c = realloc(newCalendar, sizeof(item) * (i + 2));
		if (new_c == NULL) {
			return NULL;
		}
		newCalendar = new_c;
		i++;
	}
	//add -1 as the last items month
	newCalendar[i].month = -1;

	fclose(f);
	return newCalendar;
}



int main() {
	//allocate memory for the calendar
	item* calendar = malloc(sizeof(item) * 2);		
	//last item is marked by -1 as the months value
	calendar[0].month = -1;								
	
	while (1) {

		//for initial input
		char input[80];		

		//for parsed input
		char commands[5][20];

		//get user input and strip the newline
		if (fgets(input, 80, stdin)) {
			input[strcspn(input, "\n")] = 0;				
		}

		int i = 0;
		int j = 0;
		int k = 0;

		//parse the input
		while (input[k] != '\0') {
			commands[i][j] = input[k];
			j++;
			k++;
			if (input[k] == ' ') {
				commands[i][j] = '\0';
				i++;
				k++;
				j = 0;
			}
			else if (input[k] == '\0') {
				commands[i][j] = '\0';
			}
		}

		//take the first character of input for simplification
		char choice = commands[0][0];

		if (choice == 'A') {
			//add an item to the calendar
			item* temp = addItem(commands[1], commands[2], commands[3], commands[4], calendar);
			if (temp != NULL) {
				calendar = temp;
				printf("Added a new item.\n");
			}
			else {
				printf("Failed to add a new item.\n");
			}
		}
		else if (choice == 'D') {
			//delete an item from the calendar
			item* temp = deleteItem(commands[1], commands[2], commands[3], calendar);
			if (temp != NULL) {
				printf("Item deleted succesfully.\n");
				calendar = temp;
			}
			else {
				printf("Failed to delete item.\n");
			}
		}
		else if (choice == 'L') {
			//print the contents of the calendar
			printCalendar(calendar);
		}
		else if (choice == 'W') {
			//save the calendar to a file
			if (saveCalendar(calendar, commands[1])) {
				printf("Calendar saved succesfully.\n");
			}
			else {
				printf("Failed to save calendar.\n");
			}
		}
		else if (choice == 'O') {
			//load a calendar from a file
			item* temp = loadCalendar(calendar, commands[1]);
			if (temp != NULL) {
				free(calendar);
				calendar = temp;
				printf("Loaded the calendar succesfully.\n");
			}
			else {
				printf("Failed to load the calendar.\n");
			}
		}
		else if (choice == 'Q') {
			//exit the program
			break;
		}
		else {
			//first character was something else
			printf("Invalid command.\n");
		}
	}

	//free the memory allocated to the calendar
	free(calendar);

	return 0;
}
