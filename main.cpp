#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

namespace fs = std::filesystem;
using namespace std;

passwd *pw = getpwuid(getuid());
const char *homedir = pw->pw_dir;

const vector<string> shells = {"bash", "zsh"};

template <typename T>
int indexOf(T const& el, vector<T> const& arr);
void parseInput(string const& s, string &command, vector<string> &arguments, vector<string> &options);

int main()
{
    string input, output, home, command;
    int shellIndex(0);
    for (int i(0); homedir[i] != '\0'; i++)
        home += homedir[i];
    vector<string> words, entries, arguments, options;
    fs::path path(homedir), prev(path), temp;
    while (true)
    {
        output = ((path.native().size() >= home.size()) && (path.native().substr(0, home.size()) == home)) ? "~" + path.native().substr(home.size()) : path.native();
        if (shells[shellIndex] == "zsh")
            cout << "\033[0;30;44m  " << output <<" \033[0;34m \033[0m";
        else
            cout << "\033[1;34m" << output << "\033[0m$ ";
        getline(cin, input);
        parseInput(input, command, arguments, options);
        // cout << "Command: " << command << endl;
        // cout << "Arguments: ";
        // for (int i(0); i < arguments.size(); i++)
        //     cout << arguments[i] << " ";
        // cout << endl;
        // cout << "Options: ";
        // for (int i(0); i < options.size(); i++)
        //     cout << options[i] << " ";
        // cout << endl;
        if (command == "cd")
        {
            if (arguments.size() > 1)
                cout << "Too many arguments" << endl;
            else if (arguments.size() == 0 || arguments[0] == "~")
            {
                prev = path;
                path = home;
            }
            else
            {
                temp = path;
                path.append(arguments[0]);
                if (fs::exists(path))
                    prev = temp;
                else
                {
                    path = temp;
                    cout << "Incorrect path" << endl;
                }
            }
        }
        else if (command == "ls")
        {
            if (arguments.size() > 0)
                cout << "Too many arguments" << endl;
            else
            {
                entries.clear();
                for (fs::directory_entry entry: fs::directory_iterator(path)) {
                    if (entry.path().has_filename() && entry.path().filename().string()[0] != '.')   
                        entries.push_back(entry.is_directory() ? "\033[1;34m" + entry.path().filename().string() + "\033[0m" : "\033[0;32m" + entry.path().filename().string() + "\033[0m");
                }
                if (entries.size() < 8)
                    for (int i(0); i < entries.size(); i++)
                        cout << entries[i] << " ";
                else
                {
                    for (int i(0); i < entries.size(); i++)
                    {
                        if (i && !(i % 6))
                            cout << endl;
                        cout << entries[i] << " ";
                    }
                }
                cout << endl;
            }
        }
        else if (command == "set-shell")
        {
            if (arguments.size() > 1)
                cout << "Too many arguments" << endl;
            else if (arguments.size() == 0)
                shellIndex = (shellIndex + 1) % shells.size();
            else if (arguments.size() == 1)
            {
                shellIndex = indexOf(arguments[0], shells) != -1 ? indexOf(arguments[0], shells) : shellIndex;
                if (indexOf(arguments[0], shells) == -1)
                    cout << "Incorrect shell" << endl;
            }
        }
        else
            cout << "Unknown command: \"" << command << "\"" << endl;
        
        path = path.lexically_normal().native();
        while (path.string().size() > 1 && path.string()[path.string().size() - 1] == '/')
            path = path.string().substr(0, path.string().size() - 1);
    }
}

template <typename T>
int indexOf(T const& el, vector<T> const& arr)
{
    for (int i(0); i < arr.size(); i++)
        if (arr[i] == el)
            return i;
    return -1;
}

void parseInput(string const& s, string &command, vector<string> &arguments, vector<string> &options)
{
    command.clear(), arguments.clear(), options.clear();
    bool first(true), space(true);
    for (char c: s)
    {
        if (c != ' ' && first)
            command += c;
        else if (c != ' ')
        {
            if (space)
                arguments.push_back("");
            arguments[arguments.size() - 1] += c;
        }
        if (!space && c == ' ')
            first = false;
        space = c == ' ';
    }
    // for (int i(0); i < arguments.size(); i++)
    // {
    //     if (arguments[i].size() >= 2 && arguments[i][0] == '-' && arguments[i][1] != '-')
    //     {
    //         for (int j(1); j < arguments[i].size(); j++)
    //             options.push_back(string(1, arguments[i][j]));
    //         arguments.erase(arguments.begin() + i);
    //         i--;
    //     }
    //     else if (arguments[i].size() >= 3 && arguments[i][0] == '-' && arguments[i][1] == '-' && arguments[i][2] != '-')
    //     {
    //         options.push_back(arguments[i].substr(2, arguments[i].size() - 2));
    //         arguments.erase(arguments.begin() + i);
    //         i--;
    //     }
    // }
}