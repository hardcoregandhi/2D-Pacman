#include "Pacman.h"
#include <iostream>
using namespace std;

//Entry Point for Application

int main(int argc, char* argv[]) {

	int munchieCount = 5;
	int ghostCount = 5;
	

	Pacman* game = new Pacman(argc, argv, munchieCount, ghostCount);

	
	
}