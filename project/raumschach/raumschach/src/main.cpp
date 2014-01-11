#include "raumschach.h"
#include "player.h"

//debug
#include "charstring.h"
#include <stdio.h>
#include "button.h"

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