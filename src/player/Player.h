#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include <string>
#include <exception>
#include <random>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <algorithm>
#include <chrono>
#include <thread>

#include "card.h"

using namespace std::literals::chrono_literals; // ns, us, ms, s, h, etc.

#define PI 3.14159265

class Player 
{
private:
	Player* antiClockwise;
	Player* clockwise;

	const double posX, posY; // Player positions

	virtual int PlayCard() = 0;
	virtual char ChooseGameColor() = 0;
	
	void SwapCards(int idx);
	inline int GetNumCards() const { return static_cast<int> (hand.size());}
	inline char GetPlayerName() const { return name; }

	double GetPosX() {return posX;}
	double GetPosY() {return posY;}

	friend class Uno;

protected:
	char name;
	bool cardDrawn;
	bool cardDrawnOnce;
	char preferredColor;
	char currGameColor;
	char currGameValue;
	std::vector<std::unique_ptr<Card>> hand;

	void PrintHand() const;
	
	Player(char n, double x, double y);
	virtual ~Player() = default; // might need to implement to delete raw Player*

	// time_t t0;
	// inline void ResetInitTime() { t0 = time(NULL); }

	bool yelledUno;

	bool playAgg, playKind;

	bool chooseRandGameCol;
};

class Human : public Player
{
private:
	Human(char n, double x, double y);
	virtual ~Human() = default; // need to implement
	
	virtual int PlayCard() override;
	virtual char ChooseGameColor()  override;	
	
	friend class Uno;
};

class Bot : public Player
{
private:
	
	Bot(char n, double x, double y);
	virtual ~Bot() = default;
	
	virtual int PlayCard() override;
	virtual char ChooseGameColor() override;

	friend class Uno;

	
};

#endif