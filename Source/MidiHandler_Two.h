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
 Call buffer.clear() in the process block
 
 */

#include <JuceHeader.h>

constexpr int interval = 4;
constexpr int MAX_VOICES = 4;

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}

class MidiProcessor
{
public:
    /**
    1 function, takes midi buffer by reference
     Use reference, not copy - host provides chunk, asks plugin to do manipulations
     Midi still goes through plugin
     Host expects the memory to be modified (host provides the memory)
    */
    
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
                DBG("New Note On/Off -"+currentMessage.getDescription());
                if (currentMessage.isNoteOn())
                {
                    // DBG("Note On Message on Note "+juce::String(currentMessage.getNoteNumber()));
                    for (int v=0; v < MAX_VOICES; v++) {
                        DBG(juce::String(v)+" "+BoolToString(voices[v]));
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
                        DBG(juce::String(v)+" "+BoolToString(voices[v])+" "+BoolToString(myIsNoteOnVoice(v, currentMessage.getNoteNumber())));
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
                // auto oldNoteNum = messageToTranspose.getNoteNumber();
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
                
            case 1:
                // voice 2
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
            
            case 2:
                // voice 3
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
            
            case 3:
                // voice 4
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
        }
    }
    
    void myStopNoteOnVoice(int voice, juce::MidiMessage messageToTranspose, int samplePos)
    {
        auto oldNoteNum = messageToTranspose.getNoteNumber();
        
        switch (voice) {
            case 0:
                // voice 1
                // auto oldNoteNum = messageToTranspose.getNoteNumber();
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceOnePitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
                
            case 1:
                // voice 2
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceTwoPitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
            
            case 2:
                // voice 3
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceThreePitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
            
            case 3:
                // voice 4
                messageToTranspose.setNoteNumber(oldNoteNum+adjustMasterPitch+adjustVoiceFourPitch);
                
                processedBuffer.addEvent(messageToTranspose, samplePos);
                break;
        }
    }
    
    bool myIsNoteOnVoice(int voice, int note) {
        return voices[voice] && (voiceNotes[voice] == note);
    }
    
    // class member
    juce::MidiBuffer processedBuffer;
    bool voices[4] = { false, false, false, false };
    int voiceNotes[4] = { -1, -1, -1, -1 };
    
    float adjustMasterPitch = 0;
    float adjustVoiceOnePitch = 0;
    float adjustVoiceTwoPitch = 0;
    float adjustVoiceThreePitch = 0;
    float adjustVoiceFourPitch = 0;
};

