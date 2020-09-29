# Default Shipped Interfaces

### Index

* `MM::Services::SceneServiceInterface`


## SceneServiceInterface

Not very stable. But I use it all over the place.

It uses [`EnTT`](https://github.com/skypjack/entt).

### interface

Use `.getScene()` to get current `MM::Scene`.

Use `.changeScene(newScene)` to queue `.changeSceneNow(newScene)`.

Use `.changeSceneNow(newScene)` to change currently held `MM::Scene`.

Use `.addSystemToScene(fn)` to add a `MM::System` to the `MM::Scene`.

