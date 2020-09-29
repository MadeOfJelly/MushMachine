# Services

`MM::Service`s are the Components that get added to the `MM::Engine`.

They need to extend `MM::Service` (included in `<mm/engine.hpp>`) and implement the following Interface:

* Destructor (since it is virtual)
* `bool enable(MM::Engine& engine)`
* `void disable(MM::Engine& engine)`

and optionally

* `const char* name(void)`

Not implementing `name()` does not impact functionality, but it is not good practise.

Update and FixedUpdate callbacks should only be added in `enable()` and should be removed in `disable()`.

