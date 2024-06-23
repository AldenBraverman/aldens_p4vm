/*
  ==============================================================================

    MidiHandler.cpp
    Created: 3 Jun 2024 11:00:20pm
    Author:  Alden Braverman

  ==============================================================================
*/

#include "MidiHandler.h"
MidiHandler::MidiHandler()
{
    // May have to move this to a reset() method
    voiceOneActive = false;
    voiceTwoActive = false;
    voiceThreeActive = false;
    voiceFourActive = false;
}

void MidiHandler::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2)
{
    switch (data0 & 0xF0) {
        // Note Off
        case 0x80:
            noteOff(data1 & 0x7F);
            break;
            
        // Note On
        case 0x90:
            uint8_t note = data1 & 0x7F;
            uint8_t velo = data2 & 0x7F;
            if (velo > 0) {
                noteOn(note,velo);
            } else {
                noteOff(note);
            }
            break;
    }
}

void MidiHandler::noteOn(int note, int velocity)
{
    if(voiceOneActive && voiceTwoActive && voiceThreeActive && !voiceFourActive && voiceSum == 3)
    {
        voiceFourActive = true;
        voiceSum = 4;
        DBG("MIDI Note "+juce::String(note)+"at Velocity "+juce::String(velocity)+" Assigned to Voice Four");
    }
    
    if(voiceOneActive && voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 2)
    {
        voiceThreeActive = true;
        voiceSum = 3;
        DBG("MIDI Note "+juce::String(note)+"at Velocity "+juce::String(velocity)+" Assigned to Voice Three");

    }
    
    if(voiceOneActive && !voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 1)
    {
        voiceTwoActive = true;
        voiceSum = 2;
        DBG("MIDI Note "+juce::String(note)+"at Velocity "+juce::String(velocity)+" Assigned to Voice Two");
    }
    
    // DBG("Input - Note On! Note Number: "+juce::String(note)+", Note Velocity: "+juce::String(velocity));
    if(!voiceOneActive && !voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 0)
    {
        voiceOneActive = true;
        voiceSum = 1;
        DBG("MIDI Note "+juce::String(note)+"at Velocity "+juce::String(velocity)+" Assigned to Voice One");
    }
}

void MidiHandler::noteOff(int note)
{
    // DBG("Output - Note Off! Note Number: "+juce::String(note));
    if(voiceOneActive && !voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 1)
    {
        voiceOneActive = false;
        voiceSum = 0;
        DBG("Voice One Deactive");
    }
    
    if(voiceOneActive && voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 2)
    {
        voiceTwoActive = false;
        voiceSum = 1;
        DBG("Voice Two Deactive");
    }
    
    if(voiceOneActive && voiceTwoActive && voiceThreeActive && !voiceFourActive && voiceSum == 3)
    {
        voiceThreeActive = false;
        voiceSum = 2;
        DBG("Voice Three Deactive");
    }
    
    if(voiceOneActive && voiceTwoActive && voiceThreeActive && voiceFourActive && voiceSum == 4)
    {
        voiceFourActive = false;
        voiceSum = 3;
        DBG("Voice Four Deactive");
    }
}
