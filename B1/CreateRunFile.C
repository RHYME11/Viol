
{
int start = 0;
int stop = 4;

TTree tree("tree","tree");
for(int x=start;x<=stop;x++){
    tree.ReadFile(Form("run_%i.dat",x));
}
tree.Print();
TFile *f = new TFile("run.root","recreate");
tree.Write();
f->Close();
std::cout<<"finish writing root file"<<std::endl;
}
