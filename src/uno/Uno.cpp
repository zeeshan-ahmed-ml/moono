#include "Uno.h"

Uno::Uno()
	: num_players{1}, dir{'c'}, root{nullptr}, curr_player{nullptr}, prev_player{ nullptr }, endOfGame {false},
    numRows{ 1 }, clickOccured{ false }, caughtBot{false}, botYellsUno{ false }, gameLevel{ 'm' }
{
    MakeCardCutouts(); // read png for cards and store cutouts into hashtable
	MakeColorMap(); // map char to color string/name
	LoadDeckPng();
    LoadBotPng();
    LoadMooNOPng();
    LoadMooNOBotPng();
    LoadMooNOBotDraw2Png();
}

void Uno::MakeColorMap()
{
	char colors[] = {'r','g','b','y'};
    std::string colorsText[] {"Red", "Green", "Blue", "Yellow"};
    for(int i = 0; i < 4; ++i)
    {
    	colorMap[colors[i]] = colorsText[i];
    }
}
void Uno::LoadDeckPng()
{
	if(!deckPng.LoadPng("deck3.png"))
    {
        std::cout << "Error: Failed to read deck.png" << std::endl;
        std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
        return;
    }
    deckPng.Invert();
}

void Uno::LoadBackground()
{
    if(YSERR == background.Decode("menu.png"))
    {
        std::cout << "Error: Failed to read menu background!" << std::endl;
        return;
    }
    background.Flip();
}

void Uno::LoadBotPng()
{
    if(!botPng.LoadPng("cowbot2.png"))
    {
        std::cout << "Error: Failed to read cow bot icon!" << std::endl;
        return;
    }
    botPng.Invert();
    
}


void Uno::LoadMooNOPng()
{
    if (!mooNOPng.LoadPng("moono.png"))
    {
        std::cout << "Error: Failed to read moono button!" << std::endl;
        return;
    }
    mooNOPng.Invert();
}



void Uno::LoadMooNOBotPng()
{
    if (!mooNOBotPng.LoadPng("moonobot.png"))
    {
        std::cout << "Error: Failed to read bot moono call!" << std::endl;
        return;
    }
    mooNOBotPng.Invert();
}



void Uno::LoadMooNOBotDraw2Png()
{
    if (!mooNOBotDraw2Png.LoadPng("moonobotdraw2.png"))
    {
        std::cout << "Error: Failed to read bot moono draw2 call!" << std::endl;
        return;
    }
    mooNOBotDraw2Png.Invert();
}


void Uno::SetupGame(int n)
{   
    endOfGame = false;
    num_players = n;
    // background = png;
    InitializePlayers();
    InitDeckDiscardPile();
    LoadBackground();
    caughtBot = false;
    botYellsUno = false;

    printf("\nWelcome to MooNO, the UNO game by MooPointers\n");
    printf("Starting game with %d Players...\n", num_players);
    printf("You are Player A\n");
}

void Uno::CalculateBotLocs()
{
    // total angle = 220
    // angle diiff = total angle / num bots + 1
    botLocs.clear();
    double r = 350;
    double offset = 70;
    double angTotal = 360 - 2*offset; // 220
    double angDiff = angTotal/(num_players-1); // For 1 bot, angDiff = 220 // For 2 bots, adgDiff = 73.33
    double currBotAng = angDiff/2;
    double botX, botY;

    for (int i=1; i<num_players; ++i)
    {
        // calculate x, y location
        botY = winHeight/2 + r*std::cos((offset+currBotAng) * PI / 180.0);
        botX = winWidth/2 - r*std::sin((offset+currBotAng) * PI / 180.0);
        if (botX < winWidth/2)
        {
            botX -= botY*0.3;
        }
        if (botX > winWidth/2)
        {
            botX += botY*0.3;
        }

        if (i == (num_players + 1) / 2 && (num_players - 1) % 2 != 0) {
            botOffsetX.emplace_back(10);
        }
        else {
            botOffsetX.emplace_back(35);
        }
        
        botLocs.emplace_back(std::make_pair(botX,botY));
        currBotAng += angDiff;
    }
}

void Uno::InitializePlayers()
{
	/*
	Bot names: Texas Longhorn, Jersey, Beefmaster, Beefalo, Kobe, Barzona, Japanese Black, 
	*/

    // Create first player - server/ root
    char playerName = 'A';

    root = new Human(playerName,0,0);
    Player* cur_h = root;

    CalculateBotLocs();

    // Create the other players
    for (int i=0; i<num_players-1; i++)
    {
        auto p = botLocs[i];
        playerName++;
        Player* new_h = new Bot(playerName, p.first, p.second);
        cur_h->clockwise = new_h;
        new_h->antiClockwise = cur_h;
        cur_h=new_h;
    }

    cur_h->clockwise = root; // Last node left points to the start node
    root->antiClockwise = cur_h; // First node right points to last node

    curr_player = root; // Current player is set to the Root player
    SetPrevPlayer();
}


void Uno::SetPrevPlayer()
{
    if (dir == 'c')
    {
        prev_player = curr_player->antiClockwise;
    }
    else
    {
        prev_player = curr_player->clockwise;
    }
}

int Uno::GetKeyFromColorValue(char c, char v) const
{
    int key {0};
    if(c == 'r')
    {
        key += 1000;
    }   
    else if(c == 'g')
    {
        key += 2000;
    }
    else if(c == 'b')
    {
        key += 3000;
    }
    else if(c == 'y')
    {
        key += 4000;
    }
    else if(c == 'w')
    {
        key += 5000;
    }
    else
    {
        std::cout << "ERROR: Incorrect color char in Uno::GetKeyFromColorValue" << std::endl;
    }

    key += static_cast<int>(v);

    return key;    
}

template <>
HashCommon::CodeType HashBase<int>::HashCode(const int &key) const
{
    return key;
}

void Uno::InsertCutoutIntoTable(int i, SimpleBitmap &cutout)
{
    int key {0};
    // Red Numbered cards
    if(i >= 0 && i < 10)
    {
        key = GetKeyFromColorValue('r',static_cast<char>(i)+'0');
    }
    // Yellow Numbered cards
    else if(i >= 14 && i < 24)
    {
        key = GetKeyFromColorValue('y',static_cast<char>(i-14)+'0');
    }
    // Green Numbered cards
    else if(i >= 28 && i < 38)
    {
        key = GetKeyFromColorValue('g',static_cast<char>(i-28)+'0');
    }
    // Blue Numbered cards
    else if(i >= 42 && i < 52)
    {
        key = GetKeyFromColorValue('b',static_cast<char>(i-42)+'0');
    }
    // Wildcard Multicolor
    else if(i == 13)
    {
        key = GetKeyFromColorValue('w','c');
    }
    // Wildcard Draw 4
    else if(i == 69)
    {
        key = GetKeyFromColorValue('w','f');
    }
    // Red Action Cards
    else if(i == 10)
    {
        key = GetKeyFromColorValue('r','s');
    }
    else if(i == 11)
    {
        key = GetKeyFromColorValue('r','r');
    }
    else if(i == 12)
    {
        key = GetKeyFromColorValue('r','d');
    }
    // Yellow Action Cards
    else if(i == 24)
    {
        key = GetKeyFromColorValue('y','s');
    }
    else if(i == 25)
    {
        key = GetKeyFromColorValue('y','r');
    }
    else if(i == 26)
    {
        key = GetKeyFromColorValue('y','d');
    }
    // Green Action Cards
    else if(i == 38)
    {
        key = GetKeyFromColorValue('g','s');
    }
    else if(i == 39)
    {
        key = GetKeyFromColorValue('g','r');
    }
    else if(i == 40)
    {
        key = GetKeyFromColorValue('g','d');
    }
    // Blue Action Cards
    else if(i == 52)
    {
        key = GetKeyFromColorValue('b','s');
    }
    else if(i == 53)
    {
        key = GetKeyFromColorValue('b','r');
    }
    else if(i == 54)
    {
        key = GetKeyFromColorValue('b','d');
    }

    // Key for all valid cards is > 0
    if (key>0)
    {
        CardHashTable.Update(key,cutout);
    }
}

void Uno::MakeCardCutouts() 
{   
    std::string filename = "UNO_cards_deck.png";

    SimpleBitmap bmp;
    if(!bmp.LoadPng(filename.c_str()))
    {
        std::cout << "Error: Failed to read " << std::endl;
        return;
    }

    std::cout << "Successfully read PNG of dimension (wid, hei) = (" << bmp.GetWidth() << ", " << bmp.GetHeight() << ")\n";

    SimpleBitmap cutout;
    
    // FILE* fp;

    int px_x {0}, px_y {0};
    int tabCount=0;
    for(int i = 0; i < 70; ++i)
    {
        if(!bmp.IsXInRange(px_x))
        {
            px_x = 0;
            px_y += 120;
        }
        if(!bmp.IsInRange(px_x,px_y))
        {
            break;
        }

        cutout = bmp.CutOut(px_x, px_y, 80,120);
        InsertCutoutIntoTable(i, cutout);
        // fp = fopen((std::to_string(i)+".png").c_str(), "wb");
        // cutout.SavePng(fp);
        // fclose(fp);

        px_x+=80;
    }

    // Print the hash table size
    std::cout << "CardHashTable size: " << CardHashTable.GetN() << std::endl;
}

void Uno::InitializeDeck()
{
	for(char c : {'r','g','b','y','w'})
	{
		if(c == 'w')
		{
			for(char v : {'c', 'f'})
			{
				deck.emplace_back(std::make_unique<Card>(c,v));
				deck.emplace_back(std::make_unique<Card>(c,v));
				deck.emplace_back(std::make_unique<Card>(c,v));
				deck.emplace_back(std::make_unique<Card>(c,v));
			}
		}
		else
		{
			for(char v : {'0','1','2','3','4','5','6','7','8','9','s','r','d'})
			{
				if(v!='0')
				{
					deck.emplace_back(std::make_unique<Card>(c,v));	
					deck.emplace_back(std::make_unique<Card>(c,v));
				}
				else
					deck.emplace_back(std::make_unique<Card>(c,v));
			}
		}
	}
}

void Uno::ShuffleDeck()
{
	std::shuffle(deck.begin(), deck.end(), std::mt19937{std::random_device{}()});
}

void Uno::DrawCard(Player* aPlayer, int nCards)
{
    for(int i = 0; i < nCards; ++i)
    {
        aPlayer->hand.emplace_back(std::move(deck.back()));
        deck.pop_back();
        
        if(deck.size() < 1)
        {
            deck.emplace_back(std::move(discardPile.back()));
            discardPile.pop_back();
            deck.swap(discardPile);
        }
    }
}

void Uno::DealCards()
{	
	Player* dealingToPlayer = root;
	for(int i = 0; i < initialNumCardsDealt * num_players; ++i)
	{
		DrawCard(dealingToPlayer, 1);
		dealingToPlayer = dealingToPlayer->antiClockwise;
	}
}

void Uno::CommFaceUpCardToPlayer()
{
    curr_player->currGameColor = GetCurrentColor();
    curr_player->currGameValue = GetCurrentValue();
}

void Uno::InitDeckDiscardPile()
{
    InitializeDeck();
    ShuffleDeck();
    DealCards();
    discardPile.emplace_back(std::move(deck.back()));
    deck.pop_back();
    curr_color = discardPile.back()->GetColor();
    curr_value = discardPile.back()->GetValue();
}

void Uno::PrintFaceUpCard() const
{   

    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << "Faceup card is: "; 
    discardPile.back()->Print();
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

void Uno::SetNextPlayer()
{
    if (dir=='c')
    {
        curr_player = curr_player->clockwise;
    }
    else if (dir=='a')
    {
        curr_player = curr_player->antiClockwise;
    }
    else
    {
        std::cout << "ERROR: INVALID DIRECTION" << std::endl;
    }
}


void Uno::SetNextPlayerWithSkip()
{
    SetNextPlayer();
    SetNextPlayer();
}

void Uno::SetNextPlayerWithReverse()
{
    if (dir=='a')
    {
        dir ='c';
    }
    else if (dir=='c')
    {
        dir = 'a';
    }
    else 
    {
        std::cout << "ERROR: INVALID DIRECTION" << std::endl;
    }

    if (num_players==2) // Exception to the general rule of reverse direction
    {
        SetNextPlayerWithSkip();
    }
    else
    {
        SetNextPlayer();
    }
}

bool Uno::IsValid(int idx) const
{
    if(idx<0)
        return false;
    return curr_player->hand.at(idx)->IsCardValid(curr_color, curr_value);
}

void Uno::CheckIfPlayerWonGame()
{
	if(curr_player->hand.size() == 0)
	{
		printf("Player %c played their last card!\n", curr_player->GetPlayerName());
		printf("Player %c wins MooNO!\n", curr_player->GetPlayerName());
		printf("Thank you for playing MooNO by MooPointers\n");
		endOfGame = true;
	}
}

void Uno::TakeAction()
{
    // moving card from player hand to discard pile
    curr_player->SwapCards(idxPlayerCardToCheck);
    discardPile.emplace_back(std::move(curr_player->hand.back()));
    curr_player->hand.pop_back();
    CheckIfPlayerWonGame();
    
    if(endOfGame)
    {
    	return;
    }
    
    // setting game card value
    curr_value = discardPile.back()->GetValue();

    // setting game color
    if(discardPile.back()->GetColor() != 'w')
    {
        curr_color = discardPile.back()->GetColor();
    }
    else
    {
        if (curr_player != root)
        {
            /*If game level set to easy, bot shall play dumb and choose a random color when wild played*/
            if (gameLevel == 'e')
            {
                curr_player->chooseRandGameCol = true;
            }
            curr_color = curr_player->ChooseGameColor();
        }
        else
        {
        	GetUserColorChoice();
        }
    }

    // taking game action
    if(curr_value=='s') // Skip
    {
        SetNextPlayerWithSkip();
    }
    else if(curr_value=='r') // Reverse
    {
        SetNextPlayerWithReverse();
    }
    else if(curr_value=='d') // Draw2
    {
        SetNextPlayer();
        // Add 2 cards to the next player depending on the direction
        DrawCard(curr_player, 2);
    }
    else if(curr_value=='c') // Wild
    {
        SetNextPlayer();
    }
    else if(curr_value=='f') // Wild+Draw4
    {
        SetNextPlayer();
        // Add 4 cards to the next player depending on the direction
        DrawCard(curr_player, 4);
    }
    else // Numbered Card - Do Nothing
    {
        SetNextPlayer();
    }
}

void Uno::ShowGameState() const
{
    std::cout << "\nORDER OF PLAYERS(NUMBER OF CARDS IN HAND) " << std::endl;
    std::cout << std::string(num_players*8, '=') << std::endl;

    for(int i = 0; i < num_players; ++i)
    {
        std::cout << curr_player->GetPlayerName() << "(" << curr_player->GetNumCards() << ")" << " -> ";
        if(dir == 'c')
        {
            curr_player = curr_player->clockwise;
        }
        else
        {
            curr_player = curr_player->antiClockwise;
        }
    }
    std::cout << std::endl << std::string(num_players*8, '=') << std::endl;
}

void Uno::PlayerActionMessage(int n) const
{
    std::cout << "Player ";
    if(n == -1)
    {
        std::cout << curr_player->GetPlayerName() << " drew a card." << std::endl;
    }
    else if(n == -2)
    {
        std::cout << curr_player->GetPlayerName() << " passed turn." << std::endl;   
    }
    else
    {
        std::cout << curr_player->GetPlayerName() << " played card." << std::endl;
    }
    
}
void Uno::GetXYClickLoc(int x,int y) const
{
    mx = x;
    my = y;
}

void Uno::SetClickFlagTrue()
{
    clickOccured = true;
}
void Uno::Play()
{ 
    
    if (endOfGame) 
    {
        return;
    }

    

    CommFaceUpCardToPlayer();

    
    
    if(curr_player==root)
    {
        if(clickOccured)
        { 
            idxPlayerCardToCheck = GetClickedCardIndex();
            clickOccured = false;
        }
        else
        {
            idxPlayerCardToCheck = -3;
        }

    }
    else
    {	
        /*If level set to hard and next player is root, play aggressively but be kind to fellow bots*/
        if (gameLevel == 'h')
        {
            if ((dir == 'c' && curr_player->clockwise == root) ||
                (dir == 'a' && curr_player->antiClockwise == root))
            {
                curr_player->playAgg = true;
            }
            else
            {
                curr_player->playKind = true;
            }
        }
        if (gameLevel == 'e')
        {
            if ((dir == 'c' && curr_player->clockwise == root) ||
                (dir == 'a' && curr_player->antiClockwise == root))
            {
                curr_player->playKind = true;
            }
            else
            {
                curr_player->playAgg = true;
            }
        }

        idxPlayerCardToCheck = curr_player->PlayCard();   
    }
    
    if(idxPlayerCardToCheck == -1)      // player wants to draw a card
    {
        DrawCard(curr_player, 1);

        return;
    }
    else if(idxPlayerCardToCheck == -2) // player wants to pass turn
    {
        if (curr_player==root)
        {
            root->cardDrawn = false;
        }
        
        prev_player = curr_player;
        
        SetNextPlayer();
        
        RunGameCadence();
        return;
    }
    else if (IsValid(idxPlayerCardToCheck))
    {
        if (curr_player == root)
        {
            root->cardDrawn = false;
        }

        prev_player = curr_player;
       
        TakeAction();

        RunGameCadence();
    }
}


void Uno::CallMooNOOnBot()
{
    if (prev_player->GetNumCards() == 1)
    {
        caughtBot = true;
        DrawCard(prev_player, 2);
    }
}

void Uno::DisplayACard(char c, char v, double x, double y) const
{
    int key = GetKeyFromColorValue(c, v);
    SimpleBitmap card;
    card = *CardHashTable[key];
    card.Invert();
    auto bmpPtr = card.GetBitmapPointer();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glRasterPos2d(x, y);
    glDrawPixels(80,120,GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bmpPtr);
    glDisable(GL_BLEND);
}

void Uno::DisplayRootHand() const
{
	numCardsRoot = root->GetNumCards();	
	numRows = 1;
	numCardsPerRow = numCardsRoot / numRows;
	
	visibleCardWidth = ((double)handDisplayWidth - (double)cardWidth)/(double(numCardsPerRow)-1.);
	visibleCardWidth > cardWidth ? visibleCardWidth = cardWidth : visibleCardWidth;

	x0 = winWidth/2 - (visibleCardWidth*(numCardsPerRow-1)+cardWidth)/2;
	y0 = winHeight-1-30;	

	for(const auto &card : root->hand)
	{
		if(x0 > winWidth/2 + handDisplayWidth/2)
		{
			x0 = winWidth/2 - (visibleCardWidth*(numCardsPerRow-1)+cardWidth)/2;
			y0 -= cardHeight;
		}		
		DisplayACard(card->GetColor(), card->GetValue(), x0,y0);
		x0 += visibleCardWidth;
	}
    x0 = winWidth/2 - (visibleCardWidth*(numCardsPerRow-1)+cardWidth)/2;
}


int Uno::GetClickedCardIndex() const
{
	// process click on hand of cards
    if(my <= winHeight-1-30 && my >= winHeight-1-30 - cardHeight)
    {
        
        double distFromLeft = mx-x0; 

        int index = static_cast<int>(distFromLeft/visibleCardWidth);
        if(index >=numCardsRoot-1)
        {
            if(mx > x0+(numCardsRoot-1)*visibleCardWidth && mx < x0+(numCardsRoot-1)*visibleCardWidth + cardWidth)
            {
                return numCardsRoot-1;
            }
        }
        else if(mx >=x0)
        {
            return index;
        }
    }
    // process click on deck & draw
    else if (!root->cardDrawn && mx >= winWidth/2+cardWidth && mx <= winWidth/2+2*cardWidth && 
                my>= (winHeight-1)/2+cardHeight/2 - cardHeight && my<= (winHeight-1)/2+cardHeight/2)
    {
        root->cardDrawn = true;
        return -1;
    }
    // process click on deck & pass
    else if(mx >= winWidth/2+cardWidth && mx <= winWidth/2+2*cardWidth && 
                my>= (winHeight-1)/2+cardHeight/2 - cardHeight && my<= (winHeight-1)/2+cardHeight/2
                && root->cardDrawn)
    {
        root->cardDrawn = true;
        return -2;
    }
    return -3;
}

void Uno::DisplayDiscardPile() const
{
    DisplayACard(discardPile.back()->GetColor(), discardPile.back()->GetValue(), winWidth/2-2*cardWidth,(winHeight-1)/2+cardHeight/2);
}

void Uno::DisplayDeck() const
{
    int px = winWidth/2+cardWidth;
    int py = (winHeight-1)/2+cardHeight/2;
    
    auto bmpPtr = deckPng.GetBitmapPointer();


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glRasterPos2d(px, py);
    glDrawPixels(80,120,GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bmpPtr);
    glDisable(GL_BLEND);
}

void Uno::DisplayPassButton() const
{
    glColor3ub(255,255,0);
    int px = winWidth/2 + cardWidth;
    int py = (winHeight-1)/2+cardHeight/2;
    int dx = 80;
    int dy = 120;      
    
    glColor3f(255,255,255);
    glRasterPos2i(px,py-44);
    YsGlDrawFontBitmap20x32("PASS");

}

void Uno::DisplayQuitButton() const
{   
    glColor3f(255,255,255);
    glRasterPos2i(winWidth-100,50);
    YsGlDrawFontBitmap20x32("QUIT");
}

void Uno::DisplayNewGameButton() const
{   
    glColor3f(255,255,255);
    glRasterPos2i(winWidth-180,100);
    YsGlDrawFontBitmap20x32("NEW GAME");
}

void Uno::DisplayEndGameMsg() const
{
    glColor3f(255, 255, 255);
    glRasterPos2i((winWidth / 2) - 190, (winHeight / 2) - 100);
    std::string playerName(1, curr_player->GetPlayerName());
    std::string msg = "";
    if (playerName == "A")
    {
        msg = "You have won MooNO!\n";
    }
    else
    {
        msg = "Player " + playerName + " wins MooNO!\n";
    }
    YsGlDrawFontBitmap20x32(msg.c_str());
}

void Uno::DisplayBots() const
{
    auto bot = root->clockwise;
    double botOffsetX;

    for (int i=0; i<num_players-1; ++i)
    {
        std::string botName(1, bot->name);
        glColor3f(255, 255, 255);

        if (i == (num_players - 1) / 2 && (num_players - 1) % 2 != 0) {
            botOffsetX = 10;
        }
        else {
            botOffsetX = 35;
        }
        glRasterPos2d(bot->GetPosX() - botOffsetX, bot->GetPosY());

        YsGlDrawFontBitmap20x32((botName + ":" + std::to_string(bot->GetNumCards())).c_str());

        if (bot == curr_player) {
            // std::cout << "Curr player: " << i << std::endl;
            glBegin(GL_LINE_STRIP);
            glColor3f(255, 255, 255);
            glVertex2d(bot->GetPosX() - botOffsetX - 22, bot->GetPosY() + 110 - 100);
            glVertex2d(bot->GetPosX() - botOffsetX - 22 + 100, bot->GetPosY() + 110 - 100);
            glVertex2d(bot->GetPosX() - botOffsetX - 22 + 100, bot->GetPosY() + 110);
            glVertex2d(bot->GetPosX() - botOffsetX - 22, bot->GetPosY() + 110);
            glVertex2d(bot->GetPosX() - botOffsetX - 22, bot->GetPosY() + 110 - 100);
            glEnd();
        }

        auto bmpPtr = botPng.GetBitmapPointer();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glRasterPos2d(bot->GetPosX() - botOffsetX - 22, bot->GetPosY() + 110);
        glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bmpPtr);
        glDisable(GL_BLEND);

        if (caughtBot && bot == prev_player)
        {
            auto bmpPtrMooNOBot = mooNOBotDraw2Png.GetBitmapPointer();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glRasterPos2d(bot->GetPosX() - botOffsetX - 22, bot->GetPosY() + 110);
            glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bmpPtrMooNOBot);
            glDisable(GL_BLEND);
        }

        if (botYellsUno && bot == prev_player)
        {
            auto bmpPtrMooNOBot = mooNOBotPng.GetBitmapPointer();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glRasterPos2d(bot->GetPosX() - botOffsetX - 22, bot->GetPosY() + 110);
            glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bmpPtrMooNOBot);
            glDisable(GL_BLEND);
        }
        

        bot = bot->clockwise;
    }
}

void Uno::DisplayMooNOButton() const
{
    auto bmpPtr = mooNOPng.GetBitmapPointer();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glRasterPos2d(winWidth / 2 - 40, winHeight / 2 + 180);
    glDrawPixels(mooNOButtonWidth, mooNOButtonHeight, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)bmpPtr);
    glDisable(GL_BLEND);
}

bool Uno::MooNOButtonClicked() const {
    if (mx > winWidth / 2 - 40 && mx < winWidth / 2 - 40 + mooNOButtonWidth) {
        if (my > winHeight / 2 + 180 - mooNOButtonHeight && my < winHeight / 2 + 180 ) {
            auto bmpPxlPtr = mooNOPng.GetPixelPointer(mx - (winWidth / 2 - 40), winHeight / 2 + 180 - my);
            if (bmpPxlPtr[3] > 0) {
                return true;
            }
        }
    }
    return false;
}

void Uno::DrawGameState() const
{	
	if(discardPile.back()->GetColor() == 'w')
    {
        DrawCurrentColorBar();
    }

    if(curr_player==root && HasGameEnded()==false)
    {
        HighlightRootCards();
    }

    if (endOfGame)
    {
        DisplayEndGameMsg();
        DisplayNewGameButton();
    }
    DisplayNewGameButton();
	DisplayRootHand();
    DisplayDiscardPile();
    DisplayDeck();
    DisplayBots();
    
    DisplayMooNOButton();
    DisplayQuitButton();

    if(root->cardDrawn)
    {
        DisplayPassButton();
    }
}

void Uno::DrawCurrentColorBar() const
{
	
	int px = x0 - 10;
	int py = winHeight-1-20;

	// begin alpha blending
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// set color based on curr_color and alpha = 100
	if(curr_color =='r')
		glColor4ub(255,0,0,125);
	else if(curr_color == 'g')
		glColor4ub(0,255,0,125);
	else if(curr_color == 'b')
		glColor4ub(0,0,255,200);
	else if(curr_color == 'y')
		glColor4ub(255,255,0,125);
	
	glBegin(GL_QUADS);
	glVertex2i(px,py);
	glVertex2i(px+visibleCardWidth*(numCardsRoot-1)+cardWidth+20, py);
	glVertex2i(px+visibleCardWidth*(numCardsRoot-1)+cardWidth+20, py - cardHeight-20);
	glVertex2i(px, py - cardHeight-20);
	glEnd();

	glDisable(GL_BLEND);

}

void Uno::HighlightRootCards() const
{
    int px = x0 - 10;
	int py = winHeight-1-20;

    glBegin(GL_LINE_STRIP);
    glColor3f(255, 255, 255);
    glVertex2d(px,py);
    glVertex2d(px+visibleCardWidth*(numCardsRoot-1)+cardWidth+20, py);
    glVertex2d(px+visibleCardWidth*(numCardsRoot-1)+cardWidth+20, py - cardHeight-20);
    glVertex2d(px, py - cardHeight-20);
    glVertex2d(px,py);
    glEnd();

}

void Uno::DisplayBackground() const
{
	glRasterPos2i(0, winHeight -1);
	glDrawPixels(winWidth,winHeight,GL_RGBA, GL_UNSIGNED_BYTE, background.rgba);
}

void Uno::DrawColorChoicePalette() const
{	
	int px = winWidth/2-cardHeight/2;
	int py = winHeight/2-1+cardHeight/2;

	// white palette background
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(255,255,255,100);
	glBegin(GL_QUADS);
	glVertex2i(px, py);
	glVertex2i(px + cardHeight, py);
	glVertex2i(px + cardHeight, py - cardHeight);
	glVertex2i(px, py - cardHeight);
	glEnd();
	glDisable(GL_BLEND);
	
	// red 
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(255,0,0,125);
	glBegin(GL_QUADS);
	glVertex2i(px, py);
	glVertex2i(px + cardHeight/2, py);
	glVertex2i(px + cardHeight/2, py - cardHeight/2);
	glVertex2i(px, py - cardHeight/2);
	glEnd();
	glDisable(GL_BLEND);

	// green
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(0,255,0,125);
	glBegin(GL_QUADS);
	glVertex2i(px+cardHeight/2, py);
	glVertex2i(px + cardHeight, py);
	glVertex2i(px + cardHeight, py - cardHeight/2);
	glVertex2i(px+cardHeight/2, py - cardHeight/2);
	glEnd();
	glDisable(GL_BLEND);
	
	// blue
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(0,0,255,200);
	glBegin(GL_QUADS);
	glVertex2i(px, py-cardHeight/2);
	glVertex2i(px + cardHeight/2, py-cardHeight/2);
	glVertex2i(px + cardHeight/2, py - cardHeight);
	glVertex2i(px, py - cardHeight);
	glEnd();
	glDisable(GL_BLEND);

	// yellow
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(255,255,0,125);
	glBegin(GL_QUADS);
	glVertex2i(px+cardHeight/2, py-cardHeight/2);
	glVertex2i(px + cardHeight, py-cardHeight/2);
	glVertex2i(px + cardHeight, py - cardHeight);
	glVertex2i(px+cardHeight/2, py - cardHeight);
	glEnd();
	glDisable(GL_BLEND);
}

void Uno::GetUserColorChoice()
{
	bool correctLocClicked = false;
	int mx,my,lb,mb,rb, evt;
	int px = winWidth/2-cardHeight/2;
	int py = winHeight/2-1+cardHeight/2;

	while(correctLocClicked == false)
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
		DisplayBackground();
    	DrawGameState();
		DrawColorChoicePalette();
		
		FsPollDevice();
		auto key=FsInkey();
		evt=FsGetMouseEvent(lb,mb,rb,mx,my);

		if(evt==FSMOUSEEVENT_LBUTTONDOWN)
		{
			evt=FsGetMouseEvent(lb,mb,rb,mx,my);
			if(mx > px && mx < px + cardHeight/2 && my > py - cardHeight/2 && my < py)
			{
				curr_color = 'r';
				correctLocClicked = true;
				break;
			}
			else if(mx > px + cardHeight/2 && mx < px+cardHeight + cardHeight/2 && my > py - cardHeight/2 && my < py)
			{
				curr_color = 'g';
				correctLocClicked = true;
				break;
			}				
			else if(mx > px && mx < px + cardHeight/2 && my > py - cardHeight && my < py-cardHeight/2)
			{
				curr_color = 'b';
				correctLocClicked = true;
				break;
			}
			else if(mx > px+cardHeight/2 && mx < px + cardHeight && my > py - cardHeight && my < py-cardHeight/2)
			{
				curr_color = 'y';
				correctLocClicked = true;
				break;
			}
		}

		FsSwapBuffers();
		FsSleep(10);

	}
}


void Uno::RunGameCadence()
{
    time_t t0 = time(NULL);
    int lb, mb, rb, evt;
    time_t tLat;
    caughtBot = false;
    botYellsUno = false;
    time_t tBotUno = 1;
    if (curr_player == root)
    {
        tLat = 2;
    }
    else
    {
        tLat = 3;
        
    }
    while (time(NULL)-t0 < tLat)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        DisplayBackground();
        DrawGameState();

        FsPollDevice();
        auto key = FsInkey();
        evt = FsGetMouseEvent(lb, mb, rb, mx, my);

        if (evt == FSMOUSEEVENT_LBUTTONDOWN && MooNOButtonClicked() && !botYellsUno)
        {
            CallMooNOOnBot();
            /*break;*/
        }

        if (time(NULL) - t0 >= tBotUno)
        {
            if (prev_player->GetNumCards() == 1)
            {
                botYellsUno = true;
                caughtBot = false;
            }
            
        }

        FsSwapBuffers();
        FsSleep(10);
    }
}