#ifndef _CARD_H_
#define _CARD_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
// #include "fslazywindow.h"

class Card
{
private:
	char color; // r, g, b, y, w // HashKey: 1000, 2000, 3000, 4000, 5000
	// 0,1,2,3,4,5,6,7,8,9,s(skip),r(reverse),d(draw), c(color), f(wild draw4)
	// HashKey: use char
	char value;
	std::string GetColorName() const;
	std::string GetValueName() const;
public:
	Card(char c, char v);
	~Card() = default;
	// virtual Draw() = 0;
	char GetColor() const;
	char GetValue() const;
	void Print() const;
	bool IsCardValid(char color, char value) const;
	bool IsCardSkipOrRev() const;
	bool IsCardDrawCards() const;
};

#endif