# Logger

The Logger is the [`spdlog`](https://github.com/gabime/spdlog) logging library, with some extras. Like a `Sink` for [`Tracy`](https://github.com/wolfpld/tracy) the Profiler.

# Usage

Each "Section" is ment to have its own "Logger".

To Initialize a section logger with its `sink`s, do `MM::Logger::initSectionLogger("MySection");`.
Can be placed in the `.enable()` or even the Constructor of your `Service`s.

```cpp
#include <mm/logger.hpp> // include logger

// here are the macros
#define LOG_CRIT(...)		__LOG_CRIT(	"MySection", __VA_ARGS__)
#define LOG_ERROR(...)		__LOG_ERROR("MySection", __VA_ARGS__)
#define LOG_WARN(...)		__LOG_WARN(	"MySection", __VA_ARGS__)
#define LOG_INFO(...)		__LOG_INFO(	"MySection", __VA_ARGS__)
#define LOG_DEBUG(...)		__LOG_DEBUG("MySection", __VA_ARGS__)
#define LOG_TRACE(...)		__LOG_TRACE("MySection", __VA_ARGS__)

// code goes here ......

// only if in header
//#undef LOG_CRIT
//#undef LOG_ERROR
//#undef LOG_WARN
//#undef LOG_INFO
//#undef LOG_DEBUG
//#undef LOG_TRACE
```

