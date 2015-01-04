/*
 * SafRawPlots.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: ds9133
 */

#include "SafRawPlots.h"


//_____________________________________________________________________________

SafRawPlots::SafRawPlots(SafRunner * runner) :
  SafAlgorithm(runner, "SafRawPlots"),
  m_threading(true)
{}


//_____________________________________________________________________________

SafRawPlots::~SafRawPlots()
{
	// TODO Auto-generated destructor stub
}



//_____________________________________________________________________________

void SafRawPlots::initialize()
{
	unsigned int nG = runner()->geometry()->nGlibs();
	unsigned int nC = runner()->geometry()->nChannels();

	for (unsigned int i=0; i<nG; i++) {
		std::stringstream ssGlib; ssGlib<<i;
		for (unsigned int j=0; j<nC; j++) {
			std::stringstream ssChan; ssChan<<j;

			// First set waveforms.
			std::string name = "FirstEventWaveForm" + ssChan.str() + "-" + ssGlib.str();
			TH1F * plot = new TH1F(name.c_str(), name.c_str(),
					runner()->eventTimeWindow(), 0.0, runner()->eventTimeWindow());
			h_firstEventWaveforms.push_back(plot);

			// Signal dists.
			name = "Signal" + ssChan.str() + "-" + ssGlib.str();
			h_signals.push_back(new TH1F(name.c_str(), name.c_str(), 500, 7500, 9000));
		}
	}

	int nChannels = nC*nG;
	h_allSignals = new TH2F("AllSignalDist", "AllSignalDist", nChannels, -0.5,
			nChannels-0.5, 500, 7500., 9000.);
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
	if (m_threading) {
		std::thread t1(&SafRawPlots::fill, this, 1);
		fill(0);
		t1.join();
	}
	else {
		fill(0);
		fill(1);
	}
}


//_____________________________________________________________________________

void SafRawPlots::finalize()
{
	for (unsigned int i=0; i<h_firstEventWaveforms.size(); i++) {
		int iGlib = i/76;
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/FirstWaveforms/Glib" + ssGlib.str()).c_str());

		h_firstEventWaveforms[i]->Write();
	}

	for (unsigned int i=0; i<h_signals.size(); i++) {
		int iGlib = i/76;
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((name() + "/Signals/Glib" + ssGlib.str()).c_str());

		h_signals[i]->Write();
		int fitStatus = h_signals[i]->Fit("gaus", "Q");
		if (fitStatus != 0) continue;
		h_signalMeans->SetBinContent(i, h_signals[i]->GetFunction("gaus")->GetParameter(1));
		h_signalMeans->SetBinError(i, h_signals[i]->GetFunction("gaus")->GetParError(1));

		h_signalWidths->SetBinContent(i, h_signals[i]->GetFunction("gaus")->GetParameter(2));
		h_signalWidths->SetBinError(i, h_signals[i]->GetFunction("gaus")->GetParError(2));
	}

	runner()->saveFile()->cd(name().c_str());
	h_allSignals->Write();
	h_signalMeans->Write();
	h_signalWidths->Write();

	runner()->saveFile()->cd();
}


//_____________________________________________________________________________

void SafRawPlots::fill(int thread)
{
	unsigned int nGlibs = runner()->geometry()->nGlibs();
	unsigned int nChannels = runner()->geometry()->nChannels();
	SafRawDataChannel * channel;
	unsigned int plotIndex;

	for (unsigned int i=0; i<nGlibs; i++) {
		for (unsigned int j=0; j<nChannels; j++) {
			channel = runner()->rawData()->channel(i, j);
			plotIndex = i*nChannels + j;

			for (unsigned int k=0; k<channel->nEntries(); k++) {
				if (thread == 0) {
					// Waveforms.
					if (runner()->event() == 0) {
						h_firstEventWaveforms[plotIndex]->SetBinContent(
								channel->times()->at(k), channel->signals()->at(k));
					}				
					h_allSignals->Fill(double(plotIndex), channel->signals()->at(k));
				}
		
		
				else if (thread == 1) {
					// Signal dists.
					h_signals[plotIndex]->Fill(channel->signals()->at(k));
				}
			}
		}
	}
}


//_____________________________________________________________________________
