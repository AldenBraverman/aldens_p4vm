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

class MidiProcessor
{
public:
    /**
    1 function, takes midi buffer by reference
     Use reference, not copy - host provides chunk, asks plugin to do manipulations
     Midi still goes through plugin
     Host expects the memory to be modified (host provides the memory)
    */
    float adjustMasterPitch = 0;
    
    void process(juce::MidiBuffer& midiMessages)
    {
        processedBuffer.clear();
        processMidiInput(midiMessages);
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
                
                // Make a copy of currentMessage
                // auto transposedMessage = currentMessage;
                // auto oldNoteNum = transposedMessage.getNoteNumber();
                // transposedMessage.setNoteNumber(oldNoteNum + interval);
                
                // processedBuffer.addEvent(transposedMessage, samplePos);
                addTransposedNote(currentMessage, samplePos);
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
    
    // class member
    juce::MidiBuffer processedBuffer;
};

