#pragma once
#include <cstdint>
#include <fstream>

struct WAVE_RIFF
{
    uint8_t         RIFF[4];        // RIFF Header Magic header
    uint32_t        ChunkSize;      // RIFF Chunk Size
    uint8_t         WAVE[4];        // WAVE Header

    void ReadFromStream(FILE* file);
	bool IsValid();
};

struct WAVE_FMT
{
    uint8_t         fmt[4];         // FMT header
    uint32_t        Subchunk1Size;  // Size of the fmt chunk
    uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t        bytesPerSec;    // bytes per second
    uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t        bitsPerSample;  // Number of bits per sample


    void ReadFromStream(FILE* file);
    bool IsValid();
};

struct WAVE_DATA
{
    uint8_t         data[4];        // "data"  string
    uint32_t        Subchunk2Size;  // Sampled data length
    void*           AudioData = nullptr;

    ~WAVE_DATA();

    void ReadFromStream(FILE* file);
    bool IsValid();
};

struct WAVE
{
    WAVE_RIFF riff;
    WAVE_FMT format;
    WAVE_DATA data;

    WAVE(std::string path);

    bool IsValid();
};