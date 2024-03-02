#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_CARDS 108 // change for different sized decks

// --------------------------------------
// UNO GAME
// WRITTEN BY MATTHEW WASKO
// --------------------------------------

typedef struct card_s {
	char color[7];
	int value;
	char action[5];
	struct card_s* pt;
} card;

void loadDeck(card* deck, FILE* inp); // loads the deck from file
void printDeck(card* deck); // reads the deck into console for debugging
void shuffleDeck(card* deck, int numTimes); // shuffles the deck a specific number of times
void dealCards(card* deck, card** head1p, card** tail1p, card** head2p, card** tail2p); // deals cards to each player
void deleteMembers(card** h, card** t, card* target); // removes a card from the linked list of a player
void addMember(card** h, card** t, card deckCard); // adds a card to the linked list of the player
void readHand(card* playerHeadp); // reads the hand of a player into console
void readDiscardPile(card* discP); // reads the discard pile (where players play cards) into console
void playerTurnFunc(card** plrHead, card** plrTail, card** oppHead, card** oppTail, card* discP, int* outcome, int* stackSize, int* deckPosition); // where the game logic is processed, rules are followed
int playerHandSize(card* plrHand); // finds the size of the player hand
card* getCardFromIndex(card* h, int index); // gets the card of a player in a  specific index of the linked list

int main(void) {

	FILE* inp;
	card* deck = NULL;
	deck = (card*)calloc(NUM_CARDS, sizeof(card)); // reading deck of cards file to dynamic array --------------------------------------------------------------------

	char playagain = 'y';
	while (playagain == 'y') {
		printf("Lets play a game of UNO\n");

		int numCards = 0;
		int userFileResponse = 0;

		while (userFileResponse != 1 && userFileResponse != 2) {
			printf("Press 1 to shuffle the UNO deck or 2 to load a deck from a file: ");
			scanf("%d", &userFileResponse);
			if (userFileResponse == 1) {
				inp = fopen("deck.txt", "r");
				loadDeck(deck, inp);
				fclose(inp);
			}
			else if (userFileResponse == 2) {
				char userFileName[20];
				printf("Enter the name of your file: ");
				scanf("%s", userFileName);
				userFileName[strlen(userFileName)] = '\0'; // removing newline character
				inp = fopen(userFileName, "r");
				if (inp != NULL) {
					loadDeck(deck, inp);
					fclose(inp);
				}
				else {
					printf("Not a valid file type\n");
					while (inp == NULL) {
						printf("Enter the name of your file: ");
						scanf("%s", userFileName);
						userFileName[strlen(userFileName)] = '\0'; // removing newline character
						inp = fopen(userFileName, "r");
						if (inp == NULL) {
							printf("Not a valid file type\n");
						}
					}
					loadDeck(deck, inp);
					fclose(inp);
				}
			}
			else {
				printf("Please select 1 or 2\n");
			}
		}
		// done reading deck of cards -----------------------------------------------------------------------------

		// shuffling

		int numTimesShuffle = 100000; // change to alter # of times shuffled

		//printf("Enter the number of times you want to shuffle: ");
		//scanf("%d", &numTimesShuffle);
		shuffleDeck(deck, numTimesShuffle);

		//printDeck(deck);
		// 
		// done shuffling ------------------------------------------------------------------------------------------
		printf("The deck is shuffled. Dealing cards\n");
		// creating user hands and discard pile

		card* user1headp, * user1tailp, * user2headp, * user2tailp;
		user1headp = NULL; user1tailp = NULL; user2headp = NULL; user2tailp = NULL;

		dealCards(deck, &user1headp, &user1tailp, &user2headp, &user2tailp);
		printf("Player 1 hand: ");
		readHand(user1headp); // reading player 1 hand
		printf("Player 2 hand: ");
		readHand(user2headp); // reading player 2 hand

		printf("Hands dealt, starting game\n");

		// creating discard pile, starting game -------------------------------------------------------------
		int discardPos = 14; // **** WHEN REACHES 108, RE SHUFFLE DECK IN GAME CODE ****


		card* discardPile;
		discardPile = (card*)malloc(sizeof(card));

		discardPile = NULL;
		discardPile = &deck[discardPos];

		readDiscardPile(discardPile);
		discardPos++;

		// player turns, playing game ------------------------------------------------------------------------------------------
		int player1Turn = 1; // 1 = player1, 2 = player2
		int numStacks = 0;
		bool gameOver = false; // true when a player has zero cards left
		while (gameOver == false) {
			printf("\n");
			if (discardPos == NUM_CARDS) {
				printf("Deck is out of available cards. Reshuffling\n");
				shuffleDeck(deck, 100000);
				discardPos = 1;
				printf("Deck has been shuffled\n");
			}
			if (player1Turn == 1) {
				printf("Player 1 turn\n");
				printf("\n");
				printf("Player 1 hand: ");
				readHand(user1headp);
				playerTurnFunc(&user1headp, &user1tailp, &user2headp, &user2tailp, discardPile, &player1Turn, &numStacks, &discardPos);
				readDiscardPile(discardPile);
			}
			else if (player1Turn == 2) {
				printf("Player 2 turn\n");
				printf("\n");
				printf("Player 2 hand: ");
				readHand(user2headp);
				playerTurnFunc(&user2headp, &user2tailp, &user1headp, &user1tailp, discardPile, &player1Turn, &numStacks, &discardPos);
				readDiscardPile(discardPile);
			}
			if (numStacks > 0) {
				if (discardPile->value != 12) {
					for (int i = 0; i < (2 * numStacks); i++) {
						if (player1Turn == 1) {
							addMember(&user2headp, &user2tailp, discardPile[discardPos]);
							discardPos = discardPos + 1;
						}
						else {
							addMember(&user1headp, &user1tailp, discardPile[discardPos]);
							discardPos = discardPos + 1;
						}
					}
					numStacks = 0;
				}
			}
			if (playerHandSize(user1headp) == 1) {
				printf("\n | Player 1 has UNO! |\n");
				readDiscardPile(discardPile);
			}
			else if (playerHandSize(user2headp) == 1) {
				printf("\n | Player 2 has UNO! |\n");
				readDiscardPile(discardPile);
			}
			if (playerHandSize(user1headp) == 0) {
				printf("\nPlayer 1 wins!\n");
				gameOver = true;
			}
			else if (playerHandSize(user2headp) == 0) {
				printf("\nPlayer 2 wins!\n");
				gameOver = true;
			}
			// readDiscardPile(discardPile);
		}



		printf("\nGame over\n"); // for when one player has zero cards
		printf("Do you want to play again? (y/n): ");
		scanf(" %c", &playagain);
		printf("\n");
		//gameOver = true;
	// }
		//free(discardPile);
		free(deck);
	// closing out game -----------------------------------------------------------------------------------------
	}
	printf("\nGoodbye!\n");
	return 0;

	//free(discardPile);
	//free(deck);

}

void loadDeck(card* deck, FILE* inp) {
	int position = 0;
	while (!feof(inp)) {
		card* temp = (card*)malloc(sizeof(card));
		fscanf(inp, "%d %s %s", &temp->value, temp->color, temp->action);
		deck[position] = *temp;
		position++;

		free(temp);
	}
}

void printDeck(card* deck) {
	for (int i = 0; i < NUM_CARDS; i++) {
		printf("%d %s %s\n", deck[i].value, deck[i].color, deck[i].action);
	}
}

void shuffleDeck(card* deck, int numTimes) {
	srand(time(NULL));
	for (int i = 0; i < numTimes; i++) {
		int pos1 = rand() % NUM_CARDS; // getting two random indexes to swap in deck
		int pos2 = rand() % NUM_CARDS;

		card* temp; temp = (card*)malloc(sizeof(card));

		temp->value = deck[pos1].value;
		strcpy(temp->color, deck[pos1].color);
		strcpy(temp->action, deck[pos1].action);

		// moving card 2 into card 1
		deck[pos1].value = deck[pos2].value;
		strcpy(deck[pos1].color, deck[pos2].color);
		strcpy(deck[pos1].action, deck[pos2].action);

		// moving temp into card 2
		deck[pos2].value = temp->value;
		strcpy(deck[pos2].color, temp->color);
		strcpy(deck[pos2].action, temp->action);

		free(temp);
	}
}

void addMember(card** h, card** t, card deckCard) {
	card* temp;
	temp = (card*)malloc(sizeof(card));

	temp->value = deckCard.value;
	strcpy(temp->color, deckCard.color);
	strcpy(temp->action, deckCard.action);

	if (*h == NULL) {
		*h = temp;
	}
	else {
		(*t)->pt = temp;
	}
	*t = temp;
	temp->pt = NULL;
}

void deleteMembers(card** h, card** t, card* target) { // target is the index you want to delete in player hand
	card* temp = *h;
	int handSize = playerHandSize(*h);

	if (target == (*h)) {
		(*h) = target->pt;
	}
	else if (target->pt == NULL) {
		while (temp->pt != target) {
			temp = temp->pt;
		}
		*t = temp;
		temp->pt = NULL;
	}
	else {
		while (temp->pt != target) {
			temp = temp->pt;
		}
		temp->pt = target->pt;
	}
	free(target); //freeing the deleted/played card from memory in the players hand
}

card* getCardFromIndex(card* h, int index) {
	for (int i = 0; i < index; i++) {
		h = h->pt;
	}
	return h;
}

void dealCards(card* deck, card** head1p, card** tail1p, card** head2p, card** tail2p) {
	int position = 0;
	while (position <= 13) { // First 14 cards of a deck (starts from 0)
		addMember(head1p, tail1p, deck[position]);
		position++;

		addMember(head2p, tail2p, deck[position]);
		position++;
	}
}

void readHand(card* playerHeadp) {
	int count = 1;
	while (playerHeadp != NULL) {
		if (playerHeadp->pt == NULL) {
			if (strcmp(playerHeadp->action, "basic") == 0) {
				printf("%d) %d %s\n", count, playerHeadp->value, playerHeadp->color);
			}
			else if ((strcmp(playerHeadp->action, "wild") == 0) || (strcmp(playerHeadp->action, "wildp4") == 0)) {
				printf("%d) %s\n", count, playerHeadp->action);
			}
			else {
				printf("%d) %s %s\n", count, playerHeadp->color, playerHeadp->action);
			}
		}
		else {
			if (strcmp(playerHeadp->action, "basic") == 0) {
				printf("%d) %d %s, ", count, playerHeadp->value, playerHeadp->color);
			}
			else if ((strcmp(playerHeadp->action, "wild") == 0) || (strcmp(playerHeadp->action, "wildp4") == 0)) {
				printf("%d) %s, ", count, playerHeadp->action);
			}
			else {
				printf("%d) %s %s, ", count, playerHeadp->color, playerHeadp->action);
			}
		}
		playerHeadp = playerHeadp->pt;
		count = count + 1;
	}
	count = 1; // resetting count back to zero
}

void readDiscardPile(card* discP) {
	printf("\n"); // formatting stuffs
	if (strcmp(discP->action, "basic") == 0) {
		printf("Discard pile: %d %s\n", discP->value, discP->color);
	}
	else {
		printf("Discard pile: %s %s\n", discP->color, discP->action);
	}
}

void playerTurnFunc(card** plrHead, card** plrTail, card** oppHead, card** oppTail, card* discP, int* outcome, int* stackSize, int* deckPosition) {
	int numCards = 0;
	int playerInput = -1; // initializing variable
	card* cardPtr; cardPtr = (card*)malloc(sizeof(card));
	cardPtr = *plrHead;
	//card* cardPtr = plrHead;
	numCards = playerHandSize(*plrHead);
	printf("Press 1-%d to play any card from your hand, or press 0 to draw a card from the deck: ", numCards);
	scanf("%d", &playerInput);
	playerInput = playerInput - 1; // to adjust for the positioning of the other function
	if ((playerInput >= 0) && (playerInput <= numCards-1)) {
		for (int i = 1; i <= playerInput; i++) { // moving pointer to the players card that they selected
			cardPtr = cardPtr->pt;
		}
		if (((cardPtr->value) == (discP->value) && (cardPtr->value <= 9)) || ((strcmp(cardPtr->color, discP->color) == 0) && (cardPtr->value <= 9))) {
			discP->value = cardPtr->value;
			strcpy(discP->color, cardPtr->color);
			strcpy(discP->action, cardPtr->action);
			if (*outcome == 1) {
				*outcome = 2;
			}
			else {
				*outcome = 1;
			}
			deleteMembers(plrHead, plrTail, getCardFromIndex(*plrHead, playerInput));
		}
		else if ((cardPtr->value == 10) && ((strcmp(cardPtr->color, discP->color) == 0)) || ((cardPtr->value == 10) && (discP->value == 10))) { // SKIP
			discP->value = cardPtr->value;
			strcpy(discP->color, cardPtr->color);
			strcpy(discP->action, cardPtr->action);
			if (*outcome == 1) {
				*outcome = 1;
			}
			else {
				*outcome = 2;
			}
			deleteMembers(plrHead, plrTail, getCardFromIndex(*plrHead, playerInput));
		}
		else if ((cardPtr->value == 11) && ((strcmp(cardPtr->color, discP->color) == 0)) || ((cardPtr->value == 11) && (discP->value == 11))) { // REVERSE
			discP->value = cardPtr->value;
			strcpy(discP->color, cardPtr->color);
			strcpy(discP->action, cardPtr->action);
			if (*outcome == 1) {
				*outcome = 1;
			}
			else {
				*outcome = 2;
			}
			deleteMembers(plrHead, plrTail, getCardFromIndex(*plrHead, playerInput));
		}
		else if (((cardPtr->value == 12) && (discP->value == 12)) || ((cardPtr->value == 12) && (strcmp(cardPtr->color, discP->color) == 0))) { // PLUS 2
			discP->value = cardPtr->value;
			strcpy(discP->color, cardPtr->color);
			strcpy(discP->action, cardPtr->action);
			/*
			for (int i = 0; i < 2; i++) {
				addMember(oppHead, oppTail, discP[(*deckPosition)]); // adding plus 2 cards to opponent
				*deckPosition = *deckPosition + 1;
			}
			*/
			*stackSize = *stackSize + 1;
			if (*outcome == 1) {
				*outcome = 2;
			}
			else {
				*outcome = 1;
			}
			printf("\n | PLUS 2 stack size: %d | Place another plus2 or draw %d after this turn\n", *stackSize, (*stackSize * 2));
			deleteMembers(plrHead, plrTail, getCardFromIndex(*plrHead, playerInput));
		}
		else if (cardPtr->value == 13) { // WILD CARD
			char userColor = 'Z'; // initailized
			printf("What color do you want to pick? (r/g/b/y): ");
			scanf(" %c", &userColor);
			if (userColor == 'r') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "red");
				strcpy(discP->action, "wild");
			}
			else if (userColor == 'g') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "green");
				strcpy(discP->action, "wild");
			}
			else if (userColor == 'b') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "blue");
				strcpy(discP->action, "wild");
			}
			else if (userColor == 'y') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "yellow");
				strcpy(discP->action, "wild");
			}
			if (*outcome == 1) {
				*outcome = 2;
			}
			else {
				*outcome = 1;
			}
			deleteMembers(plrHead, plrTail, getCardFromIndex(*plrHead, playerInput));
		}
		else if (cardPtr->value == 14) { // WILD PLUS 4
			char userColor = 'Z'; // initailized
			printf("What color do you want to pick? (r/g/b/y): ");
			scanf(" %c", &userColor);
			if (userColor == 'r') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "red");
				strcpy(discP->action, "wildp4");
			}
			else if (userColor == 'g') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "green");
				strcpy(discP->action, "wildp4");
			}
			else if (userColor == 'b') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "blue");
				strcpy(discP->action, "wildp4");
			}
			else if (userColor == 'y') {
				discP->value = cardPtr->value;
				strcpy(discP->color, "yellow");
				strcpy(discP->action, "wildp4");
			}
			for (int i = 0; i < 4; i++) {
				addMember(oppHead, oppTail, discP[(*deckPosition)]); // adding plus 4 cards to opponent
				*deckPosition = *deckPosition + 1;
			}
			if (*outcome == 1) {
				*outcome = 2;
			}
			else {
				*outcome = 1;
			}
			deleteMembers(plrHead, plrTail, getCardFromIndex(*plrHead, playerInput));
		}
		else {
			printf("\nInvalid move\n");
		}

	}
	else if (playerInput == -1) {
		addMember(plrHead, plrTail, discP[(*deckPosition)]);
		*deckPosition = *deckPosition + 1; // moving the current deck position up one
		if (*outcome == 1) {
			*outcome = 2;
		}
		else {
			*outcome = 1;
		}
	}
	else {
		printf("\nInvalid input\n");
	}
}

int playerHandSize(card* plrHand) {
	int handSize = 0;
	while (plrHand != NULL) {
		plrHand = plrHand->pt;
		handSize++;
	}

	return handSize;

}

