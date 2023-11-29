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

  // This writes a raw file out where each voxel is
  // 0 (pore space) or 1 (solid)
  // The geometry is a block with a centrally located cylinder

  if(argc < 5){
    cout << "Please provide 3 integers representing the number " << endl;
    cout << "of voxels in each the x, y and z directions." << endl;
    cout << "Also provide the RADIUS of the tube as a fraction " << endl;
    cout << "of the block.  e.g.:" << endl;
    cout << "> createTube 100 100 100 .1 will create a block with" << endl;
    cout << "a void cylinder in the middle of diameter 20 voxels" << endl;
    exit(1);
  }

  // Parse argument list to get image stack dimensions 
  // and name of file containing the image stack
  int Iorig=atoi(argv[1]);
  int Jorig=atoi(argv[2]);
  int Korig=atoi(argv[3]);
  float radius=atof(argv[4]);

  int rad = radius*Iorig;
  cout << "rad = " << rad << endl;

  if(Iorig!=Jorig){
    cout << "Block dimensions must be the same in the x and y directions.\n";
    exit(1);
  }

  string outRoot = "tube_";
  ostringstream fnumx, fnumy, fnumz, frad;
  fnumx << Iorig;
  fnumy << Jorig;
  fnumz << Korig;
  frad  << rad;
  string outFile = outRoot + fnumx.str() + "_" + fnumy.str() + "_" 
                           + fnumz.str() + "_" + frad.str() + ".raw";

  cout << "raw file will be placed in " << outFile << endl;
  FILE *output;
  output = fopen(outFile.c_str(),"w");

  unsigned int totalVoxels = Iorig*Jorig*Korig;
  std::vector<short> vx(totalVoxels);

  cout << "totalVoxels = " << totalVoxels << endl;

  cout << "vx.size() = " << vx.size() << endl;

  char* voxel;
  voxel = new char[totalVoxels];

  int midx=Iorig/2;
  int midy=Jorig/2;

  // Check the 26 surrounding voxels.  If any of their values
  // differ from the current voxel values, the current voxel
  // is deemed to be a border voxel
  for(int k=0; k<Korig; k++){
    for(int j=0; j<Jorig; j++){
      for(int i=0; i<Iorig; i++){
        unsigned int p = get1DIndex(i,j,k,Iorig,Jorig);
        if((i-midx)*(i-midx) + (j-midy)*(j-midy) < rad*rad){
           vx[p]=0;
        } else {
           vx[p]=1;
        }
      }
    }
  }

  // Put the value of vx back into the char* voxel array.
  for(unsigned int n=0; n<totalVoxels; n++){
    voxel[n] = vx[n];
  }

  fwrite(voxel,1,totalVoxels,output);

  fclose(output);
}
