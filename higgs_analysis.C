#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif


void higgs_analysis()
{
    gSystem->Load("libDelphes.so"); // Load the Delphes library to access the classes and functions defined in it, which are necessary for reading and analyzing the Delphes output files

    // Reading signal and background files and creating TChain and ExRootTreeReader objects for both signal and background processes
    TChain signal_chain("Delphes"); 
    signal_chain.Add("/Users/koyulmaz/program_files/MG5_aMC_v3_7_0/bin/pp2h2aa/Events/run_01/tag_1_delphes_events.root"); 
    ExRootTreeReader *signal_treeReader = new ExRootTreeReader(&signal_chain); 

    TChain background_pp2aa_chain("Delphes"); 
    background_pp2aa_chain.Add("/Users/koyulmaz/program_files/MG5_aMC_v3_7_0/bin/pp2aa/Events/run_01/tag_1_delphes_events.root"); 
    ExRootTreeReader *background_pp2aa_treeReader = new ExRootTreeReader(&background_pp2aa_chain); 

    // Set up branch addresses for the signal and background TTree readers to access the Event and Photon branches
    TClonesArray *signal_branchEvent    = signal_treeReader->UseBranch("Event");
    TClonesArray *signal_branchPhoton = signal_treeReader->UseBranch("Photon");

    TClonesArray *background_pp2aa_branchEvent    = background_pp2aa_treeReader->UseBranch("Event");
    TClonesArray *background_pp2aa_branchPhoton = background_pp2aa_treeReader->UseBranch("Photon");    

    //Output file and tree creation
    TFile *outFile = new TFile("higgs_analysis_output.root", "RECREATE");
    TTree *signal_outTree = new TTree("signal", "tree");


    // Define the cross-sections for the signal and the integrated luminosity for the analysis
    double signal_crossSection = 0.026; // pb
    double background_pp2aa_crossSection = 148.8; // pb
    double luminosity = 3000000; // pb^-1

    int n_photons; // Number of photons in the event
    float photon1_pt, photon1_eta, photon1_phi; // Kinematic variables for the leading photon
    float photon2_pt, photon2_eta, photon2_phi; // Kinematic variables for the subleading photon
    float m_aa; // Invariant mass of the diphoton system
    float deltaR_photons; // Delta R between the two photons
    double weight; // Event weight based on cross-section and luminosity
    
    signal_outTree->Branch("n_photons", &n_photons);     // Number of photons in the event
    signal_outTree->Branch("photon1_pt", &photon1_pt);   // Transverse momentum of the leading photon
    signal_outTree->Branch("photon1_eta", &photon1_eta); // Pseudorapidity of the leading photon
    signal_outTree->Branch("photon1_phi", &photon1_phi); // Azimuthal angle of the leading photon
    signal_outTree->Branch("photon2_pt", &photon2_pt);   // Transverse momentum of the subleading photon
    signal_outTree->Branch("photon2_eta", &photon2_eta); // Pseudorapidity of the subleading photon
    signal_outTree->Branch("photon2_phi", &photon2_phi); // Azimuthal angle of the subleading photon   
    signal_outTree->Branch("m_aa", &m_aa);               // Invariant mass of the diphoton (Higgs) system
    signal_outTree->Branch("deltaR_photons", &deltaR_photons); // Delta R between the two photons
    signal_outTree->Branch("weight", &weight);                 // Event weight based on cross-section and luminosity



    // Loop over the entries and fill the tree wrt to the signal process and calculating the necessary variables for the analysis
    Long64_t numberOfEntries = signal_treeReader->GetEntries(); // Get the total number of entries in the signal TTree
    for(Int_t entry = 0; entry < numberOfEntries; ++entry)
    {
      signal_treeReader->ReadEntry(entry); // Read the current entry from the signal TTree and load the data into memory   
      weight = signal_crossSection * (double)(luminosity/numberOfEntries); // Calculate the event weight based on the cross-section and luminosity

      n_photons = signal_branchPhoton->GetEntries(); // Get the number of photons in the current event from the Photon branch

      TLorentzVector p4_photon1, p4_photon2; // Define TLorentzVector objects to hold the four-momentum of the leading and subleading photons


      // Check if there are at least 2 photons in the event to proceed with the analysis
      if (n_photons >= 2){ 
        Photon *photon1 = (Photon*) signal_branchPhoton->At(0); // Get the leading photon (first photon in the array) from the Photon branch and cast it to a Photon object
        Photon *photon2 = (Photon*) signal_branchPhoton->At(1); // Get the subleading photon (second photon in the array) from the Photon branch and cast it to a Photon object

        // Get the four-momentum of the leading and subleading photons using the P4() method of the Photon class
        p4_photon1 = photon1->P4(); 
        p4_photon2 = photon2->P4(); 

        // Get kinematics for the leading and subleading photons 
        photon1_pt = p4_photon1.Pt(); 
        photon2_pt = p4_photon2.Pt(); 

        photon1_eta = p4_photon1.Eta();  
        photon2_eta = p4_photon2.Eta();       

        photon1_phi = p4_photon1.Phi(); 
        photon2_phi = p4_photon2.Phi(); 

        m_aa = (p4_photon1 + p4_photon2).M(); // Calculate the invariant mass of the diphoton (Higgs) system 
        deltaR_photons = p4_photon1.DeltaR(p4_photon2); // Calculate the Delta R between the two photons using the DeltaR()

        cout << "Photon #: " << n_photons << " " << photon1_pt << " " << photon1_eta << " " << photon1_phi << " | " 
             << photon2_pt << " " << photon2_eta << " " << photon2_phi << endl;

        signal_outTree->Fill(); // Fill the signal TTree with the calculated variables for the current event (number of photons, kinematic variables of the leading and subleading photons, invariant mass of the diphoton system, Delta R between the photons, and event weight)     
      }  
    }

    //Background analysis for pp2aa process and repating the same steps as for the signal analysis
    TTree *background_pp2aa_outTree = new TTree("pp2aa", "tree");
    background_pp2aa_outTree->Branch("n_photons", &n_photons);
    background_pp2aa_outTree->Branch("photon1_pt", &photon1_pt);
    background_pp2aa_outTree->Branch("photon1_eta", &photon1_eta);
    background_pp2aa_outTree->Branch("photon1_phi", &photon1_phi);
    background_pp2aa_outTree->Branch("photon2_pt", &photon2_pt);
    background_pp2aa_outTree->Branch("photon2_eta", &photon2_eta);
    background_pp2aa_outTree->Branch("photon2_phi", &photon2_phi);    
    background_pp2aa_outTree->Branch("m_aa", &m_aa); 
    background_pp2aa_outTree->Branch("deltaR_photons", &deltaR_photons);  
    background_pp2aa_outTree->Branch("weight", &weight);   

    numberOfEntries = background_pp2aa_treeReader->GetEntries();
    for(Int_t entry = 0; entry < numberOfEntries; ++entry)
    {
      background_pp2aa_treeReader->ReadEntry(entry);    
      weight = background_pp2aa_crossSection * (double)(luminosity/numberOfEntries);

      n_photons = background_pp2aa_branchPhoton->GetEntries();

      TLorentzVector p4_photon1, p4_photon2;

      if (n_photons >= 2){
        Photon *photon1 = (Photon*) background_pp2aa_branchPhoton->At(0);
        Photon *photon2 = (Photon*) background_pp2aa_branchPhoton->At(1);

        p4_photon1 = photon1->P4();
        p4_photon2 = photon2->P4();

        photon1_pt = p4_photon1.Pt();
        photon2_pt = p4_photon2.Pt();

        photon1_eta = p4_photon1.Eta();
        photon2_eta = p4_photon2.Eta();        

        photon1_phi = p4_photon1.Phi();
        photon2_phi = p4_photon2.Phi();   

        m_aa = (p4_photon1 + p4_photon2).M();
        deltaR_photons = p4_photon2.DeltaR(p4_photon1);

        cout << "pp2aa -> Photon #: " << n_photons << " " << photon1_pt << " " << photon1_eta << " " << photon1_phi << " | " 
             << photon2_pt << " " << photon2_eta << " " << photon2_phi << endl;

        background_pp2aa_outTree->Fill();     
      }  
    }    
    outFile->WriteTObject(signal_outTree);
    outFile->WriteTObject(background_pp2aa_outTree);
    outFile->Close();
}