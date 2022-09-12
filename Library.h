/*
  ==============================================================================

    Library.h
    Created: 23 Feb 2022 11:07:10pm
    Author:  Asus

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Library
{
public:
    Library(juce::File _file);
    juce::File file;
    juce::URL URL;
    juce::String title;
    juce::String length;
    /**objects are compared by title*/
    bool operator==(const juce::String& other) const;
};