/*
  ==============================================================================

    Utils.h
    Created: 15 Jun 2024 1:21:32pm
    Author:  Alden Braverman

  ==============================================================================
*/

#pragma once

template<typename T>

inline static void castParameter(juce::AudioProcessorValueTreeState& apvts,
                                 const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination); // parameter does not exist or wrong type
}
