/*
 * SafCoincidenceFinder.cpp
 *
 *  Created on: Jan 13, 2014
 *      Author: Daniel Saunders
 */

#include "SafCoincidenceFinder.h"


//_____________________________________________________________________________

SafCoincidenceFinder::SafCoincidenceFinder(SafRunner * runner) :
  SafAlgorithm(runner, "SafCoincidenceFinder")
{
}


//_____________________________________________________________________________

SafCoincidenceFinder::~SafCoincidenceFinder()
{
}


//_____________________________________________________________________________

void SafCoincidenceFinder::initialize()
{
}


//_____________________________________________________________________________

void SafCoincidenceFinder::threadExecute(unsigned int iGlib, unsigned int iChannelLow,
	unsigned int iChannelUp)
{
	for (unsigned int i=iChannelLow; i<std::min(iChannelUp,
		runner()->geometry()->nChannels()); i++) {

	}
}



//_____________________________________________________________________________

void SafCoincidenceFinder::execute()
{
	for (unsigned int i=0; i<runner()->geometry()->nGlibs(); i++){
		threadExecute(i, 0, runner()->geometry()->nChannels());
	}
}



//_____________________________________________________________________________

void SafCoincidenceFinder::finalize()
{
}


//_____________________________________________________________________________

