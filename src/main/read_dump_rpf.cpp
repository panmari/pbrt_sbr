/*
 * File:   rec.cpp
 * Author: fabrice
 * Modified: sm
 */

#include <fstream>

#include "core/pbrt.h"
#include "rpf/RandomParameterFilter.h"
#include "rpf/SampleData.h"
#include "core/imageio.h"
#include "filter_utils/VectorNf.h"
using namespace std;

int main(int argc, char** argv)
{
    if (argc == 1) {
        Severe("No base name provided!, \n Usage: read_dump_rpf [file.bin] [quality] [jouni] [random_params]");
    }
    string filename(argv[1]);
    string quality;
    string random_params;
    float jouni;
    if (argc >= 3)
    	quality = string(argv[2]);
    else quality = "medium";
    if (argc >= 4)
    	jouni = std::stof(string(argv[3]));
    else jouni = 0.02f;
    if (argc >= 5)
    	random_params = string(argv[4]);
    else random_params = "all";

    vector<SampleData> allSamples;
    int w, h, spp;
    std::ifstream dump(filename, std::ifstream::in | std::ifstream::binary);

    dump.read((char*)&w, sizeof(int));
	dump.read((char*)&h, sizeof(int));
	dump.read((char*)&spp, sizeof(int));
	allSamples.resize(w*h*spp);
	dump.read((char*)&(allSamples[0]), allSamples.size() * sizeof(SampleData));
	dump.close();

    RandomParameterFilter rpf(w, h, spp, jouni, allSamples);
    rpf.setQuality(quality);
    rpf.setRandomParams(random_params); //TODO make argument for this
    rpf.Apply();

    TwoDArray<Color> fltImg = TwoDArray<Color>(w, h);
    // Dumping img (and multiply with rho/albedo
    for (uint i=0; i < allSamples.size(); i+=spp) {
    	Color c;
    	for (int j=0; j<spp; j++)
    		for(int k=0; k<3;k++){
    			c[k] += allSamples[i+j].outputColors[k];
    		}
    	c /= spp;
    	fltImg(allSamples[i].x, allSamples[i].y) = c;
    }
    string filenameBase = filename.substr(0, filename.rfind("."));
    WriteImage(filenameBase + "_flt.exr", (float*)fltImg.GetRawPtr(), NULL, w, h,
                     w, h, 0, 0);

    return 0;
}
