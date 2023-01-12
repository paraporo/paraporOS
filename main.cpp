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

int main()
{
    string input, output, home;
    for (int i(0); homedir[i] != '\0'; i++)
        home += homedir[i];
    vector<string> words, entries;
    fs::path path(homedir), prev(path), temp;
    string shell("bash");
    while (true)
    {
        output = ((path.native().size() >= home.size()) && (path.native().substr(0, home.size()) == home)) ? "~" + path.native().substr(home.size()) : path.native();
        if (shell == "zsh")
            cout << "\033[0;44m  " << output<<" \033[0;34m \033[0m";
        else
            cout << "\033[1;34m" << output << "\033[0m$ ";
        getline(cin, input);
        getWords(input, words);
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
                        cout << entries[i] << " ";
                        if (!(i % 6))
                            cout << endl;
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