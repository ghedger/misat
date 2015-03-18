#include "audiofile.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
using namespace std;

unsigned int AudioFile::getPowerOf2Ceiling(unsigned int val)
{
    // Eg. 0x0201df49 should evaluate to 0x04000000
    unsigned int shiftCount = 0;
    while ( !( val & 0x80000000 ) ) {
        val <<= 1;
        shiftCount++;
    }

    shiftCount--;
    return ( 0x80000000 >> shiftCount );
}


void AudioFile::extractHeader( IFFDigest& iff, IFFChunkIterator i ) {
    const char* fmt = (*i).dataPtr();
    // GPH TODO: create offset constants/enums
    this->_format = iff_u16_be( fmt + 2 );
    this->_channelTot = iff_u16_be( fmt );
    this->_bitsPerSample = ( unsigned int ) iff_u16_be( fmt + 6 );
    switch(fmt[9]) {
        case 0x0c:
            this->setSampleRate( 11025 );
            break;
        case 0x0d:
            this->setSampleRate( 22050 );
            break;
        case 0x0e:
            this->setSampleRate( 44100 );
            break;
        case 0x0f:
            this->setSampleRate( 88200 );
            break;
        case 0x10:
            this->setSampleRate( 176400 );
            break;
        case 0x11:
            this->setSampleRate( 352800 );
            break;
        default:
            this->setSampleRate( fmt[9] );
            break;
    }
    cout << "Format " << this->_format << ", " << this->_channelTot << " channels, " << this->_rate << " Hz";
    cout << ", PCM, " << this->_bitsPerSample << " bits/sample";
    cout << "\n";
}

void AudioFile::extractData( IFFDigest& iff, IFFChunkIterator i ) {
    // Re-appropriate i for SSND chunk
    i = iff.ck_find(iff_ckid( "SSND" ) );
    if(i != iff.ck_end()) {
        const unsigned char* psnd = ( const unsigned char * )( *i ).dataPtr() + 8;     // GPH TODO: 12 is header size
        this->_len = (*i).len();
        this->setSampleTot( this->_len / this->_channelTot / ( this->_bitsPerSample >> 3 ) );
        this->setPaddedSampleTot( this->getPowerOf2Ceiling( this->getSampleTot() ) );
        cout << "Samples: " << this->_sampleTot << " Bytes: " << this->_len;
        printf(" (0x%08x)\n", this->_len);
        cout << "Padded Size: " << this->getPowerOf2Ceiling( this->_sampleTot );

        this->_data = new SAMPLETYPE [ this->getPowerOf2Ceiling( this->_sampleTot )];
        if( this->_data ) {
            unsigned int idx = 0, endIdx = this->_sampleTot;
            unsigned int sample;
            unsigned int channel;
            unsigned int bits;
            while( idx < endIdx ) {
                sample = 0;
                bits = this->_bitsPerSample;
                while( bits ) {
                    sample <<= 8;
                    sample |= ( unsigned int ) *( ( unsigned int * ) psnd ) & (unsigned int) 0xff;
                    psnd++;
                    bits -= 8;
                }

                // expand
                sample <<= ( 32 - this->_bitsPerSample );

                channel = this->_channelTot;
                while( --channel ) {
                    psnd += this->_bitsPerSample >> 3;
                }

                this->_data[idx] = ( SAMPLETYPE ) ( ( int ) sample ) / ( double ) 0x7fffffff;
                idx++;
            }

            int i;
            cout << "\nFirst few samples:";
            for( i = 0; i < 8; i++ ) {
                printf( "%f  ", this->_data[ i ] );
            }
            cout << "\n";

        } else {
            cout << "Buffer allocation error: " << this->_len << "bytes.\n";
        }
    }
}

bool AudioFile::load(const char *filename)
{
    bool ret = false;
    int fd = open( filename, O_RDONLY );
    if( 0 < fd ) {
        struct stat stbuf;
        fstat(fd, &stbuf);
        char* data = ( char * ) mmap( 0, stbuf.st_size, PROT_READ, MAP_SHARED, fd, 0 );
        IFFDigest iff( data, stbuf.st_size );
        IFFChunkIterator i=iff.ck_find(iff_ckid("COMM"));
        if( i == iff.ck_end() ) {
            IFFChunkIterator i=iff.ck_find(iff_ckid("fmt "));

            if(i==iff.ck_end()) {
                cerr << filename << " is not a valid WAV/AIFF file.\n";
                ret = true;
            } else {
                // GPH TODO: Support WAV
                const char* fmt = (*i).dataPtr();
                unsigned short wformat = iff_u16_le( fmt );
                unsigned short wchannels = iff_u16_le( fmt+2 );
                unsigned int rate = iff_u32_le( fmt + 4 );
                cout << filename << ": format " << wformat <<", "<<wchannels<<" channels, "<< this->getSampleRate() << " Hz";
                if(wformat == 1) { // It's a PCM file
                    cout<<", PCM, "<<(iff_u16_le(fmt+14))<<" bits/sample";
                }
                cout<<'\n';
            }
        } else {
            this->extractHeader( iff, i );
            this->extractData( iff, i );
        }

        munmap(data, stbuf.st_size);
        close(fd);
    } else {
        ret = true;
    }
    return ret;
}

