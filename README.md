# Identical-Audio-File-Checker-App
A simple command line app which checks if audio files are identical

BASIC IDEA:

This app will use the JUCE framework.
It will allow you to add a number of directories/files through the add command
There will also be a scan command that will iterate through these files testing them against one another


COMMANDS:
add(path) - for files and folders
scan() - to scan the current files

ADDING FILES:
Files can be added individually or together. If a folder is added it will unpack it and extract out all the audio files.

DATA:
List of current files will be held in an std::vector<File>
