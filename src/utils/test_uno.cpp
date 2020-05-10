#include <iostream>
#include "Uno.h"
#include <string>

int main()
{
    int n=20;
    std::string dir = "cw";

    if (n==1)
    {
        std::cout << "Please get more friends" << std::endl;
        return 0;
    }

    // Create the game and initialize decks
    Uno test_game(n,dir);
    test_game.initializePlayers();
    test_game.initDeckDiscardPile();
    test_game.testDeckDiscardPileInitialization();
    


    auto root = test_game.getRoot();
    std::cout << "Root is: " << root->getName() << std::endl;
    auto curr_player = test_game.getCurrentPlayer();
    std::cout << "Curr player is: " << curr_player->getName() << std::endl;
    // test_game.printFaceUpCard();


    int counter=0;
    while (counter<40)
    {
        if (counter==3) // Skip condition
        {
            test_game.setNextPlayerWithSkip(curr_player, dir);
            std::cout << "Counter: " << counter << " Dir: " << dir << " Curr player: " << curr_player->getName() << std::endl;
            counter++;
            continue;
        }
        if (counter==6) // Reverse condition
        {
            test_game.setNextPlayerWithReverse(curr_player, dir, n);
            // test_game.setNextPlayer(curr_player, dir);
            std::cout << "Counter: " << counter << " Dir: " << dir <<" Curr player: " << curr_player->getName() << std::endl;
            counter++;
            continue;

        }
        test_game.setNextPlayer(curr_player, dir);
        std::cout << "Counter: " << counter << " Dir: " << dir <<" Curr player: " << curr_player->getName() << std::endl;
        counter++;
    }



    test_game.play();



    return 0;

}