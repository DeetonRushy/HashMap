#pragma once

#include <string>

namespace Dee {

    template<class T>
    class Hash {
    public:
        virtual size_t compute(T) = 0;
    };

    template<>
    class Hash<int> {
    public:
        size_t compute(int Val) {
            return static_cast<size_t>(Val);
        }
    };

    template<>
    class Hash<long> {
    public:
        size_t compute(long Val) {
            return static_cast<size_t>(Val);
        }
    };

    template<>
    class Hash<const char*> {
    public:
        size_t compute(const char* Val) {
            size_t s = 0;
            size_t length = strlen(Val);

            for (int i = 0; i < length; i++) {
                s += static_cast<size_t>(static_cast<char>(*(Val + i)));
            }

            return s;
        }
    };

    template<>
    class Hash<std::string>
    {
    public:
        size_t compute(std::string const& Val)
        {
            size_t s = 0;
            for (int i = 0; i < Val.size(); i++) {
                s += static_cast<size_t>(Val[i]);
            }
            return s;
        }
    };
}