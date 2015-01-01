//============================================================================
// Name        : Saffron.cpp
// Author      : Daniel Saunders
// Description : Top level runner of Saffron
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include "src/SafRunner.h"
#include <TROOT.h>

int main(int argc, char *argv[])
{
  gROOT->ProcessLine("gErrorIgnoreLevel = kFatal;");
  SafRunner runner;
	runner.safPrint("Running Saffron", 0);
	runner.run();
	return 0;
}
