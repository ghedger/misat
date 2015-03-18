#pragma once
#include "iffdigest.h"

typedef float SAMPLETYPE;
class AudioFile {
    public:
     AudioFile()
    {
        _data = 0;
    }
    ~AudioFile()
    {
        // clean up
        if( _data ) {
            delete[] _data;
            _data = 0;
        }
    };

    // public API
    void extractHeader(IFFDigest& iff, IFFChunkIterator i);
    void extractData(IFFDigest& iff, IFFChunkIterator i);
    bool load(const char *);
    SAMPLETYPE *getBuffer() { return _data; }

    // access functions
    unsigned int getSampleTot() { return _sampleTot; }
    void setSampleTot( unsigned int sampleTot ) { this->_sampleTot = sampleTot; }
    unsigned int getPaddedSampleTot() { return _paddedSampleTot; }
    void setPaddedSampleTot( unsigned int sampleTot ) { this->_paddedSampleTot = sampleTot; }
    unsigned int getSampleRate() { return _rate; }
    void setSampleRate( unsigned int rate ) { this->_rate = rate; }

    protected:
    unsigned int getPowerOf2Ceiling(unsigned int val);

    SAMPLETYPE *_data;
    unsigned int _rate;
    unsigned int _format;
    unsigned int _bitsPerSample;
    unsigned int _channelTot;
    unsigned int _sampleTot;
    unsigned int _paddedSampleTot;
    unsigned int _len;
};
