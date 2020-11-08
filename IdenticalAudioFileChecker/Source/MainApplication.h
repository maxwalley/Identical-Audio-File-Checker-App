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
    
    void setAcceptedFileTypes(const juce::StringArray& fileTypes) {acceptedFileTypes = fileTypes;};
    
private:
    //Takes the vector and removes any invalid files and opens any folders
    void checkFilesAndOpenFolders(std::vector<juce::File>& filesToCheck) const;
    
    std::vector<juce::File>& files;
    juce::StringArray acceptedFileTypes;
};

class MainApplication
{
public:
    MainApplication(int argc, char* argv[]);
    ~MainApplication(){};
    
private:
    void addFiles(const juce::ArgumentList& arguments);
    
    juce::ConsoleApplication commandManager;
    std::vector<juce::File> files;
    FileAdder fileAdder;
};
