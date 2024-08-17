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
    /*
    voiceOneActive = false;
    voiceTwoActive = false;
    voiceThreeActive = false;
    voiceFourActive = false;
    */
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
        DBG("MIDI Note "+juce::String(note)+" at Velocity "+juce::String(velocity)+" Assigned to Voice Four"+'\n'+
            "Adjust Master Pitch By: "+juce::String(adjustMasterPitch)+'\n'+
            "Adjust Voice Pitch By: "+juce::String(adjustVoiceFourPitch)
            
        );
        noteVector[3] = note;
        DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
    }
    
    if(voiceOneActive && voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 2)
    {
        voiceThreeActive = true;
        voiceSum = 3;
        DBG("MIDI Note "+juce::String(note)+" at Velocity "+juce::String(velocity)+" Assigned to Voice Three"+'\n'+
            "Adjust Master Pitch By: "+juce::String(adjustMasterPitch)+'\n'+
            "Adjust Voice Pitch By: "+juce::String(adjustVoiceThreePitch)
            
        );
        noteVector[2] = note;
        DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
    }
    
    if(voiceOneActive && !voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 1)
    {
        voiceTwoActive = true;
        voiceSum = 2;
        DBG("MIDI Note "+juce::String(note)+" at Velocity "+juce::String(velocity)+" Assigned to Voice Two"+'\n'+
            "Adjust Master Pitch By: "+juce::String(adjustMasterPitch)+'\n'+
            "Adjust Voice Pitch By: "+juce::String(adjustVoiceTwoPitch)
            
        );
        noteVector[1] = note;
        DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
    }
    
    // DBG("Input - Note On! Note Number: "+juce::String(note)+", Note Velocity: "+juce::String(velocity));
    if(!voiceOneActive && !voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 0)
    {
        voiceOneActive = true;
        voiceSum = 1;
        DBG("MIDI Note "+juce::String(note)+" at Velocity "+juce::String(velocity)+" Assigned to Voice One"+'\n'+
            "Adjust Master Pitch By: "+juce::String(adjustMasterPitch)+'\n'+
            "Adjust Voice Pitch By: "+juce::String(adjustVoiceOnePitch)
            
        );
        noteVector[0] = note;
        DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
    }
}

void MidiHandler::noteOff(int note)
{
    DBG("Note Off Int = " + juce::String(note));
    // DBG("Output - Note Off! Note Number: "+juce::String(note));
    if(voiceOneActive && !voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 1)
    {
        voiceOneActive = false;
        voiceSum = 0;
        DBG("Voice One Deactive");
        DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
    }
    
    if(voiceOneActive && voiceTwoActive && !voiceThreeActive && !voiceFourActive && voiceSum == 2)
    {
        voiceTwoActive = false;
        voiceSum = 1;
        DBG("Voice Two Deactive");
        DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
    }
    
    if(voiceOneActive && voiceTwoActive && voiceThreeActive && !voiceFourActive && voiceSum == 3)
    {
        voiceThreeActive = false;
        voiceSum = 2;
        DBG("Voice Three Deactive");
        DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");


    }
    
    if(voiceOneActive && voiceTwoActive && voiceThreeActive && voiceFourActive && voiceSum == 4)
    {
        if (note == noteVector[0])
        {
            // Voice One Off
            voiceOneActive = false;
            voiceSum = 3;
            // DBG("Voice One Deactive (Vector Check) - "+juce::String(note)+"="+juce::String(noteVector[0]));
            noteVector[0] = 0;
            DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
        }

        if (note == noteVector[1])
        {
            // Voice Two Off
            voiceTwoActive = false;
            voiceSum = 3;
            // DBG("Voice Two Deactive (Vector Check) - " + juce::String(note) + "=" + juce::String(noteVector[1]));
            noteVector[1] = 0;
            DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
        }

        if (note == noteVector[2])
        {
            // Voice Three Off
            voiceThreeActive = false;
            voiceSum = 3;
            // DBG("Voice Three Deactive (Vector Check) - " + juce::String(note) + "=" + juce::String(noteVector[2]));
            noteVector[2] = 0;
            DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
        }

        if (note == noteVector[3])
        {
            // Voice Four Off
            voiceFourActive = false;
            voiceSum = 3;
            // DBG("Voice Four Deactive (Vector Check) - " + juce::String(note) + "=" + juce::String(noteVector[3]));
            noteVector[3] = 0;
            DBG("Note Vector: { " + juce::String(noteVector[0]) + ", " + juce::String(noteVector[1]) + ", " + juce::String(noteVector[2]) + ", " + juce::String(noteVector[3]) + " }");
        }
    }
}
