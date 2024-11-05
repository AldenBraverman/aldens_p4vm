/*
  ==============================================================================

    MidiHandler_Two.h
    Created: 16 Aug 2024 11:19:15am
    Author:  Alden

  ==============================================================================
*/

#pragma once
/**
 https://www.youtube.com/watch?v=tgf6J8foCiw
 */

#include <JuceHeader.h>
#include <algorithm>

// constexpr int interval = 4;
constexpr int MAX_VOICES = 16;

/*
inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}
*/
class MidiProcessor
{
public:
    void reset() {
        int baseOctave = 0; // Starting from C0
        int numOctaves = 8; // C0 to C7
        
        std::vector<int> baseScale = { 0, 2, 4, 5, 7, 9, 11 }; // Intervals for major scale
        
        for (int octave = baseOctave;octave < numOctaves; ++octave) {
            int baseMidi = 12 * (octave + 1); // Base MIDI number for the given octave (C0=12,C1=24,etc.)
            for (int interval : baseScale) {
                majorScaleMidiNumbers.push_back(baseMidi + interval);
            }
        }
        
        for(int i=0;i<7;i++){
            // DBG(i);
            modeMidiNumbers.push_back(generateModeNotes(i));
        }
        
        /*
        for(int i=0; i<4; i++){
            // DBG(i);
            isVoiceOn[i] = true;
        }
        */
        
    }
    
    std::vector<int> generateModeNotes(int degree) const {
        std::vector<int> modeIntervals = { 0, 2, 4, 5, 7, 9, 11 };  // Copy of base scale intervals
        std::rotate(modeIntervals.begin(), modeIntervals.begin() + degree, modeIntervals.end());

        std::vector<int> notesInMode;
        for (int octave = 0; octave <= 10; ++octave) {  // Covers the full MIDI range
            int baseMidi = 12 * octave;
            for (int interval : modeIntervals) {
                int note = baseMidi + interval;
                if (note >= 0 && note <= 127) {  // Only include valid MIDI numbers
                    notesInMode.push_back(note);
                }
            }
        }
        return notesInMode;
    }
    
    // Check if MIDI number exists in vector
    bool contains(int midi) const {
        return std::find(majorScaleMidiNumbers.begin(), majorScaleMidiNumbers.end(), midi) != majorScaleMidiNumbers.end();
    }
    
    bool modeContains(int midi, int mode) const {
        return std::find(modeMidiNumbers[mode].begin(), modeMidiNumbers[mode].end(), midi) != modeMidiNumbers[mode].end();
    }
    
    void process(juce::MidiBuffer& midiMessages)
    {
        processedBuffer.clear();
        myProcessMidiInput(midiMessages);
        midiMessages.swapWith(processedBuffer);
    }
    
    void myProcessMidiInput(const juce::MidiBuffer& midiMessages)
    {
        juce::MidiBuffer::Iterator it(midiMessages);
        juce::MidiMessage currentMessage;
        int samplePos;
        
        while (it.getNextEvent(currentMessage, samplePos))
        {
            if (currentMessage.isNoteOnOrOff())
            {
                if (currentMessage.isNoteOn())
                {
                    for (int v=0; v < MAX_VOICES; v++) {
                        if (!voices[v]) { // Find First Inactive Voice
                            voices[v] = true; // Activate Voice
                            voiceNotes[v] = currentMessage.getNoteNumber();
                            myPlayNoteOnVoice(v, currentMessage, samplePos);
                            break;
                        }
                    }
                }
                
                if (currentMessage.isNoteOff())
                {
                    for (int v=0; v < MAX_VOICES; v++) {
                        if (voices[v] && myIsNoteOnVoice(v, currentMessage.getNoteNumber())) {
                            voices[v] = false;
                            voiceNotes[v] = -1;
                            myStopNoteOnVoice(v, currentMessage, samplePos);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    void myPlayNoteOnVoice(int voice, juce::MidiMessage messageToTranspose, int samplePos)
    {
        auto oldNoteNum = messageToTranspose.getNoteNumber();
        
        if(isVoiceOn[voice]){
            if(inMajorScale){
                if(modeSelectIndex==3.0f){ // Lydian Mode - need to account for half step
                    // Lydian
                    if(modeContains(oldNoteNum+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex],modeSelectIndex)) {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains True");
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+1+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains False");
                    }
                } else {
                    // Non Lydian
                    if(modeContains(oldNoteNum+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex],modeSelectIndex)) {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains True");
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]-1+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains False");
                    }
                }
            } else {
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex]);
                processedBuffer.addEvent(messageToTranspose, samplePos);
                DBG("No Mode Contains");
            }
        }
    }
    
    void myStopNoteOnVoice(int voice, juce::MidiMessage messageToTranspose, int samplePos)
    {
        auto oldNoteNum = messageToTranspose.getNoteNumber();
        
        if(isVoiceOn[voice]){
            if(inMajorScale){
                if(modeSelectIndex==3.0f){ // Lydian Mode - need to account for half step
                    // Lydian
                    if(modeContains(oldNoteNum+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex],modeSelectIndex)) {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains True");
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+1+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains False");
                    }
                } else {
                    // Non Lydian
                    if(modeContains(oldNoteNum+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex],modeSelectIndex)) {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains True");
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]-1+modeIntervals[modeSelectIndex]);
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                        DBG("Mode Contains False");
                    }
                }
            } else {
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoicePitch[voice]+modeIntervals[modeSelectIndex]);
                processedBuffer.addEvent(messageToTranspose, samplePos);
            }
        }
    }
    
    bool myIsNoteOnVoice(int voice, int note) {
        return voices[voice] && (voiceNotes[voice] == note);
    }
    
    // class member
    juce::MidiBuffer processedBuffer;
    
    bool voices[MAX_VOICES] = { false, false, false, false,
                                false, false, false, false,
                                false, false, false, false,
                                false, false, false, false };
    
    int voiceNotes[MAX_VOICES] = { -1, -1, -1, -1,
                                   -1, -1, -1, -1,
                                   -1, -1, -1, -1,
                                   -1, -1, -1, -1 };
    
    float adjustMasterPitch = 0;
    
    float modeSelectIndex = 0;
    
    bool inMajorScale;
    
    float adjustVoicePitch[16];
    
    bool isVoiceOn[16] = { true, true, true, true,
                           false, false, false, false,
                           false, false, false, false,
                           false, false, false, false };

    std::vector<int> majorScaleMidiNumbers;
    
    std::vector< std::vector<int> > modeMidiNumbers;
    
    std::vector<int> modeIntervals = { 0,2,4,5,7,9,11 };
};

