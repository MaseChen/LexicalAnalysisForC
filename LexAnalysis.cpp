#include "LexAnalysis.h"

using namespace std;

const vector<string> LexAnalysis::KEYWORD = {"alignas", "alignof", "auto", "bool", "break",
                                             "case", "char", "const", "constexpr", "continue",
                                             "default", "do", "double", "else", "enum",
                                             "extern", "false", "float", "for", "goto",
                                             "if", "inline", "int", "long", "nullptr",
                                             "register", "restrict", "return", "short", "signed",
                                             "sizeof", "static", "static_assert", "struct", "switch",
                                             "thread_local", "true", "typedef", "typeof", "typeof_unqual",
                                             "union", "unsigned", "void", "volatile", "while"};
const vector<char> LexAnalysis::SEPARATOR = {'(', ')', '[', ']', '{', '}', ';', ',', '.', ':'};
const vector<char> LexAnalysis::OPERATOR_CHAR = {'[', ']', '(', ')', '.', '!', '~', '*', '/', '%',
                                                 '+', '-', '<', '>', '&', '^', '|', '=', '?', ':'};
const vector<string> LexAnalysis::OPERATOR_STRING = {"->", "++", "--", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||",
                                                     "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|="};
const vector<string> LexAnalysis::COMMENT = {"//", "/*", "*/"};
const vector<char> LexAnalysis::STRING = {'\'', '"'};
const string LexAnalysis::EMPTY_STRING = "";

LexAnalysis::LexAnalysis(const string &str)
{
    inputStr = str;
    curPos = inputStr.cbegin();
    curLine = 1;
    curStr = EMPTY_STRING;
    result = EMPTY_STRING;

    while (curPos != inputStr.cend())
    {
        if (isdigit(*curPos))
        {
            procLiteral();
        }
        else if (isalpha(*curPos) || *curPos == '_')
        {
            procIdentifier();
        }
        else if (isInSet(OPERATOR_CHAR, *curPos))
        {
            procOperator();
        }
        else if (isInSet(SEPARATOR, *curPos))
        {
            procSeparator();
        }
        else if (isInSet(STRING, *curPos))
        {
            procText();
        }
        else if (isspace(*curPos))
        {
            procSpace();
        }
        else
        {
            procOther();
        }
    }
}
bool LexAnalysis::procLiteral()
{
    bool isFloat = false;
    bool isExponent = false;

    do
    {
        curStr += *curPos;
        curPos++;
    } while (isdigit(*curPos));

    if (*curPos == '.')
    {
        isFloat = true;
        do
        {
            curStr += *curPos;
            curPos++;
        } while (isdigit(*curPos));
    }

    if (*curPos == 'e' || *curPos == 'E')
    {
        isExponent = true;
        curStr += *curPos;
        curPos++;
        if (*curPos == '+' || *curPos == '-')
        {
            curStr += *curPos;
            curPos++;
        }
        if (!isdigit(*curPos))
        {
            return false;
        }
        do
        {
            curStr += *curPos;
            curPos++;
        } while (isdigit(*curPos));
    }

    if (isFloat || isExponent)
    {
        setLineResult("Float");
    }
    else
    {
        setLineResult("Integer");
    }

    return true;
}

bool LexAnalysis::procIdentifier()
{
    do
    {
        curStr += *curPos;
        curPos++;
    } while (isalnum(*curPos) || *curPos == '_');

    if (isInSet(KEYWORD, curStr))
    {
        setLineResult("Keyword");
    }
    else
    {
        setLineResult("Identifier");
    }

    return true;
}

bool LexAnalysis::procOperator()
{
    curStr += *curPos;
    curPos++;

    if ((*(curPos - 1) == '+' || *(curPos - 1) == '-') && isdigit(*curPos))
    {
        procLiteral();
        return true;
    }

    if (*(curPos - 1) == '?')
    {
        bool isFound = false;
        for (string::const_iterator tmpPos = curPos - 1; tmpPos != inputStr.cend(); tmpPos++)
        {
            if (*tmpPos == ':')
            {
                curStr += *tmpPos;
                isFound = true;
                break;
            }
        }

        if (!isFound)
            return false;
    }
    else if (*(curPos - 1) == ':')
    {
        bool isFound = false;
        for (string::const_iterator tmpPos = inputStr.cbegin(); tmpPos != curPos - 1; tmpPos++)
        {
            if (*tmpPos == '?')
            {
                isFound = true;
                break;
            }
        }
        if (isFound)
            return true;
    }

    if (isInSet(OPERATOR_STRING, curStr + *curPos))
    {
        curStr += *curPos;
        curPos++;
        if (isInSet(OPERATOR_STRING, curStr + *curPos))
        {
            curStr += *curPos;
            curPos++;
        }
    }
    else if (isInSet(COMMENT, curStr + *curPos))
    {
        procComment();
        return true;
    }

    setLineResult("Operator");
    return true;
}
bool LexAnalysis::procComment()
{
    curStr += *curPos;

    if (curStr == "//")
    {
        do
        {
            curPos++;
        } while (*curPos != '\n');
        curPos++;
        curLine++;
    }
    else if (curStr == "/*")
    {
        do
        {
            curPos++;
            if (*curPos == '\n')
            {
                curLine++;
            }
            if (*curPos == '*' && *(curPos + 1) == '/')
                break;
        } while (*curPos != EOF);
        if (*curPos == EOF)
        {
            return false;
        }
        else
        {
            curPos += 2;
        }
    }
    else
    {
        return false;
    }

    curStr = EMPTY_STRING;
    return true;
}

bool LexAnalysis::procSeparator()
{
    curStr += *curPos;
    curPos++;
    setLineResult("Separator");
    return true;
}

bool LexAnalysis::procText()
{
    char c = *curPos;
    do
    {
        curStr += *curPos;
        curPos++;
    } while (*curPos != c);

    curStr += *curPos;
    curPos++;
    if (c == '\'')
        setLineResult("Char");
    else if (c == '"')
        setLineResult("String");
    else
        return false;
    return true;
}

bool LexAnalysis::procSpace()
{
    do
    {
        if (*curPos == '\n')
            curLine++;
        curPos++;
    } while (isspace(*curPos));
    return true;
}

bool LexAnalysis::procOther()
{
    curStr += *curPos;
    curPos++;
    return false;
}

template <typename T>
bool LexAnalysis::isInSet(const vector<T> &set, T c)
{
    for (auto i : set)
        if (c == i)
            return true;

    return false;
}
void LexAnalysis::setLineResult(const string &str)
{
    result += "line" + to_string(curLine) + ": (" + str + ", " + curStr + ")\n";
    curStr = EMPTY_STRING;
}
string LexAnalysis::getResult() const { return result; }