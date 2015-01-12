/*
 * SafFilter.cpp
 *
 *  Created on: Jan 9, 2014
 *      Author: Daniel Saunders
 */

#include "SafFilter.h"


//_____________________________________________________________________________

SafFilter::SafFilter(SafRunner * runner) :
  SafAlgorithm(runner, "SafFilter"),
  m_filterSize(8)
{
}


//_____________________________________________________________________________

SafFilter::~SafFilter()
{
}


//_____________________________________________________________________________

void SafFilter::initialize()
{
  m_threading = false;
}


//_____________________________________________________________________________

void SafFilter::threadExecute(unsigned int iGlib, unsigned int iChannelLow, 
  unsigned int iChannelUp, int iThread)
{
  for (unsigned int i=iChannelLow; i<std::min(iChannelUp, 
    runner()->geometry()->nChannels()); i++) {
    filterChannel(runner()->rawData()->channel(iGlib, i));
  }
}



//_____________________________________________________________________________

void SafFilter::execute()
{
  for (unsigned int i=0; i<runner()->geometry()->nGlibs(); i++){
    threadExecute(i, 0, runner()->geometry()->nChannels(), -1);
  }
}



//_____________________________________________________________________________

void SafFilter::finalize()
{
}


//_____________________________________________________________________________

void SafFilter::filterChannel(SafRawDataChannel * channel)
{
  std::vector<double> * signals = channel->signals();
  double cache = 0;
  bool firstTime = true;
  if (signals->size() <= m_filterSize) return;

  for (unsigned int i=0; i<signals->size() - m_filterSize; i++) {
    if (firstTime) {
      cache = 0.0;
      for (unsigned int j=0; j<m_filterSize; j++) {
        cache += signals->at(i+j);
      }
      firstTime = false;
    }
    else {
      cache -= signals->at(i-1);
      cache += signals->at(i+m_filterSize-1);
    }
    signals->at(i+m_filterSize/2) = cache/(1.*m_filterSize);
  }
}


//_____________________________________________________________________________

