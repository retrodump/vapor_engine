/************************************************************************
*
* vaporEngine (2008-2009)
*
*	<http://www.portugal-a-programar.org>
*
************************************************************************/

#pragma once

#include "vapor/Platform.h"
#include "vapor/resources/Resource.h"
#include "vapor/resources/ResourceManager.h"

namespace vapor {
	namespace resources {

//-----------------------------------//

/**
 * Possible formats for audio.
 */

namespace SoundFormat 
{
	enum Enum 
	{
		Mono8,
		Mono16,
		Stereo8,
		Stereo16
	};

	const std::string getString(Enum e);
}

//-----------------------------------//

/**
 * Represents a sound in the engine. 
 * A sound holds uncompressed audio data that can be played back.
 */

class VAPOR_API Sound : public Resource
{
public:

	Sound(const SoundFormat::Enum format, const int frequency, const std::vector<byte>& data);
	virtual ~Sound();
	
	// Gets the frequency of the sound.
	const int getFrequency() const { return frequency; }

	// Returns the format of the sound.
	SoundFormat::Enum getFormat() const { return format; }

	// Gets a pointer to the buffer containing the data.
	const std::vector<byte>& getBuffer() const { return dataBuffer; };

	// Change the buffer containing the audio data.
	void setBuffer(const std::vector<byte>& data) { dataBuffer = data; }

	// Return the proper resource group for this resource.
	ResourceGroup::Enum getResourceGroup() const { return ResourceGroup::Audio; }

private:

	// sound frequency
	int frequency;

	// sound format
	SoundFormat::Enum format;

	// holds the sound data
	std::vector<byte> dataBuffer;
};

//-----------------------------------//

typedef tr1::shared_ptr< Sound > SoundPtr;

//-----------------------------------//

} } // end namespaces