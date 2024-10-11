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
        
        
        for(int i=0; i<4; i++){
            // DBG(i);
            isVoiceOn[i] = true;
        }
        
    }
    
    // Check if MIDI number exists in vector
    bool contains(int midi) const {
        return std::find(majorScaleMidiNumbers.begin(), majorScaleMidiNumbers.end(), midi) != majorScaleMidiNumbers.end();
    }
    
    void process(juce::MidiBuffer& midiMessages)
    {
        processedBuffer.clear();
        // processMidiInput(midiMessages);
        myProcessMidiInput(midiMessages);
        midiMessages.swapWith(processedBuffer);
    }
    
    void processMidiInput(const juce::MidiBuffer& midiMessages)
    {
        juce::MidiBuffer::Iterator it(midiMessages);
        juce::MidiMessage currentMessage;
        int samplePos;
        
        while (it.getNextEvent(currentMessage, samplePos))
        {
            // DBG(currentMessage.getDescription());
            // DBG("Two - "+juce::String(samplePos));
            if (currentMessage.isNoteOnOrOff())
            {
                // currentMessage.setNoteNumber(50);
                // DBG("OG Note On/Off - "+currentMessage.getDescription());
                
                // Make a copy of currentMessage
                // auto transposedMessage = currentMessage;
                // auto oldNoteNum = transposedMessage.getNoteNumber();
                // transposedMessage.setNoteNumber(oldNoteNum + interval);
                
                // processedBuffer.addEvent(transposedMessage, samplePos);
                // addTransposedNote(currentMessage, samplePos); // THIS IS THE LINE
            }
            // Harmonize with original note, remove if we just want transposed note
            // processedBuffer.addEvent(currentMessage, samplePos);
        }
    }
    
    void addTransposedNote(juce::MidiMessage messageToTranspose, int samplePos)
    {
        auto oldNoteNum = messageToTranspose.getNoteNumber();
        messageToTranspose.setNoteNumber(oldNoteNum + adjustMasterPitch);
        
        processedBuffer.addEvent(messageToTranspose, samplePos);
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
                // DBG("New Note On/Off -"+currentMessage.getDescription());
                if (currentMessage.isNoteOn())
                {
                    // DBG("Note On Message on Note "+juce::String(currentMessage.getNoteNumber()));
                    for (int v=0; v < MAX_VOICES; v++) {
                        // DBG(juce::String(v)+" "+BoolToString(voices[v]));
                        if (!voices[v]) { // Find First Inactive Voice
                            // DBG(juce::String(v)+" is the first inactive voice");
                            voices[v] = true; // Activate Voice
                            voiceNotes[v] = currentMessage.getNoteNumber();
                            myPlayNoteOnVoice(v, currentMessage, samplePos);
                            // return;
                            break;
                        }
                    }
                }
                
                if (currentMessage.isNoteOff())
                {
                    // DBG("Note Off Message on Note "+juce::String(currentMessage.getNoteNumber()));
                    for (int v=0; v < MAX_VOICES; v++) {
                        // DBG(juce::String(v)+" "+BoolToString(voices[v])+" "+BoolToString(myIsNoteOnVoice(v, currentMessage.getNoteNumber())));
                        if (voices[v] && myIsNoteOnVoice(v, currentMessage.getNoteNumber())) {
                            // DBG(juce::String(v)+" is the voice to be deactivated");
                            voices[v] = false;
                            voiceNotes[v] = -1;
                            myStopNoteOnVoice(v, currentMessage, samplePos);
                            // return;
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
        
        switch (voice) {
            case 0:
                // voice 1
                if(isVoiceOn[0]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceOnePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }

                break;
                
            case 1:
                // voice 2
                if(isVoiceOn[1]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceTwoPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 2:
                // voice 3
                if(isVoiceOn[2]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceThreePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 3:
                // voice 4
                if(isVoiceOn[3]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFourPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 4:
                // voice 5
                if(isVoiceOn[4]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFivePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFivePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFivePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFivePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 5:
                // voice 6
                if(isVoiceOn[5]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceSixPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 6:
                // voice 7
                if(isVoiceOn[6]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceSevenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSevenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSevenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSevenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 7:
                // voice 8
                if(isVoiceOn[7]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceEightPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceEightPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceEightPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceEightPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 8:
                // voice 9
                if(isVoiceOn[8]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceNinePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceNinePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceNinePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceNinePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 9:
                // voice 10
                if(isVoiceOn[9]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceTenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 10:
                // voice 11
                if(isVoiceOn[10]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceElevenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceElevenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceElevenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceElevenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 11:
                // voice 12
                if(isVoiceOn[11]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceTwelvePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwelvePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwelvePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwelvePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 12:
                // voice 13
                if(isVoiceOn[12]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceThirteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThirteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThirteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThirteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 13:
                // voice 14
                if(isVoiceOn[13]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFourteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 14:
                // voice 15
                if(isVoiceOn[14]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFifteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFifteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFifteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFifteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 15:
                // voice 16
                if(isVoiceOn[15]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceSixteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
        }
    }
    
    void myStopNoteOnVoice(int voice, juce::MidiMessage messageToTranspose, int samplePos)
    {
        auto oldNoteNum = messageToTranspose.getNoteNumber();
        
        switch (voice) {
            case 0:
                // voice 1
                if(isVoiceOn[0]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceOnePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }

                break;
                
            case 1:
                // voice 2
                if(isVoiceOn[1]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceTwoPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 2:
                // voice 3
                if(isVoiceOn[2]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceThreePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 3:
                // voice 4
                if(isVoiceOn[3]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFourPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 4:
                // voice 5
                if(isVoiceOn[4]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFivePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFivePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFivePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFivePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 5:
                // voice 6
                if(isVoiceOn[5]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceSixPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 6:
                // voice 7
                if(isVoiceOn[6]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceSevenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSevenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSevenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSevenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 7:
                // voice 8
                if(isVoiceOn[7]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceEightPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceEightPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceEightPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceEightPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 8:
                // voice 9
                if(isVoiceOn[8]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceNinePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceNinePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceNinePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceNinePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 9:
                // voice 10
                if(isVoiceOn[9]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceTenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 10:
                // voice 11
                if(isVoiceOn[10]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceElevenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceElevenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceElevenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceElevenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 11:
                // voice 12
                if(isVoiceOn[11]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceTwelvePitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwelvePitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwelvePitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwelvePitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 12:
                // voice 13
                if(isVoiceOn[12]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceThirteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThirteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThirteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThirteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
                
            case 13:
                // voice 14
                if(isVoiceOn[13]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFourteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 14:
                // voice 15
                if(isVoiceOn[14]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceFifteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFifteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFifteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFifteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
            
            case 15:
                // voice 16
                if(isVoiceOn[15]){
                    if(inMajorScale){
                        if (contains(oldNoteNum+adjustVoiceSixteenPitch)) {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixteenPitch);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        } else {
                            messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixteenPitch+1);
                            // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                            
                            processedBuffer.addEvent(messageToTranspose, samplePos);
                        }
                    } else {
                        messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceSixteenPitch);
                        // messageToTranspose.setChannel((int)assignVoiceOneMidiChannel);
                        
                        processedBuffer.addEvent(messageToTranspose, samplePos);
                    }
                }
                break;
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
    
    bool inMajorScale;
    
    float adjustVoiceOnePitch = 0;
    float adjustVoiceTwoPitch = 0;
    float adjustVoiceThreePitch = 0;
    float adjustVoiceFourPitch = 0;
    float adjustVoiceFivePitch = 0;
    float adjustVoiceSixPitch = 0;
    float adjustVoiceSevenPitch = 0;
    float adjustVoiceEightPitch = 0;
    float adjustVoiceNinePitch = 0;
    float adjustVoiceTenPitch = 0;
    float adjustVoiceElevenPitch = 0;
    float adjustVoiceTwelvePitch = 0;
    float adjustVoiceThirteenPitch = 0;
    float adjustVoiceFourteenPitch = 0;
    float adjustVoiceFifteenPitch = 0;
    float adjustVoiceSixteenPitch = 0;
    
    bool isVoiceOn[16];
    
    /*
    float assignVoiceOneMidiChannel = 1;
    float assignVoiceTwoMidiChannel = 1;
    float assignVoiceThreeMidiChannel = 1;
    float assignVoiceFourMidiChannel = 1;
    float assignVoiceFiveMidiChannel = 1;
    float assignVoiceSixMidiChannel = 1;
    float assignVoiceSevenMidiChannel = 1;
    float assignVoiceEightMidiChannel = 1;
    float assignVoiceNineMidiChannel = 1;
    float assignVoiceTenMidiChannel = 1;
    float assignVoiceElevenMidiChannel = 1;
    float assignVoiceTwelveMidiChannel = 1;
    float assignVoiceThirteenMidiChannel = 1;
    float assignVoiceFourteenMidiChannel = 1;
    float assignVoiceFifteenMidiChannel = 1;
    float assignVoiceSixteenMidiChannel = 1;
    */

    std::vector<int> majorScaleMidiNumbers;
};

