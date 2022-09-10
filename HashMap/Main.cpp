#include "HashMap.h"

#include <map>

int main(int argc, char** argv)
{
    Dee::HashMap<std::string, int> map{};

    map.insert("Willy", 1);

    if (map.has_key("Willy")) {
        fprintf(stderr, "The correct key was found!\n");
    }

    if (map.has_key("NotFound")) {
        fprintf(stderr, "The incorrect key was found??");
    }

    const auto& value = map.get("w");
    
    if (!value) {
        fprintf(stderr, "Failed to get value!");
    }
    else {
        fprintf(stderr, "Value: %i", value);
    }
}
