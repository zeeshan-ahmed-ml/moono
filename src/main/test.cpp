#include "Uno.h"
#include <fstream>
int main()
{
	// std::ofstream file;
	// file.open("ofile.txt",std::ios::app);
	// file << "Hello, World!" << std::endl;
	// file.close();
	Uno game(5);

	for(;;)
	{
		game.Play();
	}

	return 0;
}
