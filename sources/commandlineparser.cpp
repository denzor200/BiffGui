#include "commandlineparser.h"
#include <cstdlib>
#include <new>

// TODO: test it..

#define CHK_ALLOCATED(p) \
    if (!p) \
        throw std::bad_alloc();

// TODO: потестировать на пустой строке
CommandLineParser::CommandLineParser(const QString& Value)
{
    QByteArray utf8Value = Value.toUtf8();
    Initialize(utf8Value.data(), utf8Value.size());
}

CommandLineParser::CommandLineParser(const char *Value, int Size)
{
    Initialize(Value, Size);
}

CommandLineParser::~CommandLineParser()
{
    TryFreeAll(m_Argv, m_Argc);
    if (m_Argc > 0)
    {
        Q_ASSERT(m_Argv);
        free(reinterpret_cast<void*>(m_Argv));
    }
}

void CommandLineParser::Initialize(const char *Value, int Size)
{
    int ArgcTemp = 0;
    char** ArgvTemp = nullptr;

    // Просто узнаем размер массива строк..
    ParseTo(nullptr, ArgcTemp, Value, Size);
    Q_ASSERT(ArgcTemp>=0);
    if (ArgcTemp > 0)
    {

        // Аллоцируем память под массив строк
        ArgvTemp = reinterpret_cast<char**>(malloc(static_cast<size_t>(ArgcTemp)*sizeof(char**)));
        CHK_ALLOCATED(ArgvTemp);

        // Заполняем массив строк вторим вызовом
        try {
            ParseTo(ArgvTemp, ArgcTemp, Value, Size);
        } catch (...) {
            free(reinterpret_cast<void*>(ArgvTemp));
            throw;
        }

        // ..
        m_Argc = ArgcTemp;
        m_Argv = ArgvTemp;
    }

}

// Функция выполняет два действия..
// 1. Просто узнаем размер массива строк, ничего-ничего не трогая
// 2. Заполняем переданный свыше неинициализированный массив строк
void CommandLineParser::ParseTo(char ** ArgvOut, int &ArgcOut, const char *CL, int CL_Size)
{
    int TempArgc = 0;

    // Временная мера - все аргументы приходят заключенными в кавычки
    // Так гораздо легче огранизовать разбор этой строки на этой стороне..
    for (int i=0;i<CL_Size;++i)
    {
        if (CL[i]=='\"')
        {
            char* Arg = nullptr;
            int Size = 0;
            try {
                Size = ParseArgumentTo((ArgvOut != nullptr) ? &Arg : nullptr, &CL[i+1], CL_Size - i - 1);
            } catch (...) {
                if (ArgvOut)
                    TryFreeAll(ArgvOut, TempArgc);
                throw;
            }
            if (Size==-1)
                break; // если not found, то продолжать нет смысла
            if (ArgvOut != nullptr)
                ArgvOut[TempArgc] = Arg;
            TempArgc++;
            i += Size + 1;
        }
    }
    if (ArgvOut!=nullptr)
    {
        // Вторая итерация..
        // Размер ArgcOut уже известнен и строго должен совпадать!
        Q_ASSERT(ArgcOut == TempArgc);
    }
    ArgcOut = TempArgc;
}

int CommandLineParser::ParseArgumentTo(char** ArgOut, const char *CL, int CL_Size)
{
    int Size = -1;

    // Находим размер строки поиском в лоб
    for (int i=0;i<CL_Size;++i)
    {
        if (CL[i]=='\"')
        {
            Size = i;
            break;
        }
    }

    if (Size == -1)
        return -1; // not found..

    // Если требуется, то аллоцируем память под строку и копируем данные..
    if (ArgOut != nullptr)
    {
        *ArgOut = reinterpret_cast<char*>(malloc(static_cast<size_t>(Size + 1) * sizeof(char)));
        CHK_ALLOCATED(*ArgOut);
        memcpy(*ArgOut, CL, static_cast<size_t>(Size));
        *((*ArgOut)+Size) = '\0';
    }

    // ..Или просто возвращаем размер..
    return Size;
}

void CommandLineParser::TryFreeAll(char **Argv, int Argc)
{
    for (int i=0;i<Argc;++i)
    {
        Q_ASSERT(Argv);
        Q_ASSERT(Argv[i]);
        free(reinterpret_cast<void*>(Argv[i]));
    }
}
