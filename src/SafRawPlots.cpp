/*
 * SafRawPlots.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: ds9133
 */

#include "SafRawPlots.h"


//_____________________________________________________________________________

SafRawPlots::SafRawPlots(SafRunner * runner, bool filtered) :
  SafAlgorithm(runner, "SafRawPlots")
{
	m_filtered = filtered;
}


//_____________________________________________________________________________

SafRawPlots::~SafRawPlots()
{
	// TODO Auto-generated destructor stub
}



//_____________________________________________________________________________

void SafRawPlots::initialize()
{
	m_threading = true;

	std::string direcName = name();
	if (m_filtered) direcName += "-Filtered";

	unsigned int nG = runner()->geometry()->nGlibs();
	unsigned int nC = runner()->geometry()->nChannels();

	h_firstEventWaveforms = initPerChannelPlots("FirstEventWaveForm", "FirstEventWaveForm", 
		runner()->eventTimeWindow(), 0.0, runner()->eventTimeWindow());
	h_signals = initPerChannelPlots("Signal", "Signal", 3000, 7000, 16000);


	int nChannels = nC*nG;
	h_allSignals = new TH2F("AllSignalDist", "AllSignalDist", nChannels, -0.5,
			nChannels-0.5, 3000, 7000., 16000.);
	h_signalMeans = new TH1F("SignalMeans", "SignalMeans", nChannels, -0.5, nChannels-0.5);
	h_signalWidths = new TH1F("SignalWidths", "SignalWidths", nChannels, -0.5, nChannels-0.5);


	// Root file directories.
	TDirectory * instance_direc = runner()->saveFile()->mkdir(direcName.c_str());
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


	// Per thread plots.
	for (unsigned int i=0; i<runner()->geometry()->nGlibs()*m_nThreadsPerGlib; i++) {
		h_avSignalPerEventPerChannelTemp.push_back(new TH2F("temp", "temp", 
			m_nChannelsPerThread, i*m_nChannelsPerThread, (i+1)*m_nChannelsPerThread, runner()->nEvents(), 0, runner()->nEvents()));
		h_rmsSignalPerEventPerChannelTemp.push_back(new TH2F("temp", "temp", 
			m_nChannelsPerThread, i*m_nChannelsPerThread, (i+1)*m_nChannelsPerThread, runner()->nEvents(), 0, runner()->nEvents()));
	}

	h_avSignalPerEventPerChannel = new TH2F("AvSignalPerEventPerChannel", 
		"AvSignalPerEventPerChannel", nC*nG, 0, nC*nG, runner()->nEvents(), 0, runner()->nEvents());
  h_rmsSignalPerEventPerChannel = new TH2F("RMSSignalPerEventPerChannel",
		"RMSSignalPerEventPerChannel", nC*nG, 0, nC*nG, runner()->nEvents(), 0, runner()->nEvents());
}


//_____________________________________________________________________________

void SafRawPlots::execute()
{
	for (unsigned int i=0; i<runner()->geometry()->nGlibs(); i++){
		threadExecute(i, 0, runner()->geometry()->nChannels(), -1);
	}
}


//_____________________________________________________________________________

void SafRawPlots::finalize()
{
	std::string direcName = name();
	if (m_filtered) direcName += "-Filtered";
	for (unsigned int i=0; i<h_firstEventWaveforms->size(); i++) {
		int iGlib = i/runner()->geometry()->nChannels();
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((direcName + "/FirstWaveforms/Glib" + ssGlib.str()).c_str());

		h_firstEventWaveforms->at(i)->Write();
	}

	for (unsigned int i=0; i<h_signals->size(); i++) {
		for (unsigned int j=0; j<h_signals->at(i)->GetNbinsX(); j++) {
			h_allSignals->SetBinContent(i, j, h_signals->at(i)->GetBinContent(j));
		}
		int iGlib = i/runner()->geometry()->nChannels();
		std::stringstream ssGlib; ssGlib << iGlib;
		runner()->saveFile()->cd((direcName + "/Signals/Glib" + ssGlib.str()).c_str());

		h_signals->at(i)->Write();
		int fitStatus = h_signals->at(i)->Fit("gaus", "Q");
		if (fitStatus != 0) continue;
		h_signalMeans->SetBinContent(i, h_signals->at(i)->GetFunction("gaus")->GetParameter(1));
		h_signalMeans->SetBinError(i, h_signals->at(i)->GetFunction("gaus")->GetParError(1));

		h_signalWidths->SetBinContent(i, h_signals->at(i)->GetFunction("gaus")->GetParameter(2));
		h_signalWidths->SetBinError(i, h_signals->at(i)->GetFunction("gaus")->GetParError(2));
	}

	// Thread plots.
	if (m_threading && !m_forceSingleThread) {
		for (unsigned int i=0; i<h_avSignalPerEventPerChannelTemp.size(); i++) {
			for (unsigned int j=0; j<m_nChannelsPerThread; j++) {
				for (unsigned int k=0; k<runner()->nEvents(); k++) {

					unsigned int iChannel = j+i*m_nChannelsPerThread;
					double av = h_avSignalPerEventPerChannelTemp[i]->GetBinContent(j, k);
					double rms = h_rmsSignalPerEventPerChannelTemp[i]->GetBinContent(j, k);

					if (m_rmsCache.size() < runner()->geometry()->nGlibs()*
						runner()->geometry()->nChannels()) {
						m_avCache.push_back(av);
						m_rmsCache.push_back(rms);
					}
					av /= m_avCache[iChannel];
					rms /= m_rmsCache[iChannel];

				  h_avSignalPerEventPerChannel->SetBinContent(iChannel, k, av);
				  h_rmsSignalPerEventPerChannel->SetBinContent(iChannel, k, rms);
				}
			}
		}
	}

	runner()->saveFile()->cd(direcName.c_str());
	h_allSignals->Write();
	h_signalMeans->Write();
	h_signalWidths->Write();
	h_avSignalPerEventPerChannel->Write();
	h_rmsSignalPerEventPerChannel->Write();

	runner()->saveFile()->cd();
}


//_____________________________________________________________________________

void SafRawPlots::threadExecute(unsigned int iGlib, unsigned int iLow, 
	unsigned int iUp, int iThread)
{
	unsigned int nChannels = runner()->geometry()->nChannels();
	for (unsigned int i=iLow; i<iUp; i++) {
		double sum = 0.0;
		double sumSq = 0.0;
		SafRawDataChannel * channel = runner()->rawData()->channel(iGlib, i);
		unsigned int plotIndex = iGlib*nChannels + i;

		for (unsigned int k=0; k<channel->nEntries(); k++) {
			if (runner()->event() == 0) {
				h_firstEventWaveforms->at(plotIndex)->SetBinContent(
						channel->times()->at(k), channel->signals()->at(k));
			}				
			double signal = channel->signals()->at(k);
			h_signals->at(plotIndex)->Fill(signal);
			sum += signal;
			sumSq += signal*signal;
		}
		double av = sum/(1.*channel->nEntries());
		double rms = pow(sumSq/(1.*channel->nEntries()) - av*av, 0.5);

		if (iThread != -1) {
			h_avSignalPerEventPerChannelTemp[iThread]->SetBinContent(i, m_event, av);
			h_rmsSignalPerEventPerChannelTemp[iThread]->SetBinContent(i, m_event, rms);
		}
		else {
			if (m_rmsCache.size() < runner()->geometry()->nGlibs()*nChannels) {
				m_avCache.push_back(av);
				m_rmsCache.push_back(rms);
			}
			av /= m_avCache[i+iGlib*nChannels];
			rms /= m_rmsCache[i+iGlib*nChannels];

			h_avSignalPerEventPerChannel->SetBinContent(i+iGlib*nChannels, m_event, av);
			h_rmsSignalPerEventPerChannel->SetBinContent(i+iGlib*nChannels, m_event, rms);
		}
	}
}


//_____________________________________________________________________________
