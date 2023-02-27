#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <cctype>

#include <unordered_map> // For bonus exercise
#include <bits/stdc++.h> // For bonus exercise, string manipulation

using namespace std;
namespace fs = std::filesystem;

// Global variables
bool sensitive = false;
bool recursive = false;
bool output = false;
bool display_line_number = false;
bool search_depth = false;
bool search_sub = false;
bool file_type = false;
bool exclude_dir = false;
bool hash_table = false;

std::string file_name;
int depth = 0;
std::vector<std::string> file_extensions;
std::vector<std::string> excluded_dir;
std::unordered_map<std::string, std::vector<std::string>> words_index;

// Search for word specified by the user input
void search_for_words(std::string sp, std::string word)
{
    if (hash_table)
    {
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c)
                       { return std::tolower(c); });
        for (auto h : words_index[word])
        {
            std::cout << h << "\n";
        }
    }
    else
    {
        std::ofstream outfile(file_name, ios::app);
        std::vector<std::string> folders;
        fs::path p;

        for (fs::directory_iterator itr(sp); itr != fs::directory_iterator(); ++itr)
        {
            if (itr->path().filename() != "." && itr->path().filename() != "..")
            {
                int line_number = 0;
                std::string line_str = "";
                p = itr->path();
                if (fs::is_regular_file(p) && !p.empty())
                {
                    if (search_sub)
                    {
                        continue;
                    }
                    if (file_type)
                    {
                        if (!std::count(file_extensions.begin(), file_extensions.end(), fs::path(p).extension()))
                        {
                            continue;
                        }
                    }
                    std::ifstream file(p.string());
                    std::string line;
                    bool found = false;
                    while (std::getline(file, line))
                    {
                        line_number++;
                        std::stringstream data(line);
                        std::string temp;
                        while (std::getline(data, temp, ' '))
                        {
                            if (display_line_number)
                            {
                                line_str = " - Line: " + std::to_string(line_number);
                            }
                            if (!sensitive)
                            {
                                std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c)
                                               { return std::tolower(c); });
                                std::transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c)
                                               { return std::tolower(c); });
                            }
                            if (word == temp)
                            {
                                if (output)
                                {
                                    outfile << p.string() << line_str << "\n";
                                }
                                else
                                {
                                    std::cout << p.string() << line_str << std::endl;
                                }
                                if (!display_line_number)
                                {
                                    found = true;
                                    break;
                                }
                            }
                        }
                        if (found)
                        {
                            break;
                        }
                    }
                }
                else if (fs::is_directory(p))
                {
                    if (exclude_dir)
                    {
                        bool match = false;
                        std::stringstream dir_words(p.string());
                        std::string word;

                        while (getline(dir_words, word, '\\'))
                        {
                            for (auto w : excluded_dir)
                            {
                                std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c)
                                               { return std::tolower(c); });
                                std::transform(w.begin(), w.end(), w.begin(), [](unsigned char c)
                                               { return std::tolower(c); });
                                if (word == w)
                                {
                                    match = true;
                                }
                            }
                        }
                        if (match)
                        {
                            continue;
                        }
                        else
                        {
                            folders.push_back(p.string());
                        }
                    }
                    else
                    {
                        folders.push_back(p.string());
                    }
                }
                else
                {
                    std::cout << "!Error! Cant read file :" << p.string() << std::endl;
                }
            }
        }

        if (search_sub)
        {
            search_sub = false;
        }

        if (recursive)
        {
            if (search_depth)
            {
                while (depth != 0)
                {
                    depth--;
                    for (auto f : folders)
                    {
                        search_for_words(f, word);
                    }
                }
            }
            else
            {
                for (auto f : folders)
                {
                    search_for_words(f, word);
                }
            }
        }
    }
}

// Indexing each word, Always goes through the whole directory and sub-directories
void indexing_word(std::string sp)
{
    fs::path p;
    std::vector<std::string> folders;

    for (fs::directory_iterator itr(sp); itr != fs::directory_iterator(); ++itr)
    {
        if (itr->path().filename() != "." && itr->path().filename() != "..")
        {
            p = itr->path();
            if (fs::is_regular_file(p) && !p.empty())
            {
                std::ifstream file(p.string());
                std::string line;
                while (std::getline(file, line))
                {
                    std::stringstream data(line);
                    std::string temp;
                    while (std::getline(data, temp, ' '))
                    {
                        if (!sensitive)
                        {
                            std::transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c)
                                           { return std::tolower(c); });
                        }
                        if (words_index.find(temp) != words_index.end())
                        {
                            if (words_index[temp].back() != p.string())
                            {
                                words_index[temp].push_back(p.string());
                            }
                        }
                        else
                        {
                            words_index[temp].push_back(p.string());
                        }
                    }
                }
            }
            else if ((fs::is_directory(p)))
            {
                folders.push_back(p.string());
            }
        }
        else
        {
            std::cout << "!Error! Cant read file :" << p.string() << std::endl;
        }
    }

    for (auto f : folders)
    {
        indexing_word(f);
    }
}

void default_settings()
{
    std::cout << "\nDefault search setting:\n\tCase sensitive - off\n\tRecursive search - off\n\tSearch sub-directories only - off\n\tDisplay line number - off\n\tFile type - all\n\tExclude directories - none\n\tSearch depth - none\n\tOutput - console\n";
    std::cout << "\nIndexing mode setting: (Only case sensitive option in indexing mode)\n\tCase sensitive - off\n";
}

void options_choice()
{
    std::cout << "\nOptions:\n\t-h {Indexing mode}\n\t-c {Case sensitive search}\n\t-r {Recursive search}\n\t-s {Search sub-directories only}\n\t-l {Display line number}\n\t-t {File types to search for} (Add more than one file type with ',')\n\t\t[Example: -t txt,md]";
    std::cout << "\n\t-e {Exclude directories} (Not case sensitive) (Add more than one directory with ',')\n\t\t[Example: -e mat,koffeindrycker]\n\t-d {Limit search depth}\n\t\t[Example: -d 1]\n\t-o {Writes to output file instead of console}\n\t\t[Example: -o File-Name.txt]";
}

int main(int argc, char **argv)
{
    std::string search_path = "../TestData";
    std::string word;
    int opt;
    while ((opt = getopt(argc, argv, "icro:ld:st:e:h?")) != -1)
    {
        switch (opt)
        {
        case 'c':
            sensitive = true;
            break;
        case 'r':
            recursive = true;
            break;
        case 'o':
            output = true;
            file_name = optarg;
            remove(file_name.c_str());
            break;
        case 'l':
            display_line_number = true;
            break;
        case 'd':
            search_depth = true;
            depth = atoi(optarg);
            break;
        case 's':
            search_sub = true;
            break;
        case 't':
        {
            file_type = true;
            std::string temp;
            std::stringstream tempopt(optarg);
            while (std::getline(tempopt, temp, ','))
            {
                file_extensions.push_back('.' + temp);
            }
        }
        break;
        case 'e':
        {
            exclude_dir = true;
            std::string temp;
            std::stringstream tempopt(optarg);
            while (std::getline(tempopt, temp, ','))
            {
                excluded_dir.push_back(temp);
            }
        }
        break;
        case 'h':
            hash_table = true;
            break;
        case '?':
            std::cout << "Usage: " << argv[0] << std::endl;
            default_settings();
            options_choice();
            exit(-1);
            break;
        default:
            std::cout << "Usage: " << argv[0] << std::endl;
            default_settings();
            options_choice();
            exit(-1);
            break;
        }
    }

    std::cout << "------------\nFile Crawler\n------------" << std::endl;
    std::cout << "Please enter word to search for: ";
    std::cin >> word;

    if (hash_table)
    {
        indexing_word(search_path);
    }

    search_for_words(search_path, word);
}
