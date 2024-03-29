#include "RenameFiles.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <experimental/filesystem>
#include <stack>
std::string RenameFiles::dataPath;
namespace fs = std::experimental::filesystem;

bool passing_temp = false;

void RenameFiles::ManageRenaming(int argc, char **argv)
{
    std::string option;
    if (argc == 3)
    {
        dataPath = argv[1];
        option = argv[2];
    }

    else if (argc == 1)
    {
        std::cout << boldblue << "Change File Names For Linux" << reset << std::endl;
        std::cout << "Format: rename [path] [option]" << std::endl;
        std::cout << "Options: normal(default) & recursive\n"
                  << std::endl;

        std::cout << "Path: ";
        std::getline(std::cin, dataPath);

        std::cout << "Recursive or normal: ";
        std::getline(std::cin, option);
    }
    else
    {
        std::cout << "Wrong format" << std::endl;
        return;
    }

    if (!fs::exists(dataPath))
    {
        std::cout << "Path doesn't exist" << std::endl;
        return;
    }

    std::transform(option.begin(), option.end(), option.begin(), ::tolower);
    if (!(option == "recursive" || option == "normal"))
    {
        std::cout << "Wrong expression" << std::endl;
        return;
    }

    std::cout << "File names in " << boldred << dataPath << reset << " with " << boldred << option << reset << " option will change" << std::endl;
    std::cout << "These things cannot be undone" << std::endl;
    std::cout << "Do you really want to proceed[Y/n]? ";
    std::string really;
    getline(std::cin, really);
    std::transform(option.begin(), option.end(), option.begin(), ::tolower);
    if (really == "y")
    {
        option == "recursive" ? RecursiveProcess() : NormalProcess();
    }
    else
    {
        std::cout << "Abort" << std::endl;
    }
}
void RenameFiles::NormalProcess()
{
    std::vector<std::string> cvec;
    for (const auto &entry : fs::directory_iterator(dataPath))
    {
        if (entry.path().filename() == "temp")
        {
            std::cout << boldblue << "Passing temp" << reset << std::endl;
            continue;
        }

        std::string newName = Rename(entry.path().filename());
        try
        {
            fs::rename(entry.path(), entry.path().parent_path().string() + "/" + newName);
            std::cout << entry.path().filename() << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cout << boldred << "Passing File: " << entry.path().filename() << reset << '\n';
        }
    }
}

void RenameFiles::RecursiveProcess()
{
    std::stack<fs::v1::__cxx11::path> stackfiles;
    for (const auto &entry : fs::recursive_directory_iterator(dataPath))
    {
        if (entry.path().string().find("temp") != std::string::npos)
        {
            if (!passing_temp)
            {
                std::cout << boldblue << "Passing temp" << reset << std::endl;
                passing_temp = true;
            }
        }
        else
        {
            std::cout << entry.path() << std::endl;
            stackfiles.push(entry.path());
        }
    }

    while (!stackfiles.empty())
    {
        std::string newName = Rename(stackfiles.top().filename());
        if (newName == "")
        {
            stackfiles.pop();
            continue;
        }
        fs::rename(stackfiles.top(), stackfiles.top().parent_path().string() + "/" + newName);
        stackfiles.pop();
    }
}

std::string RenameFiles::Trim(std::string str)
{
    str.erase(str.begin(), find_if(str.begin(), str.end(), [](char p) { return !isspace(p); }));
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int p) { return !std::isspace(p); }).base(), str.end());
    return str;
}

std::string RenameFiles::Rename(std::string str)
{
    if (str[0] == '.')
    { //do not touch secret files
        return "";
    }

    for (auto it = str.begin(); it != str.end(); it++)
    {
        if (ispunct(*it))
        {
            *it = ' ';
        }
    }

    std::string::iterator new_end = std::unique(str.begin(), str.end(), BothAreSpaces);
    str.erase(new_end, str.end());
    str = Trim(str);

    auto it = std::find(str.begin(), str.end(), ' ');

    while (it != str.end())
    {
        str[it - str.begin()] = '.';
        it = std::find(str.begin(), str.end(), ' ');
    }
    return str;
}