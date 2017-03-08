#include "glut.h"
#include "GameController.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

const string assetDirectory = "/Users/vamsi/Desktop/Games/Boulder_Blast/DerivedData/BoulderBlast/Build/Products/Debug/Assets";

class GameWorld;

GameWorld* createStudentWorld(string assetDir = "");

int main(int argc, char* argv[])
{
	{
		string path = assetDirectory;
		if (!path.empty())
			path += '/';
		ifstream ifs(path + "level00.dat");
		if (!ifs)
		{
			cout << "Cannot find level00.dat in ";
			cout << (assetDirectory.empty() ? "current directory"
											: assetDirectory) << endl;
			return 1;
		}
	}

    glutInit(&argc, argv);

    srand(static_cast<unsigned int>(time(nullptr)));

    GameWorld* gw = createStudentWorld(assetDirectory);
    Game().run(gw, "Boulder Blast");
}
