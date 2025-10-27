#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>


#define MAX_GUESSES 12
#define CODE_LENGTH 4
#define NUM_COLORS 6
#define WORST_CASE false


void next_combination(int combination[CODE_LENGTH]);
void shuffle_combinations(int (*combinations)[CODE_LENGTH], int const num_combinations);
void calculate_pins(int combination[CODE_LENGTH], int guess[CODE_LENGTH], int *black, int *white);
bool is_combination_possible(int combination[CODE_LENGTH], int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses);
long calculate_number_of_possible_combinations(int (*combinations)[CODE_LENGTH], const int num_combinations, int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses);
void worst_case(int combination[CODE_LENGTH], int (*combinations)[CODE_LENGTH], const int num_combinations, int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses, int (*replies)[2], const int num_replies, int (*still_possible_combinations)[CODE_LENGTH], int num_still_possible);
void average_case(int combination[CODE_LENGTH], int (*combinations)[CODE_LENGTH], const int num_combinations, int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses, int (*replies)[2], const int num_replies, int (*still_possible_combinations)[CODE_LENGTH], int num_still_possible);


int main (int argc, char **argv) {
	
	if (argc != NUM_COLORS + 1) {
		fprintf(stderr, "Invalid arguments. You must provide exactly %d different colors.\n", NUM_COLORS);
        return EXIT_FAILURE;
    }

	printf("\nSETTINGS:\n\n");
	printf("Code length: %d\n", CODE_LENGTH);
	printf("Number of colors: %d\n", NUM_COLORS);
	printf("Maximum number of guesses: %d\n", MAX_GUESSES);
	printf("Mode: %s\n", WORST_CASE ? "worst case" : "average case");
	printf("Colors:");
	for (int i = 1; i < argc; i++) {
		printf(" %s", argv[i]);
	}
	printf("\n");
	
	int combination[CODE_LENGTH];
	int num_guesses = 0;
	int black_pins[MAX_GUESSES];
	int white_pins[MAX_GUESSES];
	int guesses[MAX_GUESSES][CODE_LENGTH];
	
	srand((unsigned) time(NULL));
	rand();
	
	// Compute total number of combinations
	int num_combinations = 1;
	for (int i = 1; i <= CODE_LENGTH; i++) {
		num_combinations *= NUM_COLORS;
	}
	
	// Compute total number of possible replies
	// Note that one reply actually is impossible
	int num_replies = 0;
	for (int i = 0; i <= CODE_LENGTH; i++) {
		for (int j = 0; j + i <= CODE_LENGTH; j++) {
			num_replies++;
		}
	}
	
	// Construct all possible replies
	int replies[num_replies][2];
	num_replies = -1;
	for (int i = 0; i <= CODE_LENGTH; i++) {
		for (int j = 0; j + i <= CODE_LENGTH; j++) {
			num_replies++;
			replies[num_replies][0] = i;
			replies[num_replies][1] = j;
		}
	}
	num_replies++;
	
	// Construct all combinations/guesses	
	int combinations[num_combinations][CODE_LENGTH];
	for (int i = 0; i < CODE_LENGTH; i++) {
		combination[i] = 0;	
	}
	for (int j = 0; j < num_combinations; j++) {
		for (int i = 0; i < CODE_LENGTH; i++) {
			combinations[j][i] = combination[i];
		}
		next_combination(combination);
	}
	
	// Shuffle all combinations to get a more random behavior
	shuffle_combinations(combinations, num_combinations);
	
	// Main loop until the code is guessed
	while (true) {
	
	
		printf("\n********************************************************************************\n\n");
	
		shuffle_combinations(combinations, num_combinations);
		
	 	long num_still_possible = calculate_number_of_possible_combinations(combinations, num_combinations, guesses, black_pins, white_pins, num_guesses);
	 	
	 	printf("There are still %ld valid combinations.\n", num_still_possible);
	 	
	 	int still_possible_combinations[num_still_possible][CODE_LENGTH];
	 	
	 	num_still_possible = -1;
	 	for (int i = 0; i < num_combinations; i++) {
	 		if (is_combination_possible(combinations[i], guesses, black_pins, white_pins, num_guesses)) {
	 			num_still_possible++;
	 			memcpy(still_possible_combinations[num_still_possible], combinations[i], CODE_LENGTH * sizeof(int));
	 		}
	 	}
	 	
	 	num_still_possible++;
	 	
	 	// Hard code behavior to save time
	 	if (num_guesses == 0 && NUM_COLORS == 9 && CODE_LENGTH == 5) {
	 		int perm[NUM_COLORS];
	 		for (int i = 0; i < NUM_COLORS; i++) perm[i] = i;
			for (int i = NUM_COLORS - 1; i >= 1; i--) {
				int j = rand() % (i + 1);
				int help;
				help = perm[i];
				perm[i] = perm[j];
				perm[j] = help;
			}
	 		int perm2[CODE_LENGTH];
	 		for (int i = 0; i < CODE_LENGTH; i++) perm2[i] = i;
			for (int i = CODE_LENGTH - 1; i >= 1; i--) {
				int j = rand() % (i + 1);
				int temp = perm2[i];
				perm2[i] = perm2[j];
				perm2[j] = temp;
			}
			if (WORST_CASE == 1) {
				combination[perm2[0]] = perm[0];
				combination[perm2[1]] = perm[0];
				combination[perm2[2]] = perm[1];
				combination[perm2[3]] = perm[2];
				combination[perm2[4]] = perm[3];
			} else {
				combination[perm2[0]] = perm[0];
				combination[perm2[1]] = perm[1];
				combination[perm2[2]] = perm[2];
				combination[perm2[3]] = perm[3];
				combination[perm2[4]] = perm[4];
			}
		
	 	} else if (WORST_CASE) {
	 		worst_case(combination, combinations, num_combinations, guesses, black_pins, white_pins, num_guesses, replies, num_replies, still_possible_combinations, num_still_possible);
	 	} else {
	 		average_case(combination, combinations, num_combinations, guesses, black_pins, white_pins, num_guesses, replies, num_replies, still_possible_combinations, num_still_possible);
	 	}
		
		printf("My proposed combination for guess %d is:\n\n", num_guesses + 1);
		for (int i = 0; i < CODE_LENGTH; i++) {
			printf("%8s ", argv[combination[i] + 1]);
		}
		printf("\n\n");
		
		bool is_possible = is_combination_possible(combination, guesses, black_pins, white_pins, num_guesses);
		if (is_possible) {
			printf("My proposed guess is valid and could be the solution.\n\n");
		} else {
			printf("My proposed guess is not valid and cannot be the solution.\n\n");
		}
	
		// Insert guess into history and get reply from user
		memcpy(guesses[num_guesses], combination, CODE_LENGTH * sizeof(int));
		printf("Please enter the number of black pins: ");
		scanf("%d", &black_pins[num_guesses]);
		printf("Please enter the number of white pins: ");
		scanf("%d", &white_pins[num_guesses]);
		
		if (black_pins[num_guesses] == CODE_LENGTH) {
			printf("\nDone! Solved in %d guesses!\n", num_guesses + 1);
			break;
		} else {
			num_guesses++;
		}
	
	}
		
	return EXIT_SUCCESS;	
}

void next_combination(int combination[CODE_LENGTH]) {
	for (int i = 0; i < CODE_LENGTH; i++) {
		if (combination[i] < NUM_COLORS - 1) {
			combination[i]++;
			return;
		}
		combination[i] = 0;
	}
}

void shuffle_combinations(int (*combinations)[CODE_LENGTH], const int num_combinations) {
	for (int i = num_combinations - 1; i >= 1; i--) {
		int j = rand() % (i + 1);
		for (int k = 0; k < CODE_LENGTH; k++) {
			int temp = combinations[i][k];
			combinations[i][k] = combinations[j][k];
			combinations[j][k] = temp;
		}
	 }
}

void calculate_pins(int combination[CODE_LENGTH], int guess[CODE_LENGTH], int *black, int *white) {
	int already_considered_combination[CODE_LENGTH];
	int already_considered_guess[CODE_LENGTH];
	for (int i = 0; i < CODE_LENGTH; i++) {
		already_considered_combination[i] = false;
		already_considered_guess[i] = false;
	}
	*black = 0;
	*white = 0;
	// Compute number of black pins
	for (int i = 0; i < CODE_LENGTH; i++) {
		if (combination[i] == guess[i]) {
			already_considered_combination[i] = true;
			already_considered_guess[i] = true;
			(*black)++;
		}
	}
	// Compute number of white pins
	for (int i = 0; i < CODE_LENGTH; i++) {
		if (true == already_considered_guess[i]) continue;
		for (int j = 0; j < CODE_LENGTH; j++) {
			if (true == already_considered_combination[j]) continue;
			if (guess[i] == combination[j]) {
				already_considered_guess[i] = true;
				already_considered_combination[j] = true;
				(*white)++;
				break;
			}
		}
	}
}

bool is_combination_possible(int combination[CODE_LENGTH], int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses) {
	int black, white;
	for (int i = 0; i < num_guesses; i++) {
		calculate_pins(combination, guesses[i], &black, &white);
		if (black != black_pins[i] || white != white_pins[i]) {
			return false;
		}
	}
	return true;
}

long calculate_number_of_possible_combinations(int (*combinations)[CODE_LENGTH], const int num_combinations, int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses) {
	long count = 0;
	for (int i = 0; i < num_combinations; i++) {
		if (is_combination_possible(combinations[i], guesses, black_pins, white_pins, num_guesses)) {
			count++;
		}
	}
	return count;
}

void worst_case(int combination[CODE_LENGTH], int (*combinations)[CODE_LENGTH], const int num_combinations, int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses, int (*replies)[2], const int num_replies, int (*still_possible_combinations)[CODE_LENGTH], int num_still_possible) {
	shuffle_combinations(combinations, num_combinations);
	long best_count = LONG_MAX;
	bool is_best_count_valid = false;
	int guesses_new[num_guesses + 1][CODE_LENGTH];
	int blacks_new[num_guesses + 1];
	int whites_new[num_guesses + 1];
	
	memcpy(guesses_new, guesses, sizeof(int) * CODE_LENGTH * num_guesses);
	memcpy(blacks_new, black_pins, sizeof(int) * num_guesses);
	memcpy(whites_new, white_pins, sizeof(int) * num_guesses);
	
	#pragma omp parallel for firstprivate(guesses_new, blacks_new, whites_new)
	for (int i = 0; i < num_combinations; i++) {
		long count = LONG_MIN;
		memcpy(guesses_new[num_guesses], combinations[i], sizeof(int) * CODE_LENGTH);
		for (int j = 0; j < num_replies; j++) {
			blacks_new[num_guesses] = replies[j][0];
			whites_new[num_guesses] = replies[j][1];
			long num_possible = calculate_number_of_possible_combinations(still_possible_combinations, num_still_possible, guesses_new, blacks_new, whites_new, num_guesses + 1);
			count = num_possible > count ? num_possible : count;
		}
		bool is_valid = is_combination_possible(combinations[i], guesses_new, blacks_new, whites_new, num_guesses);
		if ( (count < best_count) || (count == best_count && is_best_count_valid == false && is_valid) ) {
			#pragma omp critical
			{
				if ( (count < best_count) || (count == best_count && is_best_count_valid == false && is_valid) ) {
					is_best_count_valid = is_valid;
					best_count = count;
					memcpy(combination, combinations[i], sizeof(int) * CODE_LENGTH);
				}
			}
		}
	}
	printf("In the worst case, there will still be %ld valid combinations afterwards.\n\n", best_count);
	
}

void average_case(int combination[CODE_LENGTH], int (*combinations)[CODE_LENGTH], const int num_combinations, int (*guesses)[CODE_LENGTH], int black_pins[MAX_GUESSES], int white_pins[MAX_GUESSES], const int num_guesses, int (*replies)[2], const int num_replies, int (*still_possible_combinations)[CODE_LENGTH], int num_still_possible) {
	shuffle_combinations(combinations, num_combinations);
	long best_count = LONG_MAX;
	int guesses_new[num_guesses + 1][CODE_LENGTH];
	int blacks_new[num_guesses + 1];
	int whites_new[num_guesses + 1];
	
	memcpy(guesses_new, guesses, sizeof(int) * CODE_LENGTH * num_guesses);
	memcpy(blacks_new, black_pins, sizeof(int) * num_guesses);
	memcpy(whites_new, white_pins, sizeof(int) * num_guesses);
	
	#pragma omp parallel for firstprivate(guesses_new, blacks_new, whites_new)
	for (int i = 0; i < num_combinations; i++) {
		long count = 0;
		memcpy(guesses_new[num_guesses], combinations[i], sizeof(int) * CODE_LENGTH);
		for (int j = 0; j < num_replies; j++) {
			blacks_new[num_guesses] = replies[j][0];
			whites_new[num_guesses] = replies[j][1];
			long num_possible = calculate_number_of_possible_combinations(still_possible_combinations, num_still_possible, guesses_new, blacks_new, whites_new, num_guesses + 1);
			count += num_possible * num_possible;
		}
		if (is_combination_possible(combinations[i], guesses_new, blacks_new, whites_new, num_guesses)) {
			count -= 1;
		}
		if (count < best_count) {
			#pragma omp critical
			{
				if (count < best_count) {
					best_count = count;
					memcpy(combination, combinations[i], sizeof(int) * CODE_LENGTH);
				}
			}
		}
	}
	
	long num_possible = calculate_number_of_possible_combinations(still_possible_combinations, num_still_possible, guesses, black_pins, white_pins, num_guesses);
	printf("The expected number of valid combinations after the next reply is %.2lf.\n\n", best_count / (double) num_possible);
}

