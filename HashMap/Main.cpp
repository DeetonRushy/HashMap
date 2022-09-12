#include "HashMap.h"

#include <map>

int main(int argc, char** argv)
{
    Dee::HashMap<int, int> map{};

    map.insert(69, 21);
    map.remove(69);

    map.clear();
}
