# apov-datasets-converter
Experimental converter of sliced 3-Dimensional data sets, to the voxels.bin file
entry used by the APoV generator. This current version has only been tested with
MRbrain from graphics.stanford.edu for now.

Extract the slices in a folder (MRbrain for example) at the root of the repository,
then execute the converter as below:
./bin/apov-convert MRbrain slice-count:109 threshold:64 depth-size:2 bit-mask:0x000E

