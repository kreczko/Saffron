void treeShortener() {
	//Get old file, old tree and set top branch address
	TFile *oldfile = new TFile("test_02dec1445.root", "r");
	TTree *oldtree = (TTree*)oldfile->Get("waveforms;64");

	Long64_t nentries = oldtree->GetEntries();

	int m_glib, m_glibchan, m_trigger, m_layer, m_chanx, m_chany; 
	std::vector<int> * m_waveform = new std::vector<int>();
	oldtree->SetBranchAddress("glib",&m_glib);
	oldtree->SetBranchAddress("glibchan",&m_glibchan);
	oldtree->SetBranchAddress("trigger",&m_trigger);
	oldtree->SetBranchAddress("layer",&m_layer);
	oldtree->SetBranchAddress("chanx",&m_chanx);
	oldtree->SetBranchAddress("chany",&m_chany);
	oldtree->SetBranchAddress("waveform",&m_waveform);

	//Create a new file + a clone of old tree in new file
	TFile *newfile = new TFile("test_02dec1445Small.root","recreate");
	TTree *newtree = oldtree->CloneTree(0);

	for (int i=0; i<76*4*500; i++) {
		oldtree->GetEntry(i);
		newtree->Fill();
		if (i%(76*4*10) == 0) std::cout<<i<<std::endl;
	}

	newtree->Write();
 }