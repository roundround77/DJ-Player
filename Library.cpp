/*
  ==============================================================================

    Library.cpp
    Created: 23 Feb 2022 11:07:10pm
    Author:  Asus

  ==============================================================================
*/

#include "LibraRY.h"
#include <filesystem>

Library::Library(juce::File _file) : file(_file),
title(_file.getFileNameWithoutExtension()),
URL(juce::URL{ _file })
{
    DBG("Created new track with title: " << title);
}

bool Library::operator==(const juce::String& other) const
{
    return title == other;
}
