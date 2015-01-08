/*
 * SafEventBuilder.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: ds9133
 */

#include "SafEventBuilder.h"

//_____________________________________________________________________________

SafEventBuilder::SafEventBuilder(SafRunner * runner) :
  SafAlgorithm(runner, "SafEventBuilder")
{
	m_randGen = new TRandom3();
  m_randGen->SetSeed();
  m_mean = 8000;
  m_rms = 100;
  m_treePos = 0;
}


//_____________________________________________________________________________

SafEventBuilder::~SafEventBuilder()
{
	// TODO Auto-generated destructor stub
}


//_____________________________________________________________________________

void SafEventBuilder::initialize()
{
	if (!runner()->rawData()) new SafRawDataSet(runner());
	TFile * fInput = new TFile("test_02dec1445.root", "r");
	m_tree = (TTree*)fInput->Get("waveforms;64");
	runner()->saveFile()->cd();
}


//_____________________________________________________________________________

void SafEventBuilder::execute()
{
	runner()->rawData()->clear();
	if (runner()->runMode() == 0) {
		monteCarlo();
	}
	else realData();
}

//_____________________________________________________________________________

void SafEventBuilder::finalize()
{
}


//_____________________________________________________________________________

void SafEventBuilder::realData()
{
	// Read tree.
  int glib, glibchan,trigger,layer,chanx,chany;
	std::vector<int> * waveform = new std::vector<int>;
	m_tree->SetBranchAddress("glib",&glib);
	m_tree->SetBranchAddress("glibchan",&glibchan);
	m_tree->SetBranchAddress("trigger",&trigger);
	m_tree->SetBranchAddress("layer",&layer);
	m_tree->SetBranchAddress("chanx",&chanx);
	m_tree->SetBranchAddress("chany",&chany);
	m_tree->SetBranchAddress("waveform",&waveform);

	m_tree->GetEntry(m_treePos);
	while (trigger == runner()->event()) {
		m_tree->GetEntry(m_treePos);
		m_treePos++;
		SafRawDataChannel * channel = runner()->rawData()->channel(glib-112, glibchan);
		for (unsigned int i=0; i<waveform->size(); i++) {
			channel->signals()->push_back(waveform->at(i));
			channel->times()->push_back(i);
			//plot(channel->signals()->at(i), "AllSignals", "AllSignals", 500, 7000, 9000);
		}
		channel->setNEntries(channel->times()->size());
	}

	//runner()->saveFile()->cd();
}



//_____________________________________________________________________________

void SafEventBuilder::monteCarlo()
{
	for (unsigned int ig = 0; ig < runner()->geometry()->nGlibs(); ig++) {
		for (unsigned int ic = 0; ic < runner()->geometry()->nChannels(); ic++) {
			SafRawDataChannel * channel = runner()->rawData()->channel(ig, ic);
			for (unsigned int i=0; i<runner()->eventTimeWindow(); i++) {
				int time = i;
				double signal = m_randGen->Gaus(m_mean, m_rms);
				channel->times()->push_back(time);
				channel->signals()->push_back(signal);
				plot(signal, "SafEventBuilder/allSignals", "allSignals", 100, 7000, 9000);
			}
			channel->setNEntries(runner()->eventTimeWindow());
		}
	}
}


//_____________________________________________________________________________

