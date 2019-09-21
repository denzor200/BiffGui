#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QString>

class CommandLineParser
{
    int m_Argc = 0;
    char** m_Argv = nullptr;
public:
    CommandLineParser(const QString& Value);
    CommandLineParser(const char* Value, int Size);
    ~CommandLineParser();

    int argc() const {return m_Argc;}
    char** argv() const {return m_Argv;}

private:
    enum class ParseStatus
    {
        SUCCESS = 0,
        OUT_OF_MEMORY
    };

    ParseStatus Initialize(const char* Value, int Size);

    static ParseStatus ParseTo(char** ArgvOut, int& ArgcOut, const char* CL, int CL_Size);
    static ParseStatus ParseArgumentTo(int& StrSizeOut,char** ArgOut, const char* CL, int CL_Size);
    static void TryFreeAll(char** Argv, int Argc);

    static void HandleStatus(ParseStatus Status);
};

#endif // COMMANDLINEPARSER_H
