# PrePal
Pre-processing codes for getting raw files ready for use in Palabos

To compile:

g++ -O3 -o make012 make012.cc
g++ -O3 -o trimBufferMakeDat trimBufferMakeDat.cc

Usage:
make012 nx ny nz RawFile.raw

trimBufferMakeDat nx ny nz ix iy iz Cystn_PV_nx_ny_nz_zot.raw

Description:

make012

Palabos wants as input a file where each voxel is either a 0 (pore space), 2 (solid) or 1 (interface).  make012 takes as input 3 integers that give the voxel
dimensions of the raw file, as well as the raw file itself.  

IMPORTANT:  The raw file provided to make012 should have voxels that are either
0 (pore space) or 1 (solid).  If they are differently valued, i.e., 0 and 255,
an additional pre-processing step is necessary.

Each solid voxel is checked to see if any of it's 26 neighbors are different.
If they are, it is an interface voxel, otherwise, it remains a solid.  Pore
space voxels are left unchanged.

Output is placed in a file named Cystn_PV_nx_ny_nz_zot.raw  The prefix is short for
Cyberstone_PoreValues_ and the suffix zot stands for "zero, one or two" just to let the user know a bit more about what information the file contains.

trimBufferMakeDat

This takes the output of make012 and trims the pack down to the desired 
subset size of the user.  Note that make012 does NOT treat the edge voxels, so
at a minimum, these should be removed.  This code also adds two layers of
void voxels at both the inlet and outlet of the pack.

trimBufferMakeDat will create two files.  Both have the base name
Cystn_PV_ix_iy_iz_zot_trim  One has a .raw extension, the other a .dat
extension.  Palabos actually uses the .dat file, but the .raw file is useful
as it is easy to look at in, e.g., ImageJ.  The .dat file is a flat text file with
a single entry on each line (for a voxel) that contains a 0, 1 or 2.
