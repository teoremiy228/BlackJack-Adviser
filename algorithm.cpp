#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <cmath>

using namespace std;

// Function to initialize the deck
vector<string> initializeDeck(int numDecks) {
    vector<string> deck;
    string cards[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    for (int i = 0; i < numDecks; ++i) {
        for (const string &card : cards) {
            for (int j = 0; j < 4; ++j) {  
                deck.push_back(card);
            }
        }
    }
    return deck;
}

// Function to get the value of the card
int cardValue(const string &card) {
    unordered_map<string, int> cardValues = {
        {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6},
        {"7", 7}, {"8", 8}, {"9", 9},
        {"10", 10}, {"J", 10}, {"Q", 10}, {"K", 10}, {"A", 11}
    };
    return cardValues[card];
}

// Function to calculate advanced probability for the player considering dealer's possible hands
double calculateWinningProbability(int playerTotal, int dealerCardValue, const vector<string> &deck) {
    int winCount = 0;
    int remainingCards = deck.size();

    // Advanced calculation considering different dealer outcomes
    for (const string &card : deck) {
        int newPlayerTotal = playerTotal + cardValue(card);

        if (newPlayerTotal > 21) continue;

        // Calculate all possible dealer outcomes
        int dealerWins = 0, dealerBusted = 0;

        for (const string &dealerHiddenCard : deck) {
            if (dealerHiddenCard == card) continue; 

            int dealerTotal = dealerCardValue + cardValue(dealerHiddenCard);

            // Dealer keeps hitting until 17 or higher
            while (dealerTotal < 17 && !deck.empty()) {
                for (const string &nextCard : deck) {
                    if (nextCard == card || nextCard == dealerHiddenCard) continue;

                    dealerTotal += cardValue(nextCard);
                    break;
                }
            }

            if (dealerTotal > 21) {
                dealerBusted++;
            } else if (dealerTotal >= newPlayerTotal) {
                dealerWins++;
            }
        }

        // Player wins if dealer busts or player has a higher total
        if (dealerBusted > dealerWins) {
            winCount++;
        }
    }

    return static_cast<double>(winCount) / remainingCards;
}

// Function to calculate the True Count
double calculateTrueCount(int runningCount, int deckSize, int numDecks) {
    return static_cast<double>(runningCount) / (deckSize / 52.0);
}

// Function to make a decision based on the probabilities
string makeDecision(double playerWinProbability, bool canDoubleDown = false) {
    if (canDoubleDown) {
        return "Double Down";
    }
    if (playerWinProbability > 0.6) {
        return "Stand";
    } else {
        return "Hit";
    }
}

// Function to play a hand
void playHand(int &playerTotal, int dealerCardValue, vector<string> &deck) {
    while (playerTotal < 21) {
        double playerWinProbability = calculateWinningProbability(playerTotal, dealerCardValue, deck);
        bool canDoubleDown = (playerTotal == 9 || playerTotal == 10 || playerTotal == 11);

        string decision = makeDecision(playerWinProbability, canDoubleDown);
        cout << "Recommended action: " << decision << endl;

        if (decision == "Stand") {
            break;
        } else if (decision == "Double Down") {
            string card;
            cout << "Enter the card you receive after doubling down: ";
            cin >> card;
            if (card == "exit") return;
            playerTotal += cardValue(card);
            deck.erase(remove(deck.begin(), deck.end(), card), deck.end());
            cout << "Your new total: " << playerTotal << endl;
            break;
        } else {
            string card;
            cout << "Enter next card: ";
            cin >> card;
            if (card == "exit") return;
            playerTotal += cardValue(card);
            deck.erase(remove(deck.begin(), deck.end(), card), deck.end());

            cout << "Your new total: " << playerTotal << endl;
        }
    }

    if (playerTotal > 21) {
        cout << "You busted!" << endl;
    } else if (playerTotal == 21) {
        cout << "You got a blackjack!" << endl;
    } else {
        cout << "You chose to stand with a total of " << playerTotal << endl;
    }
}

int main() {
    srand(time(0));

    int numDecks;
    cout << "How many decks: ";
    cin >> numDecks;

    vector<string> deck = initializeDeck(numDecks);
    int runningCount = 0;

    while (true) {
        string card;
        int playerTotal = 0;

        cout << "Enter your first card: ";
        cin >> card;
        if (card == "exit") break;
        playerTotal += cardValue(card);
        deck.erase(remove(deck.begin(), deck.end(), card), deck.end());

        cout << "Enter your second card: ";
        cin >> card;
        if (card == "exit") break;
        playerTotal += cardValue(card);
        deck.erase(remove(deck.begin(), deck.end(), card), deck.end());

        // Handle split scenario
        if (cardValue(card) == cardValue(deck[0])) {
            cout << "Do you want to split? (yes/no): ";
            string splitChoice;
            cin >> splitChoice;
            if (splitChoice == "exit") break;

            if (splitChoice == "yes") {
                cout << "Playing first hand..." << endl;
                playHand(playerTotal, cardValue(deck[1]), deck);

                cout << "Playing second hand..." << endl;
                playerTotal = cardValue(deck[0]) + cardValue(deck[1]);
                deck.erase(deck.begin(), deck.begin() + 2);
                playHand(playerTotal, cardValue(deck[1]), deck);
                continue;
            }
        }

        // Input dealer's card
        string dealerCard;
        cout << "Enter dealer's card: ";
        cin >> dealerCard;
        if (dealerCard == "exit") break;
        int dealerCardValue = cardValue(dealerCard);
        deck.erase(remove(deck.begin(), deck.end(), dealerCard), deck.end());

        runningCount += cardValue(card);

        double trueCount = calculateTrueCount(runningCount, deck.size(), numDecks);

        if (trueCount >= 1) {
            playHand(playerTotal, dealerCardValue, deck);
        } else {
            cout << "True Count is below 1. Stop playing!" << endl;
        }

        // Reinitialize deck if it's running low
        if (deck.size() < 15) {
            deck = initializeDeck(numDecks);
            runningCount = 0;  // Reset the running count for a fresh deck
        }
    }

    return 0;
}
