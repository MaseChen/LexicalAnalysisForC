#ifndef LEX_ANALYSIS_H
#define LEX_ANALYSIS_H

#include <string>
#include <vector>

class LexAnalysis
{
public:
    LexAnalysis(const std::string &str);
    std::string getResult() const;

private:
    bool procLiteral();
    bool procIdentifier();

    bool procOperator();
    bool procComment();

    bool procSeparator();
    bool procText();
    bool procSpace();

    bool procOther();

    template <typename T>
    bool isInSet(const std::vector<T> &set, T c);

    void setLineResult(const std::string &str);

    static const std::vector<std::string> KEYWORD;
    static const std::vector<char> SEPARATOR;
    static const std::vector<char> OPERATOR_CHAR;
    static const std::vector<std::string> OPERATOR_STRING;
    static const std::vector<std::string> COMMENT;
    static const std::vector<char> STRING;
    static const std::string EMPTY_STRING;

    std::string inputStr;
    std::string::const_iterator curPos;
    size_t curLine;
    std::string curStr;
    std::string result;
};

#endif