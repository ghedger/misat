#include <stdio.h>
#include <math.h>
#include <iostream>
#include "FFT.h"
#include "audiofile.h"


void printUsage()
{
    printf( " \
            MISAT\n \
            Musical Instrument Sample Analysis Tool\n \
            Copyright (C) 2015 Greg Hedger\n \
            Usage:\n \
            misat <file>\n\n" );
}


void outputPartials( AudioFile *af, float *pReal, float *pImagOut, float *pRealOut )
{
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
        // Perform FFT Operation

        // Allocate buffers for transform
        float *pReal = new float[ af->getPaddedSampleTot() ];
        float *pImagOut = new float[ af->getPaddedSampleTot() ];
        float *pRealOut = new float[ af->getPaddedSampleTot() ];

        // Initialize FFT engine
        // GPH NOTE: it calls this internally.
        // InitFFT();

        int sampleTot = af->getPaddedSampleTot();  //af->getSampleTot() -
        cout << "\nSAMPLETOT: \n" << sampleTot;

        WindowFunc(6, sampleTot, af->getBuffer() );
        FFT( sampleTot, false, af->getBuffer(), NULL, pImagOut, pRealOut );
        outputPartials( af, pReal, pImagOut, pRealOut );

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
