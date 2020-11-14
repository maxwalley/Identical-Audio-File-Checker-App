/*
  ==============================================================================

    MainApplication.h
    Created: 6 Nov 2020 3:25:48pm
    Author:  Max Walley

  ==============================================================================
*/
#include <JuceHeader.h>

#pragma once

class FileAdder
{
public:
    FileAdder(std::vector<juce::File>& listToAddTo)  : files(listToAddTo){};
    ~FileAdder(){};
    
    //Must be an iterator to a container holding files
    template<template<class ...> class Container_Type>
    int addFiles(const Container_Type<juce::File>& filesToAdd);
    
    int addFiles(const juce::StringArray& filePaths);
    
    void addFile(const juce::File& file);
    
    void setAcceptedFileTypes(const juce::StringArray& fileTypes) {acceptedFileTypes = fileTypes;};
    
private:
    std::vector<juce::File>& files;
    juce::StringArray acceptedFileTypes;
};

class AudioFormatReaderComparator
{
public:
    AudioFormatReaderComparator();
    ~AudioFormatReaderComparator(){};
    
    juce::Result compareReaders(juce::AudioFormatReader* first, juce::AudioFormatReader* second);
    
    //This is the buffer size of the buffers that are used to move through the readers. Since the readers can have different sample rates the buffer size for readers are based off their sample rates. Therefore this is a multiplier rather than a specific value. For example for a reader with a sample rate of 44100 with a buffer size of 0.5 the buffer size would be 22050. By default this is set to 1.0
    void setBufferSize(float newSizeMultiplier);
    
    //This sets how much to allow samples to be different by. Due to the way floating point values are stored, small differences in values that should be equal are likely. By default this is set to 0.001
    void setToleranceLimitBetweenValues(float newTolerance) {tolerance = newTolerance;};
    float getToleranceLimitBetweenValues() const {return tolerance;};
    
private:
    int bufferSize = 44100;
    
    juce::AudioBuffer<float> firstBuffer;
    juce::AudioBuffer<float> secondBuffer;
    
    float tolerance = 0.001;
    float bufferSizeMultiplier = 1.0;
};

class MainApplication
{
public:
    MainApplication(int argc, char* argv[]);
    ~MainApplication(){};
    
private:
    void addFiles(const juce::ArgumentList& arguments);
    
    int scanFiles();
    
    juce::ConsoleApplication commandManager;
    std::vector<juce::File> files;
    FileAdder fileAdder;
    
    juce::AudioFormatManager fmtMan;
    
    std::unique_ptr<juce::AudioFormatReader> compFileReader1 = nullptr;
    std::unique_ptr<juce::AudioFormatReader> compFileReader2 = nullptr;
    
    AudioFormatReaderComparator comparator;
};
