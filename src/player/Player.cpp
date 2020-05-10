#include "Player.h"

Player::Player(char n, double x, double y)
	: name{ n }, antiClockwise{ nullptr }, clockwise{ nullptr }, preferredColor{ 'u' }, cardDrawn{ false }, cardDrawnOnce{ false }, posX{ x }, posY{ y }, yelledUno{ true }, playAgg{false}, playKind{ false }, chooseRandGameCol{false}
{
}

void Player::SwapCards(int idx)
{
	std::swap(hand.at(idx), hand.back());
}

Human::Human(char n, double x, double y) 
	: Player(n,x,y) 
{
}

Bot::Bot(char n, double x, double y)
	: Player(n,x,y)
{
}

void Player::PrintHand() const
{	
	std::cout << "______________________" << std::endl;
	printf("Cards in Player %c's hand\n", name);
	std::cout << "______________________" << std::endl;
	std::cout << "Index\t(Color, Value)" << std::endl;
	std::cout << "______________________" << std::endl;
	int index = 1;
	for(const auto& card : hand)
	{
		std::cout << index << "\t";
		card->Print();
		index++;
	}
}

int Human::PlayCard()
{	
	int index {-3};
	std::string userIdxChoice;

	PrintHand();

	if(!cardDrawn) 
	{
		std::cout << "Enter card index number you want to play / Enter 'd' to draw a card: ";
	}
	else
	{
		std::cout << "Enter card index number you want to play / Enter 'p' to pass your turn if you don't want to play a card: ";
	}
	
	std::getline (std::cin,userIdxChoice);
	try
	{
		index = std::stoi(userIdxChoice);
	}
	catch(std::exception &e)
	{
		if (!cardDrawn  && (userIdxChoice == "d" || userIdxChoice == "D"))
		{
			cardDrawn = true;
			return -1;		
		}
		else if(cardDrawn && (userIdxChoice == "p" || userIdxChoice == "P"))
		{
			cardDrawn = false;
			return -2;
		}
	}

	while(index < 1 || index > hand.size())
	{
		std::cout << "Invalid card index entered." << std::endl;
		PrintHand();
		if (!cardDrawn) 
		{
			std::cout << "Enter card index number you want to play / Enter 'd' to draw a card: ";
		}
		else
		{
			std::cout << "Enter card index number you want to play / Enter 'p' to pass your turn if you don't want to play a card: ";
		}
		std::getline(std::cin,userIdxChoice);
		try
		{
			index = std::stoi(userIdxChoice);
		}
		catch(std::exception &e)
		{
			if (!cardDrawn  && (userIdxChoice == "d" || userIdxChoice == "D"))
			{
				cardDrawn = true;
				return -1;		
			}
			else if(cardDrawn && (userIdxChoice == "p" || userIdxChoice == "P"))
			{
				cardDrawn = false;
				return -2;
			}
		}
	}

	if(hand.at(index-1)->IsCardValid(currGameColor,currGameValue))
	{
		cardDrawn = false;
	}
	
	return index -1;
}

char Human::ChooseGameColor()
{
	std::cout << "You played a Wildcard! Set the game color." << std::endl;
	std::cout << "Enter r for Red, g for Green, b for Blue or y for Yellow: ";

	std::string userInput;
	std::getline(std::cin, userInput);
	preferredColor = userInput[0];
	
	while(!(preferredColor == 'r' || preferredColor == 'g' || preferredColor == 'b' || preferredColor == 'y' ||
		preferredColor == 'R' || preferredColor == 'G' || preferredColor == 'B' || preferredColor == 'Y'))
	{
		std::cout << "Incorrect color choice." << std::endl;
		std::cout << "Enter r for Red , g for green , b for blue or y for yellow: ";
		std::cin >> preferredColor;
	}

	return preferredColor;
}

// Bot shuffles hand at every turn and plays first valid card
int Bot::PlayCard()
{		
	std::shuffle(hand.begin(), hand.end(), std::mt19937{std::random_device{}()});
	
	if (playAgg)
	{
		for (int index = 0; index < hand.size(); ++index)
		{
			if (hand.at(index)->IsCardValid(currGameColor, currGameValue) &&
				hand.at(index)->IsCardDrawCards())
			{
				cardDrawn = false;
				playAgg = false;
				return index;
			}
		}

		for (int index = 0; index < hand.size(); ++index)
		{
			if (hand.at(index)->IsCardValid(currGameColor, currGameValue) &&
				hand.at(index)->IsCardSkipOrRev())
			{
				cardDrawn = false;
				playAgg = false;
				return index;
			}
		}
	}


	if (playKind)
	{
		for (int index = 0; index < hand.size(); ++index)
		{
			if (hand.at(index)->IsCardValid(currGameColor, currGameValue) &&
				!(hand.at(index)->IsCardDrawCards() || hand.at(index)->IsCardSkipOrRev()))
			{
				cardDrawn = false;
				playKind = false;
				return index;
			}
		}
	}
	
	for (int index=0; index<hand.size(); ++index)
	{
		if(hand.at(index)->IsCardValid(currGameColor, currGameValue))
		{
			cardDrawn = false;
			return index;
		}
	}

	if(!cardDrawn)
	{
		cardDrawn = true;
		return -1;
	}
	else
	{
		cardDrawn = false;
		return -2;
	}
}


char Bot::ChooseGameColor()
{
	int colorCounts[] = { 0,0,0,0 };
	char colors[] = { 'r','g','b','y' };
	std::string colorsText[]{ "Red", "Green", "Blue", "Yellow" };

	for (const auto& card : hand)
	{
		if (card->GetColor() == 'r')
		{
			colorCounts[0]++;
		}
		else if (card->GetColor() == 'g')
		{
			colorCounts[1]++;
		}
		else if (card->GetColor() == 'b')
		{
			colorCounts[2]++;
		}
		else if (card->GetColor() == 'y')
		{
			colorCounts[3]++;
		}
	}
	char colorChoice;
	int maxCount = -1;
	int choiceIdx = -1;
	for (int i = 0; i < 4; ++i)
	{
		if (colorCounts[i] > maxCount)
		{
			maxCount = colorCounts[i];
			colorChoice = colors[i];
			choiceIdx = i;
		}
	}

	if (maxCount == 0 || chooseRandGameCol == true)
	{
		/* initialize random seed: */
		srand(time(NULL));
		int idx = rand() % 4;
		colorChoice = colors[idx];
		std::cout << "Player " << name << " set game color to " << colorsText[idx] << std::endl;
		chooseRandGameCol = false;
		return colorChoice;
	}

	std::cout << "Player " << name << " set game color to " << colorsText[choiceIdx] << std::endl;
	chooseRandGameCol = false;
	return colorChoice;
}
