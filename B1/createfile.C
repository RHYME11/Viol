
{
TTree tree("tree","tree");
tree.ReadFile("junk.dat");
tree.Print();
TFile *f = new TFile("junk.root","recreate");
tree.Write();
f->Close();
std::cout<<"finish writing root file"<<std::endl;
}
