#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

passwd *pw = getpwuid(getuid());

const char *homedir = pw->pw_dir;

namespace fs = std::filesystem;
using namespace std;

void getWords(string const& s, vector<string> &words)
{
    words.clear();
    bool newWord(true);
    for (char c: s)
    {
        if (c == ' ')
            newWord = true;
        else
        {
            if (newWord)
            {
                newWord = false;
                words.push_back("");
            }
            words[words.size() - 1] += c;
        }
    }
}

void parseInput(string const& s, string &command, vector<string> &arguments, vector<string> &options)
{
    command.clear(), arguments.clear(), options.clear();
    bool first(true), space(true);
    for (char c: s)
    {
        if (c == ' ')
        {
            if (!space)
                first = false;
            space = true;
        }
        else
        {
            if (first)
                command += c;
            else
            {
                if (space)
                    arguments.push_back("");
                arguments[arguments.size() - 1] += c;
            }
            space = false;
        }
    }
    for (int i(0); i < arguments.size(); i++)
    {
        if (arguments[i].size() >= 2 && arguments[i][0] == '-' && arguments[i][1] != '-')
        {
            for (int j(1); j < arguments[i].size(); j++)
                options.push_back(string(1, arguments[i][j]));
            arguments.erase(arguments.begin() + i);
            i--;
        }
        else if (arguments[i].size() >= 3 && arguments[i][0] == '-' && arguments[i][1] == '-' && arguments[i][2] != '-')
        {
            options.push_back(arguments[i].substr(2, arguments[i].size() - 2));
            arguments.erase(arguments.begin() + i);
            i--;
        }
    }
}

int main()
{
    string input, output, home, shell("bash"), command;
    for (int i(0); homedir[i] != '\0'; i++)
        home += homedir[i];
    vector<string> words, entries, arguments, options;
    fs::path path(homedir), prev(path), temp;
    while (true)
    {
        output = ((path.native().size() >= home.size()) && (path.native().substr(0, home.size()) == home)) ? "~" + path.native().substr(home.size()) : path.native();
        if (shell == "zsh")
            cout << "\033[0;44m  " << output <<" \033[0;34m \033[0m";
        else
            cout << "\033[1;34m" << output << "\033[0m$ ";
        getline(cin, input);
        getWords(input, words);
        parseInput(input, command, arguments, options);
        cout << "Command: " << command << endl;
        cout << "Arguments: ";
        for (int i(0); i < arguments.size(); i++)
            cout << arguments[i] << " ";
        cout << endl;
        cout << "Options: ";
        for (int i(0); i < options.size(); i++)
            cout << options[i] << " ";
        cout << endl;
        if (words.size() == 1)
        {
            if (words[0] == "cd")
            {
                prev = path;
                path = homedir;
            }
            else if (words[0] == "ls")
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
        else if (words.size() == 2)
        {
            if (words[0] == "cd")
            {   
                if (words[1] == "~")
                {
                    prev = path;
                    path = home;
                }
                else
                {
                    temp = path;
                    path.append(words[1]);
                    if (fs::exists(path))
                        prev = temp;
                    else
                    {
                        path = temp;
                        cout << "Incorrect path" << endl;
                    }
                }
            }
            else if (words[0] == "set-shell")
            {
                if (words[1] == "bash")
                    shell = "bash";
                else if (words[1] == "zsh")
                    shell = "zsh";
                else
                    cout << "Incorrect shell" << endl;
            }
        }
        path = path.lexically_normal().native();
        while (path.string().size() > 1 && path.string()[path.string().size() - 1] == '/')
            path = path.string().substr(0, path.string().size() - 1);
    }
}