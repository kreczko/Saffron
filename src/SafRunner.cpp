/*
 * SafRunner.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: Daniel Saunders
 */

#include "SafRunner.h"

//____________________________________________________________________________

SafRunner::SafRunner() :
  m_printThreshold(0),
  m_algorithms(),
  m_rawData(NULL),
  m_eventTimeWindow(2048),
  m_event(0),
  m_timeZero(0),
  m_printRate(1)
{
	// Default algorithm list.
	m_algorithms.push_back(new SafEventBuilder(this));
	m_algorithms.push_back(new SafRawPlots(this));
	m_algorithms.push_back(new SafTrigger(this));
	m_algorithms.push_back(new SafTriggerPlots(this));

	// Geometry.
	m_geometry = new SafGeometry();

	// Options.
	m_nEvents = 15;
	m_runMode = 1; // 0 for MC, 1 for real data.

	// Save file.
	m_saveFile = new TFile("Saffron-histos.root", "RECREATE");
}


//____________________________________________________________________________

SafRunner::~SafRunner()
{
	m_saveFile->Close();
}


//____________________________________________________________________________

void SafRunner::safPrint(std::string text, int level)
{
	std::cout<<text<<std::endl;
}


//____________________________________________________________________________

void SafRunner::run()
{
	// Default runner of Saffron. In turn, calls all initialise, execute and
	// finalize methods of all SafAlgorithms used in this run.

	std::vector< SafAlgorithm* >::iterator ialgo;
	for (ialgo = m_algorithms.begin(); ialgo != m_algorithms.end(); ialgo++)
		(*ialgo)->detailedInitialize();


	// Event loop.
	for (unsigned int i=0; i<m_nEvents; i++) {
		bool eof = false;
  	if (m_event % m_printRate == 0) std::cout<<"Event: "<<m_event<<std::endl;
		for (ialgo = m_algorithms.begin(); ialgo != m_algorithms.end(); ialgo++) {
		  (*ialgo)->detailedExecute();
			if ((*ialgo)->eof()) eof = true;
	  }
	  if (eof) break;
	  m_event++;
	}


	for (ialgo = m_algorithms.begin(); ialgo != m_algorithms.end(); ialgo++)
		(*ialgo)->detailedFinalize();
	
	double totTime = 0;
	std::cout<<"\n--------- Algorithm Average Execute Time (us) ---------"<<std::endl;
	for (ialgo = m_algorithms.begin(); ialgo != m_algorithms.end(); ialgo++) {
		std::cout<<(*ialgo)->name()<<"\t\t"<<(*ialgo)->AvTime()<<std::endl;
		totTime += (*ialgo)->AvTime();
	}
	std::cout<<"\nTotal Time: \t\t"<<totTime<<std::endl;
}


//____________________________________________________________________________

