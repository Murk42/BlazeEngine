# Blaze Engine
This is a framework made to speed up C++ application development. It implements basic and advanced data structures as well as frequently needed functionalities such as image loading and saving, threading classes, logging system, file manipulation, etc. Everything is written in a consistent manner and in one place.

Blaze Engine is written in C++20 to leverage its latest features, e.g. concepts. It's written using a modular design, currently it consists of 4 libraries: 
- Blaze Engine Core - this library is standalone. It defines and implements the basic types, data classes, helper methods. Some examples include:
  - Math classes (general constexpr math functions, matrix, vectors, quaternions, 2d shape collision and manipulation)
  - String classes (generic string template classes with manipulation functions and full UTF-8/UTF-16/UTF-32 support, string view)
  - Containers (Array, List, Map, Set, Stack, Queue, View, Span, all with their custom iterators)
  - Formatting (custom string formatting implementation, using the exact same syntax as the fmt library)
  - Logging (logging to files, to the console, filtering logs, etc.)
  - Event system (groundwork for an event system)
- Blaze Engine - more complex and more platform-specific classes and systems are implemented in this library. It depends on SDL, glew, devIL... Some examples include:
  - Window class (window creation and manipulation)
  - Input handling (all OS events are handled through the event system where the users subscribe to listen to system wide events, or poll/wait for window specific ones)
  - Applicatin framework (helper classes that abstract different layers of the application into different threads and layers)  
- Blaze Engine Graphics - implements generic graphics classes and systems. Some examples include:
  - OpenGL OOP wrapper
  - Graphics context (class that encapsulates a Graphics API context, doesn't have to be OpenGL)
  - Renderer system (classes used to apstract the graphics-API-specific code)
- Blaze Engine UI - basic UI functionality with dynamic transform recalculation. Highly extensible. Some of the implemented UI elements: Label, Panel, Image, Button, TextField...

**Blaze_Engine is still under development and many things change very fast.**
