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

AudioFormatReaderComparator::AudioFormatReaderComparator()  : firstBuffer(2, bufferSize), secondBuffer(2, bufferSize)
{
    firstBuffer.clear();
    secondBuffer.clear();
}

juce::Result AudioFormatReaderComparator::compareReaders(juce::AudioFormatReader* first, juce::AudioFormatReader* second)
{
    if(first->sampleRate != second->sampleRate)
    {
        return juce::Result::fail("Incompatible Sample Rates");
    }
    
    else if(first->bitsPerSample != second->bitsPerSample)
    {
        return juce::Result::fail("Incompatible Bit Depths");
    }
    
    if(first->numChannels != second->numChannels)
    {
        return juce::Result::fail("Incompatible Num Channels");
    }
    
    if(first->numChannels != firstBuffer.getNumChannels())
    {
        firstBuffer.setSize(first->numChannels, bufferSize);
        secondBuffer.setSize(first->numChannels, bufferSize);
    }
    
    int currentSamplePosition = 0;
    
    while(currentSamplePosition < first->lengthInSamples)
    {
        firstBuffer.clear();
        secondBuffer.clear();
        
        first->read(&firstBuffer, 0, bufferSize, currentSamplePosition, true, true);
        
        second->read(&secondBuffer, 0, bufferSize, currentSamplePosition, true, true);
        
        for(int chan = 0; chan < first->numChannels; chan++)
        {
            const float* firstBufferRead = firstBuffer.getReadPointer(chan, 0);
            const float* secondBufferRead = secondBuffer.getReadPointer(chan, 0);
            
            for(int i = 0; i < bufferSize; i++)
            {
                if(firstBufferRead[i] != secondBufferRead[i])
                {
                    return juce::Result::fail("Different samples at " + juce::String(i + currentSamplePosition) + " on channel " + juce::String(chan));
                }
            }
        }
        
        currentSamplePosition += bufferSize;
    }
    
    return juce::Result::ok();
}

MainApplication::MainApplication(int argc, char* argv[])  : fileAdder(files)
{
    fileAdder.setAcceptedFileTypes(juce::StringArray(".mp3"));
    fmtMan.registerBasicFormats();
    
    commandManager.addHelpCommand("--help|-help", "The Identical Audio File Checker scans a list of audio files and finds if any are identical.", true);
    
    commandManager.addCommand({"--a", "File paths", "A list of the files to scan", "", std::bind(&MainApplication::addFiles, this, std::placeholders::_1)});
    
    commandManager.findAndRunCommand(argc, argv);
    
    if(!files.empty())
    {
        int numDuplicates = scanFiles();
        std::cout << numDuplicates << " duplicates found\n";
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

int MainApplication::scanFiles()
{
    int numDuplicates = 0;
    int currentIndex = 0;
    
    for (const juce::File& currentFile : files)
    {
        compFileReader1.reset(fmtMan.createReaderFor(currentFile));
        
        if(compFileReader1 == nullptr)
        {
            std::cout << "Warning file could not be read: " << currentFile.getFullPathName() << "\n";
            continue;
        }
        
        for(int i = ++currentIndex; i < files.size(); i++)
        {
            compFileReader2.reset(fmtMan.createReaderFor(files[i]));
            
            if(compFileReader2 == nullptr)
            {
                std::cout << "Warning file could not be read: " << files[i].getFullPathName() << "\n";
                
                //removing the unreadable file stops the it being attempted to be read again
                files.erase(files.begin() + i);
                continue;
            }
            
            juce::Result res = comparator.compareReaders(compFileReader1.get(), compFileReader2.get());
            
            if(res)
            {
                std::cout << currentFile.getFullPathName() << " is the same as " << files[i].getFullPathName() << "\n";
                
                ++numDuplicates;
            }
            else
            {
                std::cout << res.getErrorMessage() << std::endl;
            }
        }
    }
    
    return numDuplicates;
}
