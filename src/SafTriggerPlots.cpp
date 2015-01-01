/*
 * SafTriggerPlots.cpp
 *
 *  Created on: Dec 30, 2014
 *      Author: Daniel Saunders
 */

#include "SafTriggerPlots.h"


//_____________________________________________________________________________

SafTriggerPlots::SafTriggerPlots(SafRunner * runner) :
  SafAlgorithm(runner, "SafTriggerPlots")
{
	// TODO Auto-generated constructor stub

}


//_____________________________________________________________________________

SafTriggerPlots::~SafTriggerPlots()
{
	// TODO Auto-generated destructor stub
}


//_____________________________________________________________________________

void SafTriggerPlots::initialize()
{
	unsigned int nG = runner()->geometry()->nGlibs();
	unsigned int nC = runner()->geometry()->nChannels();


	for (unsigned int i=0; i<nG; i++) {
		std::stringstream ssGlib; ssGlib<<i;
		for (unsigned int j=0; j<nC; j++) {
			std::stringstream ssChan; ssChan<<j;

			// First set peaks.
			std::string name = "FirstEventPeaks" + ssChan.str() + "-" + ssGlib.str();
			TH1F * plot = new TH1F(name.c_str(), name.c_str(),
					runner()->eventTimeWindow(), 0.0, runner()->eventTimeWindow());
			h_firstEventPeaks.push_back(plot);

			// Trigger Value dists.
			name = "TriggerValues" + ssChan.str() + "-" + ssGlib.str();
			h_triggerValues.push_back(new TH1F(name.c_str(), name.c_str(), 500, 100, 10000));
		}
	}


	// Root file directories.
	TDirectory * instance_direc = runner()->saveFile()->mkdir(name().c_str());
	instance_direc->mkdir("FirstPeaks");
	for (unsigned int i=0; i<nG; i++) {
		std::stringstream ssGlib; ssGlib<<i;
		instance_direc->mkdir(("FirstPeaks/Glib" + ssGlib.str()).c_str());
	}

	instance_direc->mkdir("TriggerValues");
	for (unsigned int i=0; i<nG; i++) {
		std::stringstream ssGlib; ssGlib<<i;
		instance_direc->mkdir(("TriggerValues/Glib" + ssGlib.str()).c_str());
	}
}


//_____________________________________________________________________________

void SafTriggerPlots::execute()
{
	fill();
}


//_____________________________________________________________________________

void SafTriggerPlots::fill()
{
	unsigned int nGlibs = runner()->geometry()->nGlibs();
	unsigned int nChannels = runner()->geometry()->nChannels();
	SafRawDataChannel * channel;
	unsigned int plotIndex;

	for (unsigned int i=0; i<nGlibs; i++) {
		for (unsigned int j=0; j<nChannels; j++) {
			channel = runner()->rawData()->channel(i, j);
			plotIndex = i*nChannels + j;

			for (unsigned int k=0; k<channel->nTriggers(); k++) {
				// Peaks.
				if (runner()->event() == 0) {
					h_firstEventPeaks[plotIndex]->Fill(
							channel->triggerTimes()->at(k), channel->triggerValues()->at(k));
				}

				// Trigger value dists.
			  h_triggerValues[plotIndex]->Fill(channel->triggerValues()->at(k));
			}
		}
	}
}


//_____________________________________________________________________________

void SafTriggerPlots::finalize()
{
	for (unsigned int i=0; i<h_firstEventPeaks.size(); i++) {
		int iGlib = i/76;
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/FirstPeaks/Glib" + ssGlib.str()).c_str());

		h_firstEventPeaks[i]->Write();
	}

	for (unsigned int i=0; i<h_triggerValues.size(); i++) {
		int iGlib = i/76;
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/TriggerValues/Glib" + ssGlib.str()).c_str());

		h_triggerValues[i]->Write();
	}
}


//_____________________________________________________________________________

