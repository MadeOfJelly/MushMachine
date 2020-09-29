# Engine

The Engine is the heart of the MushMachine framework.
It holds the Storage of all [`MM::Service`](services.md)s, update callbacks and is responsible for calling them.

# Usage

Provides a forwarding header `<mm/engine_fwd.hpp>`.

## Construction

The `MM::Engine` provides a Constructor which accepts a time interval, in which `.fixedUpdate()` is called. The Default is 1/60.

## Starting
```cpp
#include <mm/engine.hpp>

MM::Engine engine;

// add and enable MM::Services here

engine.run();
```

The `MM::Engine` disables all enabled `MM::Service`s in reverse order on destruction or `.cleanup()`.

## Managing [`MM::Service`](services.md)s

### Adding a `MM::Service`

Before a `MM::Service` can be enabled, it needs to be added to the engine.
`.addService<MyService>(my, args)` adds a service and forwards args to its constructor.
There is currently no way of removing a `MM::Service`.

### Getting a `MM::Service`

You can get a Pointer via `.tryService<MyService>()` and an asserted reference via `.getService<MyService>()`. If the `MM::Service` does not exist, it returns a `nullptr. "Enablement Status" makes no difference.

### Provide a `MM::Service` (Interfaces)

`.provide<MyServiceInterface, MyService>()` registers `MyService` also as `MyServiceInterface`, so `MyService` becomes getable via `MyServiceInterface`. `MyService` needs to be added beforehand.

### Enabling/Disabling a `MM::Service`

`MM::Service`s can be Enabled/Disabled during `.run()` as a deferred operation (see `.addFixedDefer(...)`)

* `.enableService<MyService>()` calles the `.enable()` of `MyService`. Also returns `true` on success.
* `.disableService<MyService>()` calles the `.disable()` of `MyService`.
There are also overloads, which accept a type id.

## Update / FixedUpdate

FixedUpdates are called with update interval (default 1/60, set on engine ctr), while Updates are called "as fast as possible". Since Rendering is usually done using Update and blocks on VSync, this is as fast as VSync.

Since fixedUpdate callbacks don't get a time delta, querying `.getFixedDeltaTime()` is mandatory.

### adding Update callbacks

`.addUpdate(MyFn)` and `.addFixedUpdate(MyFn)` add `MyFn` as an update and fixedUpdate respectively and return a `FunctionDataHandle`(ptr) to remove them again.

### removing Update callbacks

`.removeUpdate(myHandle)` and `.removeFixedUpdate(myHandle)` remove those callbacks again.

### `.run()`

`.run()` is a blocking call, which calles `.fixedUpdate()` in a fixed interval with an accumulator. Inbetween those `.fixedUpdate()`s, `.update()` gets called "as fast as possible".

You don't need to call `.run()`, instead you can call `.update()` and `.fixedUpdate()` yourself.

### `.stop()`

Stops the Engine...
Actually quits the loop in `.run()`.

