#include "..\..\..\Header Files\engine\utils\WavFile.h"

namespace engine
{
	//---Private attributes---
	//---Public attributes---
	//---Private methods---
	//---Public methods---

	/**
	 * Constructs the WavFile object
	 */
	WavFile::WavFile(std::string argFileName)
	{
		Logger::Log("WavFile: Initializing", Logger::INFO, __FILE__, __LINE__);

		FILE* pFile = std::fopen(argFileName.c_str(), "rb");
		std::fread(&this->wavHeader, sizeof(WavChunkDescription), 1, pFile);

		if(	this->wavHeader.type[0] != 'R' ||
			this->wavHeader.type[1] != 'I' || 
			this->wavHeader.type[2] != 'F' || 
			this->wavHeader.type[3] != 'F')
		{
			Logger::Log("WavFile: Not RIFF", Logger::WARNING, __FILE__, __LINE__);
		}

		if(	this->wavHeader.desc[0] != 'W' ||
			this->wavHeader.desc[1] != 'A' || 
			this->wavHeader.desc[2] != 'V' || 
			this->wavHeader.desc[3] != 'E')
		{
			Logger::Log("WavFile: Not WAVE", Logger::WARNING, __FILE__, __LINE__);
		}

		if(	this->wavHeader.format[0] != 'f' ||
			this->wavHeader.format[1] != 'm' || 
			this->wavHeader.format[2] != 't' || 
			this->wavHeader.format[3] != ' ')
		{
			Logger::Log("WavFile: Not fmt", Logger::WARNING, __FILE__, __LINE__);
		}

		// Set the frequency
		this->frecuency = this->wavHeader.samplePerSec;

		// Set the format
		this->format = 0;
		if(this->wavHeader.bitsPerSample == 8)
		{
			if(this->wavHeader.channels == 1)
			{
				this->format = AL_FORMAT_MONO8;
			}
			else if(this->wavHeader.channels == 2)
			{
				this->format = AL_FORMAT_STEREO8;
			}
		}
		else if(this->wavHeader.bitsPerSample == 16)
		{
			if(this->wavHeader.channels == 1)
			{
				this->format = AL_FORMAT_MONO16;
			}
			else if(this->wavHeader.channels == 2)
			{
				this->format = AL_FORMAT_STEREO16;
			}
		}

		WavDataChunkDescription wavDataChunk;
		wavDataChunk.size = 0;
		do
		{
			// Skip ahead
			std::fseek(pFile, wavDataChunk.size, SEEK_CUR);
			// Read the data header
			std::fread(&wavDataChunk, sizeof(WavDataChunkDescription), 1, pFile);
		}
		while(	!std::feof(pFile) &&
				(wavDataChunk.type[0] != 'd' ||
				 wavDataChunk.type[1] != 'a' ||
				 wavDataChunk.type[2] != 't' ||
				 wavDataChunk.type[3] != 'a'));

		// Set the buffer size
		this->bufferSize = wavDataChunk.size;
		// Size per sample in the buffer
		long sizePerSample;
		if(this->wavHeader.bitsPerSample == 16)
		{
			sizePerSample = sizeof(int);
			this->buffer = new int[this->bufferSize];
		}
		// Default to 8bit
		else
		{
			sizePerSample = sizeof(unsigned char);
			this->buffer = new unsigned char[this->bufferSize];
		}
		std::fread(this->buffer, sizePerSample, this->bufferSize, pFile);
		
		Logger::Log("WavFile: Finished", Logger::INFO, __FILE__, __LINE__);
	}

	/**
	 * Destructs the Scene object.
	 */
	WavFile::~WavFile()
	{
		Logger::Log("WavFile: Disposing", Logger::INFO, __FILE__, __LINE__);
		this->CleanUp();
	}

	/**
	 * Lazy cleanup method for destructing this object.
	 * @return		void
	 */
	void WavFile::CleanUp()
	{
	}
	
	/**
	 * Gets the buffer size
	 * @return		unsigned long
	 */
	unsigned long WavFile::GetBufferSize()
	{
		return this->bufferSize;
	}

	/**
	 * Gets the buffer
	 * @return		void*
	 */
	void* WavFile::GetBuffer()
	{
		return this->buffer;
	}

	/**
	 * Gets the frecuency
	 * @return		ALuint
	 */
	ALuint WavFile::GetFrecuency()
	{
		return this->frecuency;
	}

	/**
	 * Gets the format
	 * @return		ALuint
	 */
	ALuint WavFile::GetFormat()
	{
		return this->format;
	}
}