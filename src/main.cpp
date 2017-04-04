#include "ProgramContext.h"

int main(int argc, char * argv[])
{
	ProgramContext programContext;
	programContext.init();
	programContext.run();
	while (1);
}