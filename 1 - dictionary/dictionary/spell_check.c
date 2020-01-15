#include <stdio.h>
#include <ctype.h>

#include "dictionary.h"

int main(int argc, char* argv[]) {
	char* filename = (argc == 2) ? argv[1] : "test.txt";
	printf("File to be checked: %s\n", filename);

	// load dictionary
	if (!load()) {
		printf("Could not load dictionary\n");
		return 1;
	}

	// open text file
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Couldn't open %s\n", filename);
		unload();
		return 1;
	}

	// get statistics
	printf("\n==========\nMISSPELLED WORDS\n==========\n");
	int index = 0, misspellings = 0, words = 0;
	char word[LENGTH + 1];

	// iterate through each word in file
	for (int c = fgetc(file); c != EOF; c = fgetc(file)) {
		// allow only alphabetical characters
		if (isalpha(c)) {
			word[index] = c;
			index++;

			// ignore strings longer than max length
			if (index > LENGTH) {
				while ((c = fgetc(file)) != EOF && isalpha(c));
				index = 0;
			}
		}
		else if (isdigit(c)) {
			// ignore
			while ((c = fgetc(file)) != EOF && isalnum(c));
			index = 0;
		}
		else if (index > 0) {
			// got whole word
			word[index] = '\0';
			index = 0;
			words++;

			if (!check(word)) {
				printf("%s\n", word);
				misspellings++;
			}
		}
	}

	// cleanup
	// close file
	fclose(file);

	// unload dictionary
	unload();

	// print out statistics
	printf("\nDictionary word count: %d", size());
	printf("\nText word count:       %d", words);
	printf("\nMisspelled words:      %d", misspellings);

	return 0;
}