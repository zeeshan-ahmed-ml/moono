#include "card.h"

int main()
{
	std::vector<std::unique_ptr<Card>> hand;

	hand.emplace_back(std::make_unique<Card>('r','4'));
	hand.emplace_back(std::make_unique<Card>('g','7'));

	std::cout << "Before swap" << std::endl;
	for(const auto &card : hand)
		card->Print();	

	std::swap(hand.at(0),hand.back());
	std::cout << "After swap" << std::endl;
	for(const auto &card : hand)
		card->Print();	

	// std::vector<std::unique_ptr<Card>> deck;
	
	// initializeDeck(deck);

	// for(const auto &card : deck)
	// 	card->print();
	// std::cout << std::endl << "Deck has " << deck.size() << " cards." << std::endl;

	// std::cout << std::endl <<std::endl <<std::endl <<std::endl;

	// std::cout << "Shuffling deck." << std::endl << std::endl;
	// shuffleDeck(deck);
	// for(const auto &card : deck)
	// 	card->print();
	// std::cout << std::endl << "Deck has " << deck.size() << " cards." << std::endl;	

	// std::vector<std::unique_ptr<Card>> discardPile;

	// discardPile.emplace_back(std::move(deck.back()));
	// deck.pop_back();

	// std::cout << std::endl << "Adding top card of deck to discard pile" << std::endl
	// 		<< std::endl;
	// std::cout << "Discard pile cards:" << std::endl;
	// for(const auto &card : discardPile)
	// {
	// 	card->print();
	// }
	// std::cout << std::endl << "Deck has " << deck.size() << " cards." << std::endl;		
	return 0;
}