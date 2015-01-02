/*
 * SafTrigger.cpp
 *
 *  Created on: Dec 22, 2014
 *      Author: Daniel Saunders
 */

#include "SafTrigger.h"


//_____________________________________________________________________________

SafTrigger::SafTrigger(SafRunner * runner) :
  SafAlgorithm(runner, "SafTrigger"),
  m_triggerWindowSizeA(16),
  m_triggerWindowSizeB(8),
  m_triggerWindowSizeC(8),
  m_triggerValueCut(60),
  m_nThreadsPerGlib(2),
  m_threading(true)
{
	m_triggerWindowSizeTotal = m_triggerWindowSizeA + m_triggerWindowSizeB
			+ m_triggerWindowSizeC;
}


//_____________________________________________________________________________

SafTrigger::~SafTrigger()
{
}


//_____________________________________________________________________________

void SafTrigger::initialize()
{
}


//_____________________________________________________________________________

void SafTrigger::scanChannels(unsigned int iGlib, unsigned int iLow, unsigned int iUp)
{
	unsigned int nChannels = runner()->geometry()->nChannels();
	for (unsigned int i=iLow; i<std::min(iUp, runner()->geometry()->nChannels()); i++) {
		scanChannel(runner()->rawData()->channel(iGlib, i));
	}
}


//_____________________________________________________________________________

void SafTrigger::scanGlib(unsigned int iGlib)
{
	unsigned int nChannels = runner()->geometry()->nChannels();
	int step = nChannels/m_nThreadsPerGlib;
	std::vector<std::thread> threads;
	for (unsigned int i=0; i<nChannels; i+=step) {
		threads.push_back(std::thread(&SafTrigger::scanChannels, this, iGlib, i, i+step));
	}
	
	for (std::vector<std::thread>::iterator i = threads.begin(); 
			i!=threads.end(); i++) {
		(*i).join();
	}
}


//_____________________________________________________________________________

void SafTrigger::execute()
{
	unsigned int nGlibs = runner()->geometry()->nGlibs();
	unsigned int nChannels = runner()->geometry()->nChannels();
	
	if (m_threading) {
		std::vector<std::thread> threads;
		for (unsigned int i=0; i<nGlibs; i++) {
			threads.push_back(std::thread(&SafTrigger::scanGlib, this, i));
		}
		
		for (std::vector<std::thread>::iterator i = threads.begin(); 
				i!=threads.end(); i++) {
			(*i).join();
		}
	}
	
	else {
		for (unsigned int i=0; i<nGlibs; i++) {
			for (unsigned int j=0; j<nChannels; j++) {
				scanChannel(runner()->rawData()->channel(i, j));
			}
		}
	}
}


//_____________________________________________________________________________

void SafTrigger::scanChannel(SafRawDataChannel * channel)
{
	unsigned int nTriggers = 0;
	std::vector<int> * times = channel->times();
	std::vector<double> * signals = channel->signals();
	if (times->size() <= m_triggerWindowSizeTotal) return;


	for (unsigned int i=0; i<times->size() - m_triggerWindowSizeTotal; i++) {
		double triggerValue = evalTimeWindow(signals, times, i);
		if (triggerValue > m_triggerValueCut) {
			channel->triggerTimes()->push_back(times->at(i));
			channel->triggerValues()->push_back(triggerValue);
			nTriggers++;
		}
		//plot(triggerValue, "AllTriggerValues", "AllTriggerValues", 500, -1000, 10000);
	}

	channel->setNTriggers(channel->triggerTimes()->size());
}


//_____________________________________________________________________________

double SafTrigger::evalTimeWindow(std::vector<double> * signals,
		std::vector<int> * times, unsigned int iStart)
{
	double value;
	double baseLineSum = 0.0;
	double peakSum = 0.0;
	double dipSum = 0.0;

	unsigned int nBaseLinePresent = 0;
	unsigned int nPeakPresent = 0;
	unsigned int nDipPresent = 0;

	unsigned int i;
	int currentTime = times->at(iStart);

	for (i=0; i<m_triggerWindowSizeA; i++) {
    if (currentTime == times->at(iStart+i)) {
    	nBaseLinePresent++;
    	baseLineSum += signals->at(iStart+i);
    }
    currentTime++;
	}
	baseLineSum /= (1.0*nBaseLinePresent);


	for (i=0; i<m_triggerWindowSizeB; i++) {
		int iWindow = iStart+i+m_triggerWindowSizeA;
    if (currentTime == times->at(iWindow)) {
    	nPeakPresent++;
    	peakSum += signals->at(iWindow);
    }
    currentTime++;
	}
	peakSum /= (1.0*nPeakPresent);


	for (i=0; i<m_triggerWindowSizeC; i++) {
		int iWindow = iStart+i+m_triggerWindowSizeA + m_triggerWindowSizeB;
    if (currentTime == times->at(iWindow)) {
    	nDipPresent++;
    	dipSum += signals->at(iWindow);
    }
    currentTime++;
	}
	dipSum /= (1.0*nDipPresent);

	value = (peakSum - baseLineSum) - (dipSum - baseLineSum);
	//std::cout<<value<<std::endl;
	return value;
}



//_____________________________________________________________________________

void SafTrigger::finalize()
{
}


//_____________________________________________________________________________

