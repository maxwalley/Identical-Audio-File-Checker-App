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
    
private:
    int bufferSize = 44100;
    
    juce::AudioBuffer<float> firstBuffer;
    juce::AudioBuffer<float> secondBuffer;
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
