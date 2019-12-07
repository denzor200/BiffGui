#ifndef SRT_H
#define SRT_H

namespace SrtFormat {
    enum class MarkupType
    {
        square,
        round,
        braces,
        parentheses_any,
        caps
    };

    inline bool CheckMarkupType(MarkupType mt)
    {
        switch (mt)
        {
        case MarkupType::square:
        case MarkupType::round:
        case MarkupType::braces:
        case MarkupType::parentheses_any:
        case MarkupType::caps:
            return true;
        }
        return false;
    }
}

#endif // SRT_H
