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
  m_triggerValueCut(350),
  m_nThreadsPerGlib(4),
  m_threading(true),
  m_nTriggers(0),
  m_caching(false)
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
	for (unsigned int i=0; i<nGlibs; i++) {
	  for (unsigned int j=0; j<nChannels; j++) {
			m_nTriggers += runner()->rawData()->channel(i, j)->nTriggers();
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

	
	bool firstTimeEval = true;
	for (unsigned int i=0; i<times->size() - m_triggerWindowSizeTotal; i++) {
		// Temporary variables used to retrieve info from eval method. Also used 
		// for caching.
		double triggerValue;
		double triggerDipValue;
		double triggerPeakValue;
		double triggerBaseLine;
		
		evalTimeWindow(signals, times, i, &triggerValue, &triggerDipValue, 
				&triggerPeakValue, &triggerBaseLine, &firstTimeEval);
		
		if (triggerValue > m_triggerValueCut) {
			channel->triggerTimes()->push_back(times->at(i));
			channel->triggerValues()->push_back(triggerValue);
			channel->triggerDipValues()->push_back(triggerDipValue);
			channel->triggerPeakValues()->push_back(triggerPeakValue);
			channel->triggerBaseLines()->push_back(triggerBaseLine);
			nTriggers++;
			i += m_triggerWindowSizeB;
		}
	}
	

	channel->setNTriggers(channel->triggerTimes()->size());
}


//_____________________________________________________________________________

void SafTrigger::evalTimeWindow(std::vector<double> * signals,
		std::vector<int> * times, unsigned int iStart, double * triggerValue, 
		double * triggerDipValue, double * triggerPeakValue, double * triggerBaseLine,
		bool * firstTimeEval)
{
	// Cannot handle zero surpression in this version.
	// First eval case for this channel, and/or no caching cases.
	double baseLine = 0.0;
	double peakBit = 0.0;
	double dipBit = 0.0;
	
	
	if (*firstTimeEval || !m_caching) {
		unsigned int i;
		for (i = iStart; i<iStart + m_triggerWindowSizeA; i++)
			baseLine += signals->at(i);
		baseLine /= (1.*m_triggerWindowSizeA);
	
	
		for (; i<iStart + m_triggerWindowSizeA + m_triggerWindowSizeB; i++)
			peakBit += signals->at(i) - baseLine;
		peakBit /= (1.*m_triggerWindowSizeB);
	
	
		for (; i<iStart + m_triggerWindowSizeTotal; i++)
			dipBit += signals->at(i) - baseLine;
		dipBit /= (1.*m_triggerWindowSizeC);
		
		
		(*triggerValue) = peakBit - dipBit;;
		(*triggerDipValue) = dipBit;
		(*triggerPeakValue) = peakBit;
		(*triggerBaseLine) = baseLine;
		(*firstTimeEval) = false;
	}
	
	
	// Otherwise, fancy stuff with caching.
	else {
		// Broken.
		unsigned int triggerWindowSizeAB = m_triggerWindowSizeA + m_triggerWindowSizeB;
		(*triggerPeakValue) += (*triggerBaseLine);
		(*triggerDipValue) += (*triggerBaseLine);
		
		(*triggerPeakValue) *= (1.*m_triggerWindowSizeB);
		(*triggerDipValue) *= (1.*m_triggerWindowSizeC);
		
		
		(*triggerBaseLine) -= signals->at(iStart - 1)/(1.*m_triggerWindowSizeA);
		(*triggerBaseLine) += signals->at(iStart + m_triggerWindowSizeA - 1)/(1.*m_triggerWindowSizeA);
		
		(*triggerPeakValue) -= signals->at(iStart + m_triggerWindowSizeA - 1);
		(*triggerPeakValue) += signals->at(iStart + triggerWindowSizeAB - 1);
		
		(*triggerDipValue) -= signals->at(iStart + triggerWindowSizeAB - 1);
		(*triggerDipValue) += signals->at(iStart + m_triggerWindowSizeTotal - 1);
		
		(*triggerPeakValue) -= (*triggerBaseLine);
		(*triggerDipValue) -= (*triggerBaseLine);
		
		(*triggerPeakValue) /= (1.*m_triggerWindowSizeB);
		(*triggerDipValue) /= (1.*m_triggerWindowSizeC);
		
		(*triggerValue) = (*triggerPeakValue) - (*triggerDipValue);
	}
}



//_____________________________________________________________________________

void SafTrigger::finalize()
{
	std::cout<<"nTriggers:\t"<<m_nTriggers<<std::endl;
}


//_____________________________________________________________________________

