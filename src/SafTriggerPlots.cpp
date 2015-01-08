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
	
	h_allTriggerValues = new TH1F("TriggerValues", "TriggerValues", 500, 0, 6000);
	h_dipValues = new TH1F("DipValues", "DipValues", 500, -1000, 2500);
	h_peakValues = new TH1F("PeakValues", "PeakValues", 500, -1000, 2500);
	h_dipVsPeakValues = new TH2F("PeakVsDipValues", "PeakVsDipValues", 500, -1000, 
			2500, 500, -1000, 2500);
	h_dipVsTriggerValues = new TH2F("DipVsTriggerValues", "DipVsTriggerValues", 500, -1000, 
			2500, 500, -1000, 2500);
	h_peakVsTriggerValues = new TH2F("PeakVsTriggerValues", "PeakVsTriggerValues", 500, -1000, 
			2500, 500, -1000, 2500);
	
  h_firstEventPeaks = initPerChannelPlots("FirstEventPeaks", "FirstEventPeaks", 
		runner()->eventTimeWindow(), 0.0, runner()->eventTimeWindow());
	h_triggerValues = initPerChannelPlots("TriggerValues", "TriggerValues", 500, 100, 2500);
	
	h_nTriggers = new TH1F("AverageTriggerRate", "AverageTriggerRate", nC*nG, 
			0, nC*nG);

	int nChannels =  nC*nG; 
	h_allTriggers = new TH2F("AllTriggerDist", "AllTriggerDist", nChannels, -0.5, nChannels-0.5, 500, 100, 2500);


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
				// Global plots.
				h_allTriggerValues->Fill(channel->triggerValues()->at(k));
				h_dipValues->Fill(channel->triggerDipValues()->at(k));
				h_peakValues->Fill(channel->triggerPeakValues()->at(k));
				h_dipVsPeakValues->Fill(channel->triggerDipValues()->at(k), 
						channel->triggerPeakValues()->at(k));
				h_dipVsTriggerValues->Fill(channel->triggerDipValues()->at(k), 
						channel->triggerValues()->at(k));
				h_peakVsTriggerValues->Fill(channel->triggerPeakValues()->at(k), 
						channel->triggerValues()->at(k));
				
				
				// Peaks.
				if (runner()->event() == 0) {
					h_firstEventPeaks->at(plotIndex)->Fill(
							channel->triggerTimes()->at(k), channel->triggerValues()->at(k));
				}

				// Trigger value dists.
			  h_triggerValues->at(plotIndex)->Fill(channel->triggerValues()->at(k));
			}
		}
	}
}


//_____________________________________________________________________________

void SafTriggerPlots::finalize()
{
	for (unsigned int i=0; i<h_firstEventPeaks->size(); i++) {
		int iGlib = i/runner()->geometry()->nChannels();
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/FirstPeaks/Glib" + ssGlib.str()).c_str());

		h_firstEventPeaks->at(i)->Write();
	}

	for (unsigned int i=0; i<h_triggerValues->size(); i++) {
		int iGlib = i/runner()->geometry()->nChannels();
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/TriggerValues/Glib" + ssGlib.str()).c_str());

		h_triggerValues->at(i)->Write();
	}
	
	unsigned int nGlibs = runner()->geometry()->nGlibs();
	unsigned int nChannels = runner()->geometry()->nChannels();
	
	for (unsigned int i=0; i<nGlibs; i++) {
		for (unsigned int j=0; j<nChannels; j++) {
			SafRawDataChannel * channel = runner()->rawData()->channel(i, j);
			h_nTriggers->SetBinContent(i*runner()->geometry()->nChannels() + j, 
					channel->nTriggers()/(1.*runner()->nEvents()));
		}
	}

	for (unsigned int i=0; i<h_triggerValues->size(); i++){
		for (unsigned int j=0; j<h_triggerValues->at(i)->GetNbinsX(); j++) {
			h_allTriggers->SetBinContent(i, j, h_triggerValues->at(i)->GetBinContent(j));	
		}
	}

	
	runner()->saveFile()->cd(name().c_str());
	h_dipValues->Write();
	h_peakValues->Write();
	h_allTriggerValues->Write();
	h_dipVsPeakValues->Write();
	h_dipVsTriggerValues->Write();
	h_peakVsTriggerValues->Write();
	h_nTriggers->Write();
	h_allTriggers->Write(); 
}


//_____________________________________________________________________________


