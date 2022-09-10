# HashMap
Just a bit of fun, there is probably memory leaking all over the place

It does work well though

It's all inline so HashMap.cpp is empty, all the implementation is over in HashMap.h & Hash.h

For a type to be used as a key, it must specialize Dee::Hash

```cpp
namespace Dee {

template<>
class Hash<MyType>{
public:
    size_t compute(MyType t){
        /* compute something that will generate a consistent and unique hash... */
        return 0;
    }
}

}
```

To change the default size and default re-allocation size redefine `DEE_HASHMAP_INITIAL_SIZE`
