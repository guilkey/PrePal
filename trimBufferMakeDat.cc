#include<iostream>
#include<iomanip>
#include<fstream>
#include<vector>
#include<string>
#include<stdio.h>
using namespace std;

int get1DIndex(int i, int j, int k,
               int X, int Y)
{
  // given voxel indices in 3D space, return an index
  // into the 1D array that stores the voxel values
  return k*(X*Y) + j*X + i;
}

int main(int argc, char** argv){

  // Reads in a raw file assuming that each voxel is 0, 1 or 2.
  // Raw file dimensions are the first three command line entries
  // Data are trimmed, starting one voxel in from each edge, to
  // the size provided in the next three entries in the command line
  // Two slices of buffer region (zeros) are added to both ends in the
  // z-dimension

  if(argc < 7){
    cout << "Please provide:\n";
    cout << "Three integers representing the number of " << endl;
    cout << "voxels in each the x, y and z directions in the original file\n\n";
    cout << "An additional three integers represent the number of voxels in\n";
    cout << "x, y and z directions that the image is to be trimmed down to.\n\n";
    cout << "An input file name is also required, which should be an 8 bit\n";
    cout << "raw file in which each entry is a 0, 1 or 2 " << endl;
    cout << "This would typically be output of the make012 code" << endl;
    exit(1);
  }

  // Parse argument list to get image stack dimensions 
  // and name of file containing the image stack
  int Iorig=atoi(argv[1]);
  int Jorig=atoi(argv[2]);
  int Korig=atoi(argv[3]);
  int Inew =atoi(argv[4]);
  int Jnew =atoi(argv[5]);
  int Knew =atoi(argv[6]);
  char* inFile = argv[7];

  const int buffer = 2;  // This could be an input parameter

  // Raw file input
  ifstream input(inFile, ios::in | ios::binary);

  if(!input) {
      cout << "Cannot open input file " << inFile << endl;
      return 1;
  }

  // Open two output files.  One is the trimmed raw file.
  // The other is a ".dat" file that is a flat text file
  // that is the actual input to Palabos
  string outRoot = "Cystn_PV_";
  ostringstream fnumx, fnumy, fnumz;
  fnumx << Inew;
  fnumy << Jnew;
  fnumz << Knew+4;
  string outFileRaw = outRoot + fnumx.str() + "_" + fnumy.str() + "_" 
                              + fnumz.str() + "_zot_trim.raw";
  string outFileDat = outRoot + fnumx.str() + "_" + fnumy.str() + "_" 
                              + fnumz.str() + "_zot_trim.dat";
  cout << "Processed data will be placed in two files:\n";
  cout << "The first is " << outFileRaw << endl;
  cout << "where the _zot in the name indicates that each " << endl;
  cout << "voxel has a value of Zero, One or Two" << endl;
  cout << "and the _trim indicates that the geometry has been\n";
  cout << "trimmed from the original size.\n\n";
  cout << "The other file is " << outFileDat << " which is a flat text\n";
  cout << "file with the same information in the format that Palabos wants\n\n";

  FILE *output;
  output = fopen(outFileRaw.c_str(),"w");

  ofstream out(outFileDat.c_str());

  // Read in raw data that has been converted to 0, 1 or 2
  int totalVoxelsIn  = Iorig*Jorig*Korig;
  int totalVoxelsOut = Inew*Jnew*Knew;
  std::vector<short> zot(totalVoxelsIn);
  std::vector<short> clip(totalVoxelsOut);

  char* voxel;
  voxel = new char[totalVoxelsIn];
  input.read(voxel, totalVoxelsIn);

  char* trimWBuffer;
  trimWBuffer = new char[Inew*Jnew*(Knew+2*buffer)];

  // Fill front of trimWBuffer with zeros
  int p;
  for(p=0;p<Inew*Jnew*buffer;p++){
    trimWBuffer[p]=0;
  }

  // Trim the 3D array, starting from 1 from each edge
  int count=0;
  for(int k=1; k<=Knew; k++){
    for(int j=1; j<=Jnew; j++){
      for(int i=1; i<=Inew; i++){
        int p   = get1DIndex(i,j,k,Iorig,Jorig);
        int pm1 = get1DIndex(i-1,j-1,k-1,Inew,Jnew);
        clip[pm1]=voxel[p];
        count++;
      }
    }
  }

  for(int n=0; n<totalVoxelsOut; n++){
    trimWBuffer[n+p] = clip[n];
  }


  // Fill end of trimWBuffer with zeros
  int m = 0;
  for(m=p+totalVoxelsOut;m<p+totalVoxelsOut+Inew*Jnew*buffer;m++){
    trimWBuffer[m]=0;
  }

  int numEntries = Inew*Jnew*(Knew+2*buffer);
  // Write the raw file
  cout << "Writing raw file" << endl;
  fwrite(trimWBuffer,1,numEntries,output);
  cout << "Done writing raw file" << endl;

  // Write the dat file
  cout << "Writing dat file (this takes a little longer)" << endl;
  stringstream ss;
  for(int n=0;n<numEntries;n++){
    ss << (short) trimWBuffer[n] << "\n";
  }
  out << ss.str();
  
  cout << "Done writing dat file" << endl;

  input.close();
  fclose(output);
  out.close();
}
