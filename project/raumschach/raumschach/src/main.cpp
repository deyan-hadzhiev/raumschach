#include <SDL.h>
#include "raumschach.h"
#include "configuration.h"

// Project total lines - 3733 - commit - 8b845894 - 12.01.14 - 01:12:15

int main(int argc, char **argv){

	Raumschach rchess;

	rchess.Initialize();
	rchess.Start();

	return 0;
}