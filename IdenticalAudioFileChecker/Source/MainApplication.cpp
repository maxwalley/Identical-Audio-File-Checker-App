/*
  ==============================================================================

    MainApplication.cpp
    Created: 6 Nov 2020 3:25:48pm
    Author:  Max Walley

  ==============================================================================
*/

#include "MainApplication.h"

template<template<class ...> class Container_Type>
int FileAdder::addFiles(const Container_Type<juce::File>& filesToAdd)
{
    int oldSize = files.size();
    
    for (const juce::File& file : filesToAdd)
    {
        addFile(file);
    }
    return files.size() - oldSize;
}

int FileAdder::addFiles(const juce::StringArray& filePaths)
{
    std::vector<juce::File> files;
    files.resize(filePaths.size());
    
    
    std::transform(filePaths.begin(), filePaths.end(), files.begin(), [](const juce::String& path)
    {
        return juce::File(path);
    });
    return addFiles(files);
}

void FileAdder::addFile(const juce::File& file)
{
    if(file.existsAsFile() && acceptedFileTypes.contains(file.getFileExtension()))
    {
        files.push_back(file);
    }
    
    else if(file.isDirectory())
    {
        juce::Array<juce::File> children = file.findChildFiles(2, true);
        
        for (const juce::File& child : children)
        {
            addFile(child);
        }
    }
}

MainApplication::MainApplication(int argc, char* argv[])  : fileAdder(files)
{
    fileAdder.setAcceptedFileTypes(juce::StringArray(".mp3"));
    
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
    juce::StringArray filesToAdd;
    filesToAdd.strings.resize(arguments.size() - 1);
    
    std::transform(arguments.arguments.begin() + 1, arguments.arguments.end(), filesToAdd.begin(), [](const juce::ArgumentList::Argument& arg)
    {
        return arg.text;
    });
    
    fileAdder.addFiles(filesToAdd);
}
