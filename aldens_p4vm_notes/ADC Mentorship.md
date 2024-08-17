### 07/09/2024 - First 1on1

Goals:
- Introduction

Paulo Chiliguano
- Electronics Engineer
- Yamaha PSS-680
- **Telecoms teaches DSP**
	- Speech Processing is studied in telecoms
- GA tech survey of music technology
	- https://pe.gatech.edu/courses/survey-music-technology-soup
	- https://www.classcentral.com/course/musictech-605
- EarSketch
	- https://earsketch.gatech.edu/landing/#/
- Queen Mary University - London
- Paulo wants to master C++
	- realtime approach to doing things

Dafx - J.D. Reiss
- **Working with the Web Audio API**
- https://www.eecs.qmul.ac.uk/~josh/publications.htm

Juan Gil - Audio Effects
- https://github.com/juandagilc/Audio-Effects

Voloco Producer
- 1.5 years to recreate
- https://resonantcavity.com/plugin.html

Spotify Research github
- Python package?
- https://github.com/spotify-research
- https://research.atspotify.com/

Muse Group
- Audacity
- https://www.mu.se/

Viator DSP
- https://viatordsp.gumroad.com/


Foleys Finest Audio
- https://foleysfinest.com/

https://dspguide.com/
- https://dspguide.com/ch2/1.htm
	- "Chapter 2: Statistics, Probability and Noise"
	- Could have some Financial Engineering connections (Stochastic/Monte Carlo)


### 08/12/2024 - Code Review

Goals:
- Implement more elegant solution for representing voices
	- Need to define floats/bools for each individual voice. If I want 16 voices, I would need 16 floats/bools for each voice.
		- Is this actually the best way to do it?
	- Chronology of variables being checked/bools (highest priority variable - voice sum should have the higher precedence?)
		- https://en.wikipedia.org/wiki/Karnaugh_map
		- https://www.boolean-algebra.com/kmap/
		- https://www.allaboutcircuits.com/textbook/digital/chpt-8/larger-5-6-variable-karnaugh-maps/
			- ![[Pasted image 20240812182938.png]]
		- Switch/case
		- 16 if/else for each case
		- How fast are changes happening?
			- Latch
				- How much from the voice active change should be maintained?
				- consider comparing timestamps (avoid)
		- Create bool check voices function that is called in noteOn and noteOff
		- struct VoiceState
			- vector_bool **SEE IMPLEMENTATION**
				- https://en.cppreference.com/w/cpp/container/vector_bool
	- "I have four bools I need to check in c++, how would I use a kmap to efficiently check the state of the bools"
		- https://chatgpt.com/c/aee21d0b-8ad8-4c0a-93df-f76d4c636133
- Data structure for transforming midi
	- If I read in a midi note and need to check to see if it exists in a scale, and transform the note if needed, what is the most optimal way to do this?
		- std::map to start (JUCE has a map)
			- https://en.cppreference.com/w/cpp/container/map
			- https://docs.juce.com/master/group__juce__core-maths.html#ga8acdd3d518517bd5e3c0bd1922218bf9
				- lineaer jmap
			- NormalisableRange (implementation)
				- std::log to transform
				- cmath library
			- need frequency value equivalent to midi note
			- add struct to midi note
	- dictionaries and hash functions are similar
		- think about parameter layout
			- maybe for presets
		- UI
			- UI components have UUIDs
				- If UI is growing too much
					- Maybe use hash table
						- Usage is similar to dictionary
- Whats hip in audio right now?
	- LinkedIn for plugins
	- https://www.linkedin.com/feed/update/urn:li:activity:7225105239605940224/
	- https://www.youtube.com/live/PFBbUhhrWF4
	- https://x.com/csteinmetz1
	- AI
		- Source seperation
			- Older recordings
			- Recent recordings wont need it
				- More satisfactory for recent recordings
		- Generative AI
			- Results using same/predictable
			- Music is exciting when is different
				- Hence, analog
				- Mimic artifacts from variable components from solid equipment
				- FFB filter moog
					- Moog Filter bank
					- Gaussian noise / dithering
						- Psuedo random
						- Need less compute intensity and faster
							- Apply AI
		- Modulation effects with neural networks
		- https://arxiv.org/pdf/1809.04281
