#include "commandlineparser.h"
#include <cstdlib>
#include <new>

// TODO: test it..

#define CHK_ALLOCATED(p) \
    if (!p) \
        return ParseStatus::OUT_OF_MEMORY;

#define PS_SUCCESS(S) (S == ParseStatus::SUCCESS)
#define PS_FAILED(S) (S != ParseStatus::SUCCESS)

// TODO: потестировать на пустой строке
CommandLineParser::CommandLineParser(const QString& Value)
{
    QByteArray utf8Value = Value.toUtf8();
    auto Status = Initialize(utf8Value.data(), utf8Value.size());
    HandleStatus(Status);
}

CommandLineParser::CommandLineParser(const char *Value, int Size)
{
    auto Status = Initialize(Value, Size);
    HandleStatus(Status);
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

CommandLineParser::ParseStatus CommandLineParser::Initialize(const char *Value, int Size)
{
    ParseStatus Status = ParseStatus::SUCCESS;
    int ArgcTemp = 0;
    char** ArgvTemp = nullptr;

    // Просто узнаем размер массива строк..
    Status = ParseTo(nullptr, ArgcTemp, Value, Size);
    Q_ASSERT(PS_SUCCESS(Status));   // мы ничего не трогали!
                                    // всегда должен быть SUCCESS
    Q_ASSERT(ArgcTemp>=0);
    if (ArgcTemp > 0)
    {

        // Аллоцируем память под массив строк
        ArgvTemp = reinterpret_cast<char**>(malloc(static_cast<size_t>(ArgcTemp)*sizeof(char**)));
        CHK_ALLOCATED(ArgvTemp);

        // Заполняем массив строк вторим вызовом
        Status = ParseTo(ArgvTemp, ArgcTemp, Value, Size);
        if (PS_FAILED(Status))
        {
            free(reinterpret_cast<void*>(ArgvTemp));
            return Status;
        }

        // ..
        m_Argc = ArgcTemp;
        m_Argv = ArgvTemp;
    }
    return Status;
}

// Функция выполняет два действия..
// 1. Просто узнаем размер массива строк, ничего-ничего не трогая
// 2. Заполняем переданный свыше неинициализированный массив строк
CommandLineParser::ParseStatus CommandLineParser::ParseTo(char ** ArgvOut, int &ArgcOut, const char *CL, int CL_Size)
{
    ParseStatus Status = ParseStatus::SUCCESS;
    int TempArgc = 0;

    // Временная мера - все аргументы приходят заключенными в кавычки
    // Так гораздо легче огранизовать разбор этой строки на этой стороне..
    for (int i=0;i<CL_Size;++i)
    {
        if (CL[i]=='\"')
        {
            char* Arg = nullptr;
            int Size = 0;
            Status = ParseArgumentTo(
                        Size,
                        (ArgvOut != nullptr) ? &Arg : nullptr,
                        &CL[i+1], CL_Size - i - 1);
            if (PS_FAILED(Status))
            {
                if (ArgvOut)
                    TryFreeAll(ArgvOut, TempArgc);
                return Status;
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

    return Status;
}

CommandLineParser::ParseStatus CommandLineParser::ParseArgumentTo(int &StrSizeOut, char **ArgOut, const char *CL, int CL_Size)
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
    {
        StrSizeOut = -1; // not found..
        return ParseStatus::SUCCESS;
    }

    // Если требуется, то аллоцируем память под строку и копируем данные..
    if (ArgOut != nullptr)
    {
        *ArgOut = reinterpret_cast<char*>(
                    malloc(static_cast<size_t>(Size + 1) * sizeof(char)));
        CHK_ALLOCATED(*ArgOut);
        memcpy(*ArgOut, CL, static_cast<size_t>(Size));
        *((*ArgOut)+Size) = '\0';
    }

    // ..Или просто возвращаем размер..
    StrSizeOut = Size;
    return ParseStatus::SUCCESS;
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

void CommandLineParser::HandleStatus(CommandLineParser::ParseStatus Status)
{
    if (PS_FAILED(Status))
    {
        switch (Status)
        {
        case ParseStatus::OUT_OF_MEMORY:
            throw std::bad_alloc();
        }
        qFatal("Invalid ParseStatus");
        Q_ASSERT(0);
    }
}
