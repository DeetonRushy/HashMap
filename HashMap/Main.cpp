#include "HashMap.h"
#include "LockedItem.h"

#include <map>

#include <functional>
#include <chrono>
#include <iostream>

void measure_execution_time(const char* name, std::function<void()> fn)
{
    auto start = std::chrono::high_resolution_clock::now();
    fn();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast
        <std::chrono::microseconds>(end - start);

    std::wcout << "\"" << name << "\" took " << duration.count() << L" microseconds\n";
}

void func_takes_locked_item(const LockedItem<int>& count)
{
    if (!count.has_permission(PERM_READ)) {
        fprintf(stderr, "cannot read to item count.");
        return;
    }

    fprintf(stderr, "Item count: %i", count.read());
}

int main(int argc, char** argv)
{
    LockedItem<int> item(29);

    item.assign_permission(PERM_READ);

    func_takes_locked_item(item.const_reference());
}
