#include <iostream>
#include <string>
#include "LexAnalysis.h"
using namespace std;

int main()
{
    string input;
    char temp;
    while ((temp = cin.get()) != EOF)
    {
        input += temp;
    }
    LexAnalysis lexAnalysis(input);
    cout << lexAnalysis.getResult() << endl;
}