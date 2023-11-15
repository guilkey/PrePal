#include<iostream>
#include<iomanip>
#include<fstream>
#include<vector>
#include<string>
#include<stdio.h>
using namespace std;

unsigned int get1DIndex(int i, int j, int k,
                        int X, int Y)
{
  // given voxel indices in 3D space, return an index
  // into the 1D array that stores the voxel values
  return k*(X*Y) + j*X + i;
}

int main(int argc, char** argv){

  // Reading in a raw file assuming that each voxel is 0 or 1.
  // Need to modify if voxels are 0 or 255
  // This writes a raw file back out where each voxel is
  // 0 (pore space), 1 (interface) or 2 (solid)
  // Boundary nodes are either 0 or 2, for now

  if(argc < 5){
    cout << "Please provide 3 integers representing the number " << endl;
    cout << "of voxels in each the x, y and z directions" << endl;
    cout << "as well as a raw file representing a 3D 8-bit " << endl;
    cout << "image where each voxel is either a 0 or 1" << endl;
    exit(1);
  }

  // Parse argument list to get image stack dimensions 
  // and name of file containing the image stack
  int Iorig=atoi(argv[1]);
  int Jorig=atoi(argv[2]);
  int Korig=atoi(argv[3]);
  char* inFile=argv[4];

  // Open input file containing the raw data
  ifstream input(inFile, ios::in | ios::binary);

  string outRoot = "Cystn_PV_";
  ostringstream fnumx, fnumy, fnumz;
  fnumx << Iorig;
  fnumy << Jorig;
  fnumz << Korig;
  string outFile = outRoot + fnumx.str() + "_" + fnumy.str() + "_" 
                           + fnumz.str() + "_zot.raw";
  cout << "Processed data will be placed in " << outFile << endl;
  cout << " where the zot in the name indicates that each " << endl;
  cout << "voxel has a value of Zero, One or Two" << endl;
  FILE *output;
  output = fopen(outFile.c_str(),"w");

  if(!input) {
      cout << "Cannot open input file" << inFile << endl;
      return 1;
  }

  unsigned int totalVoxels = Iorig*Jorig*Korig;
  std::vector<short> vx(totalVoxels);
  std::vector<short> zot(totalVoxels);

  cout << "totalVoxels = " << totalVoxels << endl;

  cout << "vx.size() = " << vx.size() << endl;

  char* voxel;
  voxel = new char[totalVoxels];
//  std::vector<char> voxel[totalVoxels];
  input.read(voxel, totalVoxels);

  // Convert char* to short int.  Multiply by 2
  // so all values are 0 or 2
  for(unsigned int n=0; n<totalVoxels; n++){
    vx[n]  = 2*voxel[n];
    zot[n] = 2*voxel[n];
  }

  // Check the 26 surrounding voxels.  If any of their values
  // differ from the current voxel values, the current voxel
  // is deemed to be a border voxel
  for(int k=1; k<Korig-1; k++){
    for(int j=1; j<Jorig-1; j++){
      for(int i=1; i<Iorig-1; i++){
        unsigned int p = get1DIndex(i,j,k,Iorig,Jorig);
        if(vx[p]>1){
          for(int l = -1;l<2;l++){
            for(int m = -1;m<2;m++){
              for(int n = -1;n<2;n++){
                unsigned int poffset = get1DIndex(i+l,j+m,k+n,Iorig,Jorig);
                if(vx[poffset]!=vx[p]){ 
                  zot[p]=1;
                  goto fuzz;
                }
              }
            }
          }
        }
        fuzz:;
      }
    }
  }

  // Put the value of zot back into the char* voxel array.
  for(unsigned int n=0; n<totalVoxels; n++){
    voxel[n] = zot[n];
  }

  fwrite(voxel,1,totalVoxels,output);

  input.close();
  fclose(output);
}
