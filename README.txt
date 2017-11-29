MISAT
Musical Instrument Sample Analysis Tool

Purpose:
To provide detailed information about musical instrument samples in PCM format for the purpose of generalizing and building small, high-quality loop-based instruments.  Can also be used for fingerprinting.

Todo:
    General:
    - Bring code up to C++11 standards (DONE).

    Features:
    - Extend loaded sample allocation space to nearest power-of-two ceiling (DONE).
    - Support WAV as well as AIFF.
    - Fix core dump on attempt to load nonextant file (DONE).
    - Create a Sample class (DONE)
        o length
        o recording frequency
    - Replace hardcoded test code with references to this Sample class (DONE).
    - Fundamental detection
        o Use FFT to aid in this
        o Use a window, starting at middle of file, and slide the window until
            a corresponding pattern of minimal difference is found.
    - Sound playback
        o Raw sample
        o Specific frequencies
    - Fourier recombination
        o Reverse transform ("Straight up")
        o Selective recombination, with ability to exclude certain frequencies and ranges.
    - A command-line interface.
    - Job threads (fork)

