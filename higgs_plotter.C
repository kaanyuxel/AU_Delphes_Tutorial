void higgs_plotter()
{

    TFile *f = new TFile("higgs_analysis_output.root");
    TTree *signal = (TTree *) f->Get("signal"); // To read signal file
    TTree *background_pp2aa = (TTree *) f->Get("pp2aa"); // To read background file

    TH1 *h_signal = new TH1F("h_signal","Invariant Mass of Higgs", 70, 70, 140);  // Create a histogram to store the invariant mass 
    TH1 *h_background_pp2aa = new TH1F("h_background_pp2aa","Invariant Mass of Higgs", 70, 70, 140); // Create a histogram to store the invariant mass 

    TH1 *h_signal_cut = new TH1F("h_signal_cut","Invariant Mass of Higgs", 70, 70, 140);  // Create a histogram to store the invariant mass after cut
    TH1 *h_background_pp2aa_cut = new TH1F("h_background_pp2aa_cut","Invariant Mass of Higgs", 70, 70, 140);  // Create a histogram to store the invariant mass after cut

    TString without_cut = "(1)*weight"; //Cut definition for no cut   
    signal->Draw("m_aa>>h_signal", without_cut); //Draw the invariant mass distribution for the signal
    background_pp2aa->Draw("m_aa>>h_background_pp2aa", without_cut); //Draw the invariant mass distribution for the pp2aa background


    // Draw the invariant mass distributions for both signal and background on the same canvas without cut
    TCanvas *c1 = new TCanvas("c1","H -> #gamma #gamma Invariant Mass Distribution", 600, 400);
    THStack *ths_mass = new THStack("ths_mass",""); 
    h_signal->SetStats(0);
    h_signal->SetLineColor(kRed);
    ths_mass->Add(h_signal,"HIST");
    h_background_pp2aa->SetStats(0);
    h_background_pp2aa->SetLineColor(kBlue);
    ths_mass->Add(h_background_pp2aa,"HIST");
    ths_mass->Draw("nostack"); 	

    TString cut = "(photon1_pt > 50 && photon2_pt > 30 && abs(photon1_eta) < 2.5 && abs(photon2_eta) < 2.5)*weight"; //Cut definition for cut
    signal->Draw("m_aa>>h_signal_cut", cut); //Draw the invariant mass distribution for the signal
    background_pp2aa->Draw("m_aa>>h_background_pp2aa_cut", cut); //Draw the invariant mass distribution for the pp2aa background

    // Draw the invariant mass distributions for both signal and background on the same canvas after cut
    TCanvas *c2 = new TCanvas("c2","H -> #gamma #gamma Invariant Mass Distribution after Cut", 600, 400);
    THStack *ths_mass_cut = new THStack("ths_mass_cut",";m_{#gamma #gamma} [GeV];Events"); 
    h_signal_cut->SetStats(0);
    h_signal_cut->SetLineColor(kRed);
    ths_mass_cut->Add(h_signal_cut,"HIST");
    h_background_pp2aa_cut->SetStats(0);
    h_background_pp2aa_cut->SetLineColor(kBlue);
    ths_mass_cut->Add(h_background_pp2aa_cut,"HIST");
    ths_mass_cut->Draw("nostack"); 	

    double s_event_cut = h_signal_cut->Integral(40,60); // Calculate the number of signal events in the mass window 
    cout << "Number of signal events after cut: " << s_event_cut << endl; 
    double background = h_background_pp2aa_cut->Integral(40,60); // Calculate the number of background events in the mass window
    cout << "Number of background events after cut: " << background << endl;
    double significance = s_event_cut / sqrt(s_event_cut + background); // Calculate the significance using the formula S/sqrt(S+B)
    cout << "Significance after cut: " << significance << endl;
} 