#include "WaveIn.h"

void WAVE_RIFF::ReadFromStream(FILE* file)
{
    while (fread(this, 1, 4, file) == 4)
        if (RIFF[0] == 'R' && RIFF[1] == 'I' && RIFF[2] == 'F' && RIFF[3] == 'F')
            if (fread(&this->ChunkSize, 1, 8, file) == 8)
                if (IsValid())
                    break;
}

bool WAVE_RIFF::IsValid()
{
    if (RIFF[0] != 'R' || RIFF[1] != 'I' || RIFF[2] != 'F' || RIFF[3] != 'F') return false;
    if (WAVE[0] != 'W' || WAVE[1] != 'A' || WAVE[2] != 'V' || WAVE[3] != 'E') return false;
    return true;
}

void WAVE_FMT::ReadFromStream(FILE* file)
{
    while (fread(this, 1, 4, file) == 4)
        if (IsValid())
            if (fread(&this->Subchunk1Size, 1, sizeof(WAVE_FMT) - 4, file) == sizeof(WAVE_FMT) - 4)
                break;
}

bool WAVE_FMT::IsValid()
{
    if (fmt[0] != 'f' || fmt[1] != 'm' || fmt[2] != 't' || fmt[3] != ' ') return false;
    return true;
}

WAVE_DATA::~WAVE_DATA()
{
    delete[] AudioData;
}

void WAVE_DATA::ReadFromStream(FILE* file)
{
    while (fread(this, 1, 4, file) == 4)
        if (IsValid())
        {
            fread(&this->Subchunk2Size, 1, 4, file);
            AudioData = new char[Subchunk2Size];
            size_t read = fread(AudioData, 1, Subchunk2Size, file);
            break;
        }
}

bool WAVE_DATA::IsValid()
{
    if (data[0] != 'd' || data[1] != 'a' || data[2] != 't' || data[3] != 'a') return false;
    return true;
}

WAVE::WAVE(std::string path)
{
    FILE* file(nullptr);
    errno_t err = fopen_s(&file, path.c_str(), "rb");
    if (file == nullptr) return;
    riff.ReadFromStream(file);
    format.ReadFromStream(file);
    data.ReadFromStream(file);
    fclose(file);
}

bool WAVE::IsValid()
{
    return riff.IsValid() && format.IsValid() && data.IsValid();
}
