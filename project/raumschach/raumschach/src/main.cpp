#include "raumschach.h"
#include "player.h"

//debug
#include "charstring.h"
#include <stdio.h>
#include "button.h"

// Project total lines - 3733 - commit - 8b845894 - 12.01.14 - 01:12:15

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