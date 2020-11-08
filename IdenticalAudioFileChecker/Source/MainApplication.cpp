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
    int count = 0;
    
    for (const juce::File& file : filesToAdd)
    {
        if(!acceptedFileTypes.contains(file.getFileExtension()))
        {
            continue;
        }
        
        files.push_back(file);
        ++count;
    }
    
    return count;
}

int FileAdder::addFiles(const juce::StringArray& filePaths)
{
    int count = 0;
    
    for (const juce::String& str : filePaths)
    {
        int lastStop = str.lastIndexOf(".");
        
        if(!acceptedFileTypes.contains(str.substring(lastStop)))
        {
            continue;
        }
        
        files.push_back(juce::File(str));
        ++count;
    }
    
    return count;
}

void FileAdder::checkFilesAndOpenFolders(std::vector<juce::File>& filesToCheck) const
{
    for(int i = 0; i < filesToCheck.size(); i++)
    {
        juce::File& currentFile = filesToCheck[i];
        
        while(!currentFile.exists() || (currentFile.existsAsFile() && !acceptedFileTypes.contains(currentFile.getFileExtension())))
        {
            filesToCheck.erase(filesToCheck.begin() + i);
        }
        
        if(currentFile.isDirectory())
        {
        if(currentFile.getNumberOfChildFiles(juce::File::findFilesAndDirectories) > 0)
            {
                juce::Array<juce::File> res = currentFile.findChildFiles(juce::File::findFilesAndDirectories, true);
                
                filesToCheck.insert(filesToCheck.begin() + i + 1, res.begin(), res.end());
            }
            
            filesToCheck.erase(filesToCheck.begin() + i);
            
            --i;
        }
    }
    
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
    juce::StringArray filesToAdd;
    
    for(int i = 1; i < arguments.size(); i++)
    {
        filesToAdd.add(arguments[i].text);
    }
    
    fileAdder.addFiles(filesToAdd);
}
