#include <iostream>
#include <SDL.h>
#include <cmath>
#include "utils.h"
#include "configuration.h"
#include "constants.h"
#include "board.h"
#include "render.h"
#include "raumschach.h"
#include "player.h"

int main(int argc, char **argv){

	Player * white = new Player;
	Player * black = new Player;

	Raumschach rchess;

	rchess.Initialize(white, black);
	rchess.Start();

	delete white;
	delete black;

	return 0;
}