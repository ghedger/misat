#include <stdio.h>
#include <math.h>
#include <iostream>
#include "FFT.h"
#include "audiofile.h"


void printUsage()
{
    printf( " \
HarmPlot\n \
Harmonic Plotter for AIFF/WAV files\n \
Copyright (C) 2015 Greg Hedger\n \
Usage:\n \
harmplot <file>\n\n" );

}

int main( int argc, const char * argv[])
{
    if (argc < 2) {
        printUsage();
        return -1;
    }
    AudioFile *af = new AudioFile();
    if( !af->load( argv[1] ) )
    {
        // Allocate buffers for transform
        float *pReal = new float[ af->getSampleTot() * 2 ];
        float *pImagOut = new float[ af->getSampleTot() *2 ];
        float *pRealOut = new float[ af->getSampleTot() *2 ];

        // Initialize FFT engine
        // GPH NOTE: it calls this internally.
        // InitFFT();

        int sampleTot = af->getPaddedSampleTot();  //af->getSampleTot() -
        cout << "\nSAMPLETOT: \n" << sampleTot;

        WindowFunc(6, sampleTot, af->getBuffer() );
        FFT( sampleTot, false, af->getBuffer(), NULL, pImagOut, pRealOut );

        double step = ( ( double ) af->getPaddedSampleTot() / 2.0 ) / af->getSampleRate();
        double at = 0;
        double hz = 0.0;
        double hzStep = ( af->getSampleRate() / 2 ) / ( double ) af->getPaddedSampleTot() / 2.0;
        unsigned int i;
        double end = af->getPaddedSampleTot() / 2.0;
        while( at < end ) {
            i = ( unsigned int ) at;
            printf( "%05.2fHz:\t% 2.4f\n", hz, log(sqrt(pRealOut[i] * pRealOut[i] + pImagOut[i] * pImagOut[i] ) ) );
            at += 1.0;
            hz += hzStep * 4;
        }


        // Clean up
        DeinitFFT();
        delete[] pReal;
        delete[] pImagOut;
        delete[] pRealOut;
    } else {
        cout << "\nTrouble loading file...\n";
    }
    delete af;
    return 0;
}
