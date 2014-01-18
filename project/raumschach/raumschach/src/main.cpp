#include "raumschach.h"

//debug
#include "charstring.h"
#include <stdio.h>
#include "utils.h"
#include "random_generator.h"
#include "board.h"

// Project total lines - 3733 - commit - 8b845894 - 12.01.14 - 01:12:15

int main(int argc, char **argv){

	Raumschach rchess;

	rchess.Initialize();
	rchess.Start();

	return 0;
}