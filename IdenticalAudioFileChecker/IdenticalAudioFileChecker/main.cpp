//
//  main.cpp
//  IdenticalAudioFileChecker
//
//  Created by Max Walley on 05/11/2020.
//  Copyright © 2020 Max Walley. All rights reserved.
//

#include <iostream>

enum class Commands
{
    Exit,
    Help,
    Unknown
};

Commands getCommandFromString(const std::string& command)
{
    if(command == "Help")
    {
        return Commands::Help;
    }
    else if(command == "Exit")
    {
        return Commands::Exit;
    }
    
    return Commands::Unknown;
}

int main(int argc, const char * argv[])
{
    std::string inputCommand;
    
    while(inputCommand != "Exit")
    {
        std::cin >> inputCommand;
        
        Commands commandToRun = getCommandFromString(inputCommand);
        
        switch (commandToRun)
        {
            case Commands::Help:
                std::cout << "Valid commands are Exit, \n";
                break;
                
            case Commands::Unknown:
                std::cout << "Command not recognised\nUse 'Help' to get a list of valid commands\n";
                break;
                
            case Commands::Exit:
                break;
        }
    }
}
