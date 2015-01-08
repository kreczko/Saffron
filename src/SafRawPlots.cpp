/*
 * SafRawPlots.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: ds9133
 */

#include "SafRawPlots.h"


//_____________________________________________________________________________

SafRawPlots::SafRawPlots(SafRunner * runner) :
  SafAlgorithm(runner, "SafRawPlots")
{}


//_____________________________________________________________________________

SafRawPlots::~SafRawPlots()
{
	// TODO Auto-generated destructor stub
}



//_____________________________________________________________________________

void SafRawPlots::initialize()
{
	m_threading = true;
	unsigned int nG = runner()->geometry()->nGlibs();
	unsigned int nC = runner()->geometry()->nChannels();

	h_firstEventWaveforms = initPerChannelPlots("FirstEventWaveForm", "FirstEventWaveForm", 
		runner()->eventTimeWindow(), 0.0, runner()->eventTimeWindow());
	h_signals = initPerChannelPlots("Signal", "Signal", 2000, 7000, 12000);


	int nChannels = nC*nG;
	h_allSignals = new TH2F("AllSignalDist", "AllSignalDist", nChannels, -0.5,
			nChannels-0.5, 2000, 7500., 16000.);
	h_signalMeans = new TH1F("SignalMeans", "SignalMeans", nChannels, -0.5, nChannels-0.5);
	h_signalWidths = new TH1F("SignalWidths", "SignalWidths", nChannels, -0.5, nChannels-0.5);


	// Root file directories.
	TDirectory * instance_direc = runner()->saveFile()->mkdir(name().c_str());
	instance_direc->mkdir("FirstWaveforms");
	for (unsigned int i=0; i<nG; i++) {
		std::stringstream ssGlib; ssGlib<<i;
		instance_direc->mkdir(("FirstWaveforms/Glib" + ssGlib.str()).c_str());
	}

	instance_direc->mkdir("Signals");
	for (unsigned int i=0; i<nG; i++) {
		std::stringstream ssGlib; ssGlib<<i;
		instance_direc->mkdir(("Signals/Glib" + ssGlib.str()).c_str());
	}
}


//_____________________________________________________________________________

void SafRawPlots::execute()
{
	for (unsigned int i=0; i<runner()->geometry()->nGlibs(); i++){
		threadExecute(i, 0, runner()->geometry()->nChannels());
	}
}


//_____________________________________________________________________________

void SafRawPlots::finalize()
{
	for (unsigned int i=0; i<h_firstEventWaveforms->size(); i++) {
		int iGlib = i/runner()->geometry()->nChannels();
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/FirstWaveforms/Glib" + ssGlib.str()).c_str());

		h_firstEventWaveforms->at(i)->Write();
	}

	for (unsigned int i=0; i<h_signals->size(); i++) {
		for (unsigned int j=0; j<h_signals->at(i)->GetNbinsX(); j++) {
			h_allSignals->SetBinContent(i, j, h_signals->at(i)->GetBinContent(j));
		}
		int iGlib = i/runner()->geometry()->nChannels();
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/Signals/Glib" + ssGlib.str()).c_str());

		h_signals->at(i)->Write();
		int fitStatus = h_signals->at(i)->Fit("gaus", "Q");
		if (fitStatus != 0) continue;
		h_signalMeans->SetBinContent(i, h_signals->at(i)->GetFunction("gaus")->GetParameter(1));
		h_signalMeans->SetBinError(i, h_signals->at(i)->GetFunction("gaus")->GetParError(1));

		h_signalWidths->SetBinContent(i, h_signals->at(i)->GetFunction("gaus")->GetParameter(2));
		h_signalWidths->SetBinError(i, h_signals->at(i)->GetFunction("gaus")->GetParError(2));
	}

	runner()->saveFile()->cd(name().c_str());
	h_allSignals->Write();
	h_signalMeans->Write();
	h_signalWidths->Write();

	runner()->saveFile()->cd();
}


//_____________________________________________________________________________

void SafRawPlots::threadExecute(unsigned int iGlib, unsigned int iLow, 
	unsigned int iUp)
{
	unsigned int nChannels = runner()->geometry()->nChannels();
	for (unsigned int i=iLow; i<iUp; i++) {
		SafRawDataChannel * channel = runner()->rawData()->channel(iGlib, i);
		unsigned int plotIndex = iGlib*nChannels + i;

		for (unsigned int k=0; k<channel->nEntries(); k++) {
			if (runner()->event() == 0) {
				h_firstEventWaveforms->at(plotIndex)->SetBinContent(
						channel->times()->at(k), channel->signals()->at(k));
			}				
			h_signals->at(plotIndex)->Fill(channel->signals()->at(k));			
		}
	}
}


//_____________________________________________________________________________
