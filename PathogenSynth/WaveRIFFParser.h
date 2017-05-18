#pragma once

#include <stdint.h>
#include <stdio.h>


class WaveRIFFParser
{


public:

	typedef enum {
		UNSUPPORTED = 0,
		WAV_PCM = 1,
		WAV_FLOAT = 3,
	} eWavFormat;

	typedef enum
	{
		SUCCESS,
		OPEN_ERR,
		READ_ERR,
		INVALID_FILE,
	} eErrorCodes;



	WaveRIFFParser();
	virtual ~WaveRIFFParser();

	eErrorCodes importWave(const char* file);

	uint16_t getFormat(void) { return Format; }
	uint16_t getChannelCount(void) { return ChannelCount; }
	uint32_t getFrameCount(void) { return FrameCount; }
	uint32_t getSampleRate(void) { return SampleRate; }

	void* getBuf(void) { return rawData; }


protected:

	enum
	{
		TEMP_BUFF_SIZE = 64,
		RIFF_FMT_OFFSET = 8,
		RIFF_HEADER_SIZE = 12,
		CHUNK_HEADER_SIZE = 8,

		FORMAT_OFFSET = 0,
		CHANNEL_COUNT_OFFSET = 2,
		SAMPLE_RATE_OFFSET = 4,
		BYTES_PER_SEC_OFFSET = 8,
		BYTES_PER_FRAME_OFFSET = 12,
		BITS_PER_SAMPLE_OFFSET = 14,
	};

	uint32_t SampleRate;
	uint32_t SampleLength;
	uint16_t  ChannelCount;
	uint16_t  BitsPerSample;
	uint16_t FrameSize;

	uint16_t  Format;

	uint32_t DataSize;
	uint32_t FrameCount;

	uint8_t* rawData;
	int16_t* rawData16;
	float* rawDataF;

	uint32_t FCC(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
		return (((uint32_t)b4 << 24) + ((uint32_t)b3 << 16) + ((uint16_t)b2 << 8) + (uint8_t)b1);}

	uint32_t DWORD(uint8_t* b1) {
		return (((uint32_t)b1[3] << 24) + ((uint32_t)b1[2] << 16) + ((uint16_t)b1[1] << 8) + (uint8_t)b1[0]);
	}

	uint16_t WORD(uint8_t* b1) {
		return uint16_t(b1[0]) + uint16_t(b1[1] << 8); }

	WaveRIFFParser::eErrorCodes parseWave(FILE* const f);
};

