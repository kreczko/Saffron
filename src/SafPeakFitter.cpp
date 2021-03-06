/*
 * SafPeakFitter.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: Daniel Saunders
 */

#include "SafPeakFitter.h"


//_____________________________________________________________________________

SafPeakFitter::SafPeakFitter(SafRunner * runner) :
  SafAlgorithm(runner, "SafPeakFitter")
{
}


//_____________________________________________________________________________

SafPeakFitter::~SafPeakFitter()
{
}


//_____________________________________________________________________________

void SafPeakFitter::initialize()
{
}


//_____________________________________________________________________________

void SafPeakFitter::threadExecute(unsigned int iGlib, unsigned int iChannelLow, 
	unsigned int iChannelUp, int iThread)
{
	for (unsigned int i=iChannelLow; i<std::min(iChannelUp, 
		runner()->geometry()->nChannels()); i++) {
		//fitPeaks(runner()->rawData()->channel(iGlib, i));
	}
}



//_____________________________________________________________________________

void SafPeakFitter::execute()
{
	for (unsigned int i=0; i<runner()->geometry()->nGlibs(); i++){
		threadExecute(i, 0, runner()->geometry()->nChannels(), -1);
	}
}



//_____________________________________________________________________________

void SafPeakFitter::finalize()
{
}


//_____________________________________________________________________________

