#include <iostream>
#include <vector>
#include <filesystem>

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
    string input;
    vector<string> words;
    fs::path path("/"), prev(path), temp;
    while (true)
    {
        cout << path << "$ ";
        getline(cin, input);
        getWords(input, words);
        switch (words.size())
        {
            case 1:
                if (words[0] == "cd")
                {
                    prev = path;
                    path = "/";
                }
                break;
            case 2:
                if (words[0] == "cd")
                {
                    if (words[1] == "..")
                    {
                        prev = path;
                        path = path.parent_path();
                    }
                    else if (words[1] == "-")
                    {
                        temp = prev;
                        prev = path;
                        path = temp;
                    }
                    else if (words[1] != ".")
                    {
                        temp = path;
                        path.append(words[1]);
                        if (fs::exists(path))
                        {
                            prev = temp;
                        }
                        else
                        {
                            path = temp;
                            cout << "Incorrect path" << endl;
                        }
                    }
                }
        }
    }
}