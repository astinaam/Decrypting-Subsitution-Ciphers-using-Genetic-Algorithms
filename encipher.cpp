// This code produces a cipher text from plain text with random key
#include<bits/stdc++.h>

using namespace std;

#define sz(x) (int)x.size()

string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int main()
{
    srand (time(NULL));
    // plain text all lowercase
    freopen("test2.txt","r",stdin);
    // cipher text will be produced
    freopen("test.txt","w",stdout);
    // randomly generate a key
    random_shuffle(alphabet.begin(),alphabet.end());
    // generate key mappings
    map<char,char> key;
    for(int i=0;i<26;++i)
    {
        key[char('A' + i)] = alphabet[i];
        cerr<<char('A' + i)<<" "<<alphabet[i]<<endl; // output log
    }
    // get the key on a std::string
    string s, str = "";
    while(getline(cin,s))
    {
        str += s;
    }
    cerr<<"KEY : "<<alphabet<<endl; // log output
    // covert to uperacase
    transform(str.begin(),str.end(),str.begin(), ::toupper);
    // encode only the letters & numbers
    for(int i=0;i<sz(str);++i)
    {
        if((str[i] >= '0' && str[i] <= '9') || (str[i]>='A' && str[i] <= 'Z') || (str[i]>='a' && str[i]<='z'))
        {
            str[i] = key[str[i]];
        }
    }
    cerr<<str<<endl;
    // output the cipher text
    cout<<str;
}
