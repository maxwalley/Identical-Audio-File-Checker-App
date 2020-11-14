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
    //Makes sure the one with the higher sample rate is first
    if(first->sampleRate < second->sampleRate)
    {
        std::swap(first, second);
    }
    
    if(first->bitsPerSample != second->bitsPerSample)
    {
        return juce::Result::fail("Incompatible Bit Depths");
    }
    
    if(first->numChannels != second->numChannels)
    {
        return juce::Result::fail("Incompatible Num Channels");
    }
    
    //Work out the difference in sample rates - Turn this into seperate class
    int sampleDistanceOnFirst = 1;
    int sampleDistanceOnSecond = 1;
    
    if(first->sampleRate > second->sampleRate)
    {
        //Higher and lower means the sample rate
        double firstLenOfSample = 1000.0 / first->sampleRate;
        double secondLenOfSample = 1000.0 / second->sampleRate;
        
        double curSample = firstLenOfSample;
        int i = 1;
        
        //Check if this length is divisible by the length of the lower sample
        while(fmod(curSample, secondLenOfSample) > 0.0000001)
        {
            //Move it on a sample
            curSample += firstLenOfSample;
            ++i;
        }
        
        sampleDistanceOnFirst = i;
        sampleDistanceOnSecond = floor(curSample / secondLenOfSample);
    }
    
    firstBuffer.setSize(first->numChannels, first->sampleRate * bufferSizeMultiplier);
    secondBuffer.setSize(second->numChannels, second->sampleRate * bufferSizeMultiplier);
    
    //Keeps track of where the buffer is in the stream
    int firstBufferStartPos = 0;
    int secondBufferStartPos = 0;
    
    //while positions are less than the length of the readers
    while(firstBufferStartPos < first->lengthInSamples)
    {
        firstBuffer.clear();
        secondBuffer.clear();
        
        first->read(&firstBuffer, 0, firstBuffer.getNumSamples(), firstBufferStartPos, true, true);
        second->read(&secondBuffer, 0, secondBuffer.getNumSamples(), secondBufferStartPos, true, true);
        
        for(int channel = 0; channel < firstBuffer.getNumChannels(); channel++)
        {
            const float* firstBufferReadPtr = firstBuffer.getReadPointer(channel);
            const float* secondBufferReadPtr = secondBuffer.getReadPointer(channel);
            
            int firstBufferReadPos = 0;
            int secondBufferReadPos = 0;
            
            while(firstBufferReadPos < firstBuffer.getNumSamples() && secondBufferReadPos < secondBuffer.getNumSamples())
            {
                if(std::abs(firstBufferReadPtr[firstBufferReadPos] - secondBufferReadPtr[secondBufferReadPos]) > tolerance)
                {
                    return juce::Result::fail("Different samples found");
                }
                firstBufferReadPos += sampleDistanceOnFirst;
                secondBufferReadPos += sampleDistanceOnSecond;
            }
        }
        firstBufferStartPos += firstBuffer.getNumSamples();
        secondBufferStartPos += secondBuffer.getNumSamples();
    }
    
    return juce::Result::ok();
}

MainApplication::MainApplication(int argc, char* argv[])  : fileAdder(files)
{
    fileAdder.setAcceptedFileTypes(juce::StringArray({".mp3", ".wav"}));
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
