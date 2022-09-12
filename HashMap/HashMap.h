#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <stdexcept>

#include "Hash.h"

#define DEE_HASHMAP_INITIAL_SIZE 64

namespace Dee {
    template <typename T>
    using is_specialized = std::is_default_constructible<T>;

    class out_of_memory : public std::exception {
    public:
        out_of_memory()
            : std::exception()
        {}

        out_of_memory(const char* msg)
            : std::exception(msg)
        {}
    };

    typedef struct {
        size_t hash;
        size_t index;
    } KeyMap;

    template<class Key, class Value>
    class HashMap
    {
        static_assert(is_specialized<Hash<Key>>::value, "specified key does not specialize Dee::Hash");

        Value* m_values{ nullptr };
        KeyMap* m_index_info{ nullptr };

        size_t m_count{ 0 };
        size_t m_allocated_cells{ 0 };

        std::vector<int> m_free_cells{};
    public:
        HashMap();
        ~HashMap();

        void insert(Key const&, Value const);
        void remove(Key const&);
        void clear();

        Value const& get(Key const&);

        bool has_key(Key const&);
        const size_t size() const noexcept {
            return m_count;
        }

    private:
        void check_needs_more_mem();
        // meant to be used after a malloc, if malloc fails
        // we're out of memory and theres not much to be done
        void panic(const char * msg) {
            fprintf(stderr, "Failed to allocate! - %s", msg);
            throw out_of_memory();
        }
        void log(const char* msg, ...) {
#if defined (_DEBUG)
            va_list args;
            __crt_va_start(args, msg);
            vfprintf(stderr, msg, args);
            __crt_va_end(args);
#endif
        }
    };

    template<class Key, class Value>
    inline HashMap<Key, Value>::HashMap()
        : m_allocated_cells(DEE_HASHMAP_INITIAL_SIZE)
    {
        m_values = (Value*)malloc(sizeof(Value) * DEE_HASHMAP_INITIAL_SIZE);
        m_index_info = (KeyMap*)malloc(sizeof(KeyMap) * DEE_HASHMAP_INITIAL_SIZE);
    }

    template<class Key, class Value>
    inline HashMap<Key, Value>::~HashMap()
    {
        if (m_values) {
            ::free(m_values);
        }

        if (m_index_info) {
            ::free(m_index_info);
        }
    }

    template<class Key, class Value>
    inline void HashMap<Key, Value>::insert(Key const& key, Value const value)
    {
        check_needs_more_mem();

        Hash<Key> hash_generator{};
        auto hash = hash_generator.compute(key);

        size_t index = 0;
        bool reused = false;

        if (m_free_cells.size() > 0) {
            auto free_cell = m_free_cells[0];
            m_free_cells.erase(std::next(m_free_cells.begin()));
            index = free_cell;
            reused = true;
            log("re-using old cell at index %zu\n", index);
        }
        else {
            log("no free'd cells, using new cell at %zu\n", m_count);
            index = m_count;
        }

        m_index_info[index] = { hash, index };
        m_values[index] = value;

        if (!reused) {
            ++m_count;
        }
    }

    template<class Key, class Value>
    inline void HashMap<Key, Value>::remove(Key const& key)
    {
        if (m_count == 0) {
            return;
        }

        const size_t hash = Hash<Key>().compute(key);
        for (int i = 0; i < m_allocated_cells; ++i) {
            KeyMap& ref = m_index_info[i];

            if (ref.hash != hash)
                continue;

            log("position %zu is being cleaned up", ref.index);

            m_free_cells.push_back(ref.index);
            ref.hash = 0;
            ref.index = 0;
        }
    }

    template<class Key, class Value>
    inline void HashMap<Key, Value>::clear()
    {
        ::free(m_values);
        ::free(m_index_info);

        m_values = (Value*)malloc(sizeof(Value) * DEE_HASHMAP_INITIAL_SIZE);
        m_index_info = (KeyMap*)malloc(sizeof(KeyMap) * DEE_HASHMAP_INITIAL_SIZE);

        m_allocated_cells = DEE_HASHMAP_INITIAL_SIZE;
        m_count = 0;
    }

    template<class Key, class Value>
    inline Value const& HashMap<Key, Value>::get(Key const& key)
    {
        const size_t hash = Hash<Key>().compute(key);
        for (int i = 0; i < m_allocated_cells; ++i) {
            const KeyMap& ref = m_index_info[i];
            if (ref.hash == hash) {
                return m_values[ref.index];
            }
        }
        throw std::invalid_argument("no such key-value pair");
    }

    template<class Key, class Value>
    inline bool HashMap<Key, Value>::has_key(Key const& key)
    {
        const size_t hash = Hash<Key>().compute(key);
        for (int i = 0; i < m_allocated_cells; ++i) {
            const KeyMap& ref = m_index_info[i];
            if (ref.hash == hash) {
                return true;
            }
        }
        return false;
    }

    template<class Key, class Value>
    inline void HashMap<Key, Value>::check_needs_more_mem()
    {
        // 4 allocations away from being fucked...
        if ((m_count - m_free_cells.size()) >= (m_allocated_cells - 4)) {
            // YES
            log("moving info: Old: 0x%p, 0x%p\n",
                m_values,
                m_index_info);

            auto* temp_values = static_cast<Value*>(::malloc(m_count * sizeof(Value)));
            if (!temp_values) {
                panic("failed to allocate memory for relocation");
            }
            ::memcpy(temp_values, m_values, (m_count * sizeof(Value)));

            auto* temp_keymap = static_cast<KeyMap*>(::malloc(m_count * sizeof(KeyMap)));
            if (!temp_keymap) {
                panic("failed to allocate memory for relocation");
            }
            ::memcpy(temp_keymap, m_index_info, (m_count * sizeof(KeyMap)));

            ::free(m_values);
            ::free(m_index_info);

            m_values = static_cast<Value*>
                (::malloc(
                    (m_count + DEE_HASHMAP_INITIAL_SIZE) * sizeof(Value)
                ));
            m_index_info = static_cast<KeyMap*>(
                ::malloc((m_count + DEE_HASHMAP_INITIAL_SIZE) * sizeof(KeyMap))
                );

            if (!m_values || !m_index_info) {
                panic("relocation failed, malloc failed");
            }

            ::memcpy(m_values, temp_values, (m_count * sizeof(Value)));
            ::memcpy(m_index_info, temp_keymap, (m_count * sizeof(KeyMap)));

            this->m_allocated_cells = (m_count + DEE_HASHMAP_INITIAL_SIZE);

            ::free(temp_values);
            ::free(temp_keymap);

            log("moved values & index info! New: 0x%p, 0x%p\n",
                m_values,
                m_index_info);
        }

        return;
    }

}

