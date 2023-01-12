#include <iostream>
#include <vector>

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
    while (true)
    {
        getline(cin, input);
        getWords(input, words);
        for (string word: words)
            cout << word << " ";
        cout << endl;
    }
}