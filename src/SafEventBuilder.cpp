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
  m_firstTime = true;
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
	
	m_waveform = new std::vector<int>;
	m_tree->SetBranchAddress("glib",&m_glib);
	m_tree->SetBranchAddress("glibchan",&m_glibchan);
	m_tree->SetBranchAddress("trigger",&m_trigger);
	m_tree->SetBranchAddress("layer",&m_layer);
	m_tree->SetBranchAddress("chanx",&m_chanx);
	m_tree->SetBranchAddress("chany",&m_chany);
	m_tree->SetBranchAddress("waveform",&m_waveform);

	runner()->saveFile()->cd();
}


//_____________________________________________________________________________

void SafEventBuilder::execute()
{
	runner()->rawData()->clear();
	if (runner()->runMode() == 0) {
		monteCarlo();
	}
	else realData(runner()->geometry()->nChannels()*runner()->geometry()->nGlibs());
}

//_____________________________________________________________________________

void SafEventBuilder::finalize()
{
}


//_____________________________________________________________________________

void SafEventBuilder::realData(unsigned int channelIndexUpper)
{
	// Read tree.
	m_tree->GetEntry(m_treePos);
	while (m_trigger == runner()->event()) {
		m_tree->GetEntry(m_treePos);
		m_treePos++;
		SafRawDataChannel * channel = runner()->rawData()->channel(m_glib-112, m_glibchan);
		unsigned int size = m_waveform->size();
		for (unsigned int i=0; i<size; i++) {
			channel->signals()->push_back(m_waveform->at(i));
			channel->times()->push_back(i);
		}
		channel->setNEntries(channel->times()->size());

		if (m_firstTime) m_firstTime = false;
		unsigned int channelIndex = (m_glib-112)*runner()->geometry()->nChannels() + m_glibchan;
		if (channelIndex > channelIndexUpper) break;
	}
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

