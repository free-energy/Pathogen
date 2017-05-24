#include "WaveRIFFParser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


WaveRIFFParser::WaveRIFFParser()
{
	rawData = NULL;
	rawData16 = NULL;
	rawDataF = NULL;
}


WaveRIFFParser::~WaveRIFFParser()
{
	if (rawData)
	{
		free(rawData);
		rawData = NULL;
		rawData16 = NULL;
		rawDataF = NULL;
	}
}

WaveRIFFParser::eErrorCodes WaveRIFFParser::parseWave(FILE* const f )
{

	uint8_t headerBuffer[TEMP_BUFF_SIZE];
	uint16_t chunkSize; 

	if (fread(&headerBuffer[0], 1, RIFF_HEADER_SIZE, f) != RIFF_HEADER_SIZE)
	{
		return READ_ERR;
	}

	//Check for 'WAVE' ID
	if (DWORD(&headerBuffer[RIFF_FMT_OFFSET]) != FCC('W', 'A', 'V', 'E'))
	{
		return INVALID_FILE;
	}

	//Determine Format
	if (fread(&headerBuffer[0], 1, CHUNK_HEADER_SIZE, f) != CHUNK_HEADER_SIZE)
	{
		return READ_ERR;
	}

	//Check for 'WAVE' ID
	if (DWORD(&headerBuffer[0]) != FCC('f', 'm', 't', ' '))
	{
		return INVALID_FILE;
	}

	chunkSize = DWORD(&headerBuffer[4]);
	if ((chunkSize < 16) || (chunkSize > 128))
	{
		return INVALID_FILE;
	}

	if (fread(&headerBuffer[0], 1, chunkSize, f) != chunkSize)
	{
		return READ_ERR;
	}

	Format = WORD(&headerBuffer[FORMAT_OFFSET]);
	if ((Format != WAV_PCM) && (Format != WAV_FLOAT))
	{
		return INVALID_FILE;
	}

	ChannelCount = WORD(&headerBuffer[CHANNEL_COUNT_OFFSET]);
	if ((ChannelCount != 1) && (ChannelCount != 2))
	{
		return INVALID_FILE;
	}

	SampleRate = DWORD(&headerBuffer[SAMPLE_RATE_OFFSET]);
	if ((SampleRate != 44100) && (SampleRate != 48000) && (SampleRate != 96000))
	{
		return INVALID_FILE;
	}

	FrameSize = WORD(&headerBuffer[BYTES_PER_FRAME_OFFSET]);
	BitsPerSample = WORD(&headerBuffer[BITS_PER_SAMPLE_OFFSET]);

	//Find the data section
	for (;;)
	{
		if (fread(&headerBuffer[0], 1, CHUNK_HEADER_SIZE, f) != CHUNK_HEADER_SIZE)
		{
			return INVALID_FILE;
		}

		//Check for 'data' ID
		if (DWORD(&headerBuffer[0]) == FCC('d', 'a', 't', 'a'))
		{
			DataSize = DWORD(&headerBuffer[4]);
			rawData = (uint8_t*)malloc(DataSize);
			if (fread(rawData, 1, DataSize, f) != DataSize)
			{
				return READ_ERR;
			}

			if (Format == WAV_PCM)
			{
				rawData16 = (int16_t*)rawData;
			}

			if (Format == WAV_FLOAT)
			{
				rawDataF = (float*)rawData;
			}

			FrameCount = DataSize / (ChannelCount * (BitsPerSample / 8));

			break;
		}
		else
		{
			chunkSize = DWORD(&headerBuffer[4]);
			fpos_t pos;
			fgetpos(f, &pos);
			fseek(f, pos + chunkSize, 0);

		}
	}

	return SUCCESS;
	
}



WaveRIFFParser::eErrorCodes WaveRIFFParser::importWave(const char* file)
{
	FILE* f = fopen(file, "rb");
	if (f == NULL)
	{
		return OPEN_ERR;
	}
	rewind(f);

	if (rawData)
	{
		free(rawData);
		rawData = NULL;
		rawData16 = NULL;
		rawDataF = NULL;
	}


	eErrorCodes ret = parseWave(f);
	fclose(f);

	return ret;
}


