# Screen Director

Orchistrates `Screen`s.

The `Screen Director` lets you define "States"(`Screen`s) which activate/deactivate `MM::Service`s on Entering and Leaving the `Screen`.

# Usage

The current implementation is very open and does not really use encapsulation ...

## Adding `Screen`s

Due to the open nature of this implementation, one does access members directly:
```
auto& my_screen = screen_director.screens["my_screen"];

// filling in start_disable, start_enable, start_provide, end_disable and end_enable
// and optionally start_fn and end_fn which are called upon entering and leaving the screen.
my_screen.start_enable.push_back(engine.type<MyService>());
// [...]
```

## Changing to a Screen

Changing to a `Screen` can either be done on call with `.changeScreenTo(engine, "my_screen_id")` or qued via `.queueChangeScreenTo("my_screen")`. The former should only be used when the `Engine` is not running or in a `fixedDeferUpdate`.

## Service Enablement

Enabling the ScreenDirector causes a queued `Screen` to be started.


