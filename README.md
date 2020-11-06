# Identical-Audio-File-Checker-App
A simple command line app which checks if audio files are identical

BASIC IDEA:

This app will use the JUCE framework.

COMMANDS:
-a file - to add files and folders for files and folders

ADDING FILES:
Files can be added individually or together. If a folder is added it will unpack it and extract out all the audio files.

DATA:
List of current files will be held in an std::vector<File>

EXTENTIONS:
Optional comman -r removes the any copies of a file after it has found one copy
