#include "iffdigest.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
using namespace std;

void parseAIFF(IFFDigest& iff, IFFChunkIterator i, const char *filename) {
    const char* fmt = (*i).dataPtr();
    unsigned short wformat = iff_u16_be(fmt+2);
    unsigned short wchannels = iff_u16_be(fmt);
    unsigned int smplrate = 0;
    unsigned short bitsPerSample = iff_u16_be(fmt+6);
    switch(fmt[9]) {
        case 0x0c:
            smplrate = 11025;
            break;
        case 0x0d:
            smplrate = 22050;
            break;
        case 0x0e:
            smplrate = 44100;
            break;

        default:
            break;
    }
    cout<<filename<<": format "<<wformat<<", "<<wchannels<<" channels, "<<smplrate<<" Hz";
    if(wformat == 1) { // PCM file
        cout<<", PCM, "<< bitsPerSample <<" bits/sample";
    }
    cout<<'\n';

    // Re-appropriate i for SSND chunk
    i=iff.ck_find(iff_ckid("SSND"));
    if(i != iff.ck_end()) {
        unsigned int len = (*i).len();
        unsigned int sampleTot = len / wchannels / (bitsPerSample >> 3);
        cout << "Samples: " << sampleTot << " Bytes: " << len;
        printf(" (0x%08x)\n", len);
    }
}

main(int argc, char* argv[])
{
    if(argc<2) {
        cerr<<"usage: "<<argv[0]<<" filename\n";
        return 2;
    }
    int fd = open(argv[1], O_RDONLY);
    struct stat stbuf;
    fstat(fd, &stbuf);
    char* data = (char*)mmap(0, stbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    IFFDigest iff(data, stbuf.st_size);
    IFFChunkIterator i=iff.ck_find(iff_ckid("COMM"));
    if(i==iff.ck_end()) {
        IFFChunkIterator i=iff.ck_find(iff_ckid("fmt "));

        if(i==iff.ck_end()) {
            cerr<<argv[1]<<" is not a valid WAV file.\n";
            return 1;
        } else {
            const char* fmt = (*i).dataPtr();
            unsigned short wformat = iff_u16_le(fmt);
            unsigned short wchannels = iff_u16_le(fmt+2);
            unsigned int smplrate = iff_u32_le(fmt+4);
            cout<<argv[1]<<": format "<<wformat<<", "<<wchannels<<" channels, "<<smplrate<<" Hz";
            if(wformat == 1) { // PCM file
                cout<<", PCM, "<<(iff_u16_le(fmt+14))<<" bits/sample";
            }
            cout<<'\n';
        }
    } else {
        parseAIFF(iff,i,argv[1]);
    }

    munmap(data, stbuf.st_size);
    close(fd);
    return 0;
}

