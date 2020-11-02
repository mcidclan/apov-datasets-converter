# apov-datasets-converter
Experimental converter of sliced 3-Dimensional data sets, to the voxels.bin file
entry used by the APoV generator. The current version, is only supporting
grayscale slices. Tests have only been made with datasets, part of the "University
of North Carolina Volume Rendering Test Data Set".

Extract the slices in a folder (slices for example) at the root of the repository,
then execute the converter as one of the lines below:
Keeps only hard voxels:
./bin/apov-convert slices slice-count:109 depth-size:2 bit-and-mask:0x0008
Keeps background, remove imperfections:
./bin/apov-convert slices slice-count:109 depth-size:2 bit-and-mask:0x000E

You can generate the file with transparent voxels as shown in the following line:
./bin/apov-convert slices slice-count:109 depth-size:2 bit-and-mask:0x000E \ 
transparent-voxels rescale-luminosity-from:64
