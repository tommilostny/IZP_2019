/*	proj1.c
 *
 *	Projekt 1 - Práce s textem
 *	Přerušená posloupnost - prémiové zadání
 *
 *	Tomáš Milostný (xmilos02)
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h> //strlen
#include <unistd.h> //isatty

#define LINE_MAX_LENGTH 102

//function to check match by phone number
//params: number - phone number; input - program argument input number;
bool is_match_phone(char *number, char *input)
{
	int found_cnt = 0;
	int match_len = strlen(input);

	for (int i = 0; number[i] != '\0'; i++)
	{
		//check if current phone number char matches searched input number (or '+' for #0)
		//increment found count on match and move to next input number
		if (number[i] == input[found_cnt] || (input[found_cnt] == '0' && number[i] == '+'))
			found_cnt++;
		//it's a match when found count is equal to input string length
		if (found_cnt == match_len)
			return true;
	}
	return false;
}

//function to check match by contact's name
//params: name - contact's name; input - program argument input number;
bool is_match_person(char *name, char *input)
{
	int found_cnt = 0;
	int match_len = strlen(input);

	for (int i = 0; name[i] != '\0'; i++)
	{
		//check if current name char is a matching number,
		//increment found count on match and move to next input number
		if (name[i] == input[found_cnt])
			found_cnt++;
		//compare input number with letters range of each number
		else switch (input[found_cnt])
		{
			case '2':
				if ((name[i] >= 'a' && name[i] <= 'c') || (name[i] >= 'A' && name[i] <= 'C'))
					found_cnt++;
				break;
			case '3':
				if ((name[i] >= 'd' && name[i] <= 'f') || (name[i] >= 'D' && name[i] <= 'F'))
					found_cnt++;
				break;
			case '4':
				if ((name[i] >= 'g' && name[i] <= 'i') || (name[i] >= 'G' && name[i] <= 'I'))
					found_cnt++;
				break;
			case '5':
				if ((name[i] >= 'j' && name[i] <= 'l') || (name[i] >= 'J' && name[i] <= 'L'))
					found_cnt++;
				break;
			case '6':
				if ((name[i] >= 'm' && name[i] <= 'o') || (name[i] >= 'M' && name[i] <= 'O'))
					found_cnt++;
				break;
			case '7':
				if ((name[i] >= 'p' && name[i] <= 's') || (name[i] >= 'P' && name[i] <= 'S'))
					found_cnt++;
				break;
			case '8':
				if ((name[i] >= 't' && name[i] <= 'v') || (name[i] >= 'T' && name[i] <= 'V'))
					found_cnt++;
				break;
			case '9':
				if ((name[i] >= 'w' && name[i] <= 'z') || (name[i] >= 'W' && name[i] <= 'Z'))
					found_cnt++;
		}

		//it's a match when found count is equal to input string length
		if (found_cnt == match_len)
			return true;
	}
	return false;
}

//function to check if a subject string is a number
bool is_number(char *subject)
{
	for (int i = 0; subject[i] != '\0'; i++)
	{
		//allowed chars for phone number
		switch (subject[i])
		{
			case '+':
			case ' ':
				continue;
		}
		if (subject[i] < '0' || subject[i] > '9')
			return false;
	}
	return true;
}

//remove \n character from subject string (fgets loads line with \n)
void remove_newline(char *subject)
{
	int len = strlen(subject) - 1;
	if (subject[len] == '\n')
		subject[len] = '\0';
}

int main(int argc, char **argv)
{
	//checking if STDIN refers to a terminal (no file with a contacts list)
	if (isatty(STDIN_FILENO))
	{
		fprintf(stderr, "No file on STDIN\n");
		return 1;
	}
	//checking input number
	if (argc > 1 && !is_number(argv[1]))
	{
		fprintf(stderr, "Invalid input format\n");
		return 1;
	}

	char name[LINE_MAX_LENGTH]; //contact's name string
	char number[LINE_MAX_LENGTH]; //phone number string
	bool found_al1 = false; //found at least one contact flag

	//loading contact's name
	while(fgets(name, LINE_MAX_LENGTH, stdin) != NULL)
	{
		remove_newline(name);
		//error if contact's name is a number or line is longer than max line length
		if ((name[0] != '\0' && is_number(name)) || strlen(name) >= LINE_MAX_LENGTH - 1)
		{
			fprintf(stderr, "Encountered invalid contact's name\n");
			return 1;
		}

		//loading contact's phone number
		if (fgets(number, LINE_MAX_LENGTH, stdin) != NULL)
		{
			remove_newline(number);
			//error if phone number isn't a number or line is longer than max line length
			if (!is_number(number) || strlen(number) >= LINE_MAX_LENGTH - 1)
			{
				fprintf(stderr, "Encountered invalid phone number\n");
				return 1;
			}

			//checking if contact matches search by number or name / is always match without input argument
			if (argc < 2 || is_match_phone(number, argv[1]) || is_match_person(name, argv[1]))
			{
				printf("%s, %s\n", name, number);
				found_al1 = true;
			}
		}
		else //error if loaded a person without a phone number
		{
			fprintf(stderr, "Contact missing a phone number\n");
			return 1;
		}
	}

	if (!found_al1) printf("Not found\n");
	return 0;
}