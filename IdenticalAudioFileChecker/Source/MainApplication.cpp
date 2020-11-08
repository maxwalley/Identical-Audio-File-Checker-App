/*
  ==============================================================================

    MainApplication.cpp
    Created: 6 Nov 2020 3:25:48pm
    Author:  Max Walley

  ==============================================================================
*/

#include "MainApplication.h"

template<template<class ...> class Container_Type>
int FileAdder::addFiles(Container_Type<juce::File>)
{
    
}

template<template<class ...> class Container_Type>
int FileAdder::addFiles(Container_Type<juce::String>)
{
    
}

int FileAdder::addFiles(const juce::StringArray& filePaths)
{
    
}

MainApplication::MainApplication(int argc, char* argv[])  : fileAdder(files)
{
    
    commandManager.addHelpCommand("--help|-help", "The Identical Audio File Checker scans a list of audio files and finds if any are identical.", true);
    
    commandManager.addCommand({"--a", "File paths", "A list of the files to scan", "", std::bind(&MainApplication::addFiles, this, std::placeholders::_1)});
    
    commandManager.findAndRunCommand(argc, argv);
    
    if(!files.empty())
    {
        //Scan
    }
}

void MainApplication::addFiles(const juce::ArgumentList& arguments)
{
    //Add files through file adder
}
