#ifndef _UNO_H_
#define _UNO_H_

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <time.h>

#include "card.h"
#include "Player.h"
#include "simplebitmap.h"
#include "hashtable.h"
#include "fslazywindow.h"
#include "ysglfontdata.h"
#include "yspng.h"

using namespace std::literals::chrono_literals; // ns, us, ms, s, h, etc.

class Uno  
{
private:
    int num_players;        // Number of players
    char dir;               // Direction of the game
    Player *root;           // Root Player - Never changes, controls the server computer
    Player* prev_player;
    mutable Player *curr_player;    // Current player 
    std::vector<std::unique_ptr<Card>> deck;
    std::vector<std::unique_ptr<Card>> discardPile;
    std::vector<std::pair<int,int>> botLocs;
    std::vector<double> botOffsetX;

    char curr_color;
    char curr_value;
    enum {initialNumCardsDealt = 7 };
    int idxPlayerCardToCheck;
    bool endOfGame;
    std::unordered_map<char,std::string> colorMap;
    SimpleBitmap deckPng;
    SimpleBitmap botPng;
    SimpleBitmap mooNOPng;
    SimpleBitmap mooNOBotPng;
    SimpleBitmap mooNOBotDraw2Png;
    HashTable <int, SimpleBitmap> CardHashTable;
    YsRawPngDecoder background;

    
    mutable int numCardsRoot;
    mutable int numRows;
    enum {cardWidth = 80, cardHeight = 120};
    enum {winWidth = 1200, winHeight = 800};
    enum {mooNOButtonWidth = 80, mooNOButtonHeight = 80 };
    mutable double x0,y0;
    mutable int numCardsPerRow;
    mutable double visibleCardWidth;
    enum { handDisplayWidth = 1040 };
    mutable int mx,my;
    bool clickOccured;
    bool caughtBot;
    bool botYellsUno;

    void MakeColorMap();
    void LoadDeckPng();
    void LoadBackground();
    void LoadBotPng();
    void LoadMooNOPng();
    void LoadMooNOBotPng();
    void LoadMooNOBotDraw2Png();
    void InitializePlayers();
    int GetKeyFromColorValue(char c, char v) const;
    void InsertCutoutIntoTable(int i, SimpleBitmap &cutout);
    void MakeCardCutouts();
    void InitializeDeck();
    void ShuffleDeck();
    void DrawCard(Player* aPlayer, int nCards);
    void DealCards();
    void InitDeckDiscardPile();
    void CommFaceUpCardToPlayer();

    void SetNextPlayer();
    void SetNextPlayerWithSkip();
    void SetNextPlayerWithReverse();

    void PrintFaceUpCard() const;
    bool IsValid(int idx) const;
    void TakeAction();
    void ShowGameState() const;
    void PlayerActionMessage(int n) const;
    void CheckIfPlayerWonGame();

    void SetPrevPlayer();

    bool MooNOButtonClicked() const;

    void CallMooNOOnBot();


    

    
public:
    Uno();
    ~Uno() = default;
    void SetupGame(int n);
    void GetXYClickLoc(int x,int y) const;
    void SetClickFlagTrue();
    void Play();

    char gameLevel; // 'e', 'm', 'h'

    inline bool HasGameEnded() const { return endOfGame; }

    inline Player* GetRoot()
    {
        return root;
    }

    inline Player* GetCurrentPlayer()
    {
        return curr_player;
    }

    inline char GetCurrentColor()
    {
        return curr_color;
    }

    inline char GetCurrentValue()
    {
        return curr_value;
    }

    void RunGameCadence();

// following are all draw functions which main program will need to call
private:
	void DisplayACard(char c, char v, double x, double y) const;
	void DisplayRootHand() const;
    int GetClickedCardIndex() const;
    void DisplayDiscardPile() const;
    void DisplayDeck() const;
    void DisplayPassButton() const;
    void DisplayMooNOButton() const;
    void DrawCurrentColorBar() const;
    void HighlightRootCards() const;
    void DrawColorChoicePalette() const;
    void DisplayBackground() const;
    void GetUserColorChoice();
    void DisplayBots() const;
    void CalculateBotLocs();
public:
    void DrawGameState() const;
    void DisplayQuitButton() const;
    void DisplayNewGameButton() const;
    void DisplayEndGameMsg() const;
};

#endif