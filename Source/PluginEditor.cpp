/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Aldens_p4vmAudioProcessorEditor::Aldens_p4vmAudioProcessorEditor (Aldens_p4vmAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    // startTimerHz(60); // 60 FPS
    setSize (400, 300);
}

Aldens_p4vmAudioProcessorEditor::~Aldens_p4vmAudioProcessorEditor()
{
}

//==============================================================================
void Aldens_p4vmAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // addAndMakeVisible(openGLComponent);
    // openGLComponent.setBounds(0, 0, 400, 300);
}

void Aldens_p4vmAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
