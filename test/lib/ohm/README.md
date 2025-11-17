# ohm

![Test](https://github.com/mucbuc/ohm/workflows/Test/badge.svg)

Local message broker

## Examples

### Example 1
#### Build command
```
g++ -isystem test test/src/example.cpp -pthread -DTARGET_TEST=1 -std=c++17 -o tmp
```
#### Source code
```
#include <iostream>
#include <string>

#define ASSERT(p)

#include <lib/ohm/src/index.h>

int main()
{
    using namespace om636::control;
    using namespace std;

    auto e = make_emitter<string, int>();
    auto l = e->on("run", [](int v) {
        cout << "run " << v << endl;
    });
    e->interupt("run", 4);
    return 0;
}
```
#### Output
```
run 4
```
### Example 2
#### Build command
```
g++ -isystem test test/src/example2.cpp -pthread -DTARGET_TEST=1 -std=c++17 -o tmp
```
#### Source code
```
#include <iostream>
#include <string>

#define ASSERT(p)

#include <lib/ohm/src/index.h>

int main()
{
    using namespace om636::control;
    using namespace std;

    auto e = make_emitter<string, int>();
    auto l = e->on("run", [](int v) {
        cout << "run " << v << endl;
    });
    e->interupt("run", 322);
    return 0;
}
```
#### Output
```
run 322
```
## Interface
```
#pragma once

#include <functional>
#include <map>
#include <memory>
#include <set>

#include <lib/dynamo/src/interface.h>

namespace om636 {
namespace control {

    template <typename T, typename... U>
    class Emitter {
    public:
        typedef T event_type;
        typedef std::function<void(U...)> callback_type;
        typedef std::shared_ptr<callback_type> listener_type;

        virtual ~Emitter() = default;
        virtual listener_type on(event_type, callback_type) = 0;
        virtual listener_type once(event_type, callback_type) = 0;
        virtual void interupt(event_type, U...) = 0;
    };

    template <typename T, typename... U>
    class Quemitter : public Emitter<T, U...> {

    public:
        typedef Emitter<T, U...> base_type;
        using typename base_type::callback_type;
        using typename base_type::event_type;
        using typename base_type::listener_type;

        virtual ~Quemitter() = default;
        virtual void emit(event_type, U...) = 0;
    };

    template <typename T, typename... U>
    std::shared_ptr<Emitter<T, U...>> make_emitter();

    template <typename T, typename... U>
    std::shared_ptr<Emitter<T, U...>> make_quemitter();

} //control
} // om636
```

