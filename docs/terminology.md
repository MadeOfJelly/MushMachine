# Terminology

### Engine
is defined as [`MM::Engine`](framework/engine/engine.md).

### Service
is defined as [`MM::Services::Service`](framework/engine/services.md), the Service's base class, which was previously known as `ServiceSystem` and `SubSystem`.

### Scene
is defined as a ECS-style EnTT::registry. It contains Entities, Components and Systems.

### Entity
is defined as a ECS Entity, see [`EnTT`](https://github.com/skypjack/entt).

### System
is defined as a ECS-style System. In this case, it's a free function.

### Screen
is defined as a State of enabled and disabled `Service`s. (see [`ScreenDirector`](framework/screen_director/screen_director.md))

