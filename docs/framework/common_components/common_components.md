(draft)
# Common Components

This is a collection of commonly used (ECS) `Components`.

Json serialization is provided for all `Components`.

## Name

Contains a string. For debugging.

It's special, since the entity widget will try to get the Name and display it in `ImGui`.

## Transform

Contians Positon, Rotation and Scale.
2D and 3D variants.

Might get replaced with a Unity3D style transform system...

## Velocity

Contians Velocity(Position and Rotation).
2D and 3D variants.

## View_Dir

In almost every Project I had the need for this `Component`, so it's here.
2D and 3D variants.


## Color

I use this a lot. I probably shouldn't.
Hase 4 color channels.

