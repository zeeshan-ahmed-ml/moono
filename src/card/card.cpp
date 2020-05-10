#include "card.h"

Card::Card(char c, char v)
	: color{c}, value{v}
{
}

char Card::GetColor() const 
{
	return color;
}

char Card::GetValue() const
{
	return value;
}

void Card::Print() const
{
	std::cout << "(" << GetColorName() << ", " << GetValueName() << ")" << std::endl;
}

std::string Card::GetColorName() const
{
	if(color == 'r')
	{
		return "Red";
	}
	else if(color == 'g')
	{
		return "Green";
	}
	else if(color == 'b')
	{
		return "Blue";
	}
	else if(color == 'y')
	{
		return "Yellow";
	}
	else if(color == 'w')
	{
		return "Wildcard";
	}
	return "COLOR ERROR";
}

std::string Card::GetValueName() const
{
	if(value == 's')
	{
		return "Skip";
	}
	else if(value == 'r')
	{
		return "Reverse";
	}
	else if(value  == 'd')
	{
		return "Draw 2";
	}
	else if(value == 'c')
	{
		return "Multicolor";
	}
	else if(value == 'f')
	{
		return "Multicolor Draw 4";
	}
	// returning numbered cards below as string
	std::string s(1,value);
	return s;
}

bool Card::IsCardValid(char color, char value) const
{
	if(GetColor() == color)
	{
		return true;
	}
	else if(GetValue() == value)
	{
		return true;
	}
	else if(GetColor() == 'w')
	{
		return true;
	}
	else if(color == 'w')
	{
		return true;
	}
	return false;
}


bool Card::IsCardSkipOrRev() const
{
	if (value == 's' || value == 'r')
	{
		return true;
	}
	return false;
}

bool Card::IsCardDrawCards() const
{
	if (value == 'd' || value == 'f')
	{
		return true;
	}
	return false;
}