# FelineLogger
A simple logger that also allows to read logs

## install
To install FelineLogger use the [catcaretaker](https://github.com/labricecat/catcaretaker) and execute the following:
```
$ catcare get catcaretaker/felinelogger
```

## example
This is a basic example of how the logger works:
```cpp
#include "catmods/felinelogger/felinelogger.hpp"

int main() {
    // A new logger for the file "out.log"
    FelineLogger logger("out.log");

    // Clear the log file
    logger.clear();

    // Logs something under the name "Info"
    logger.log("Info","First log!");
    // Logs something without any name
    logger.log("Regular log");

    // Set the default name for logs without any name specification
    logger.name("LoggerTest");

    // Changes to the file "out2.log"
    logger.file("out2.log");

    // Reads in the logs
    auto logs = logger.read_log();

    // Iterate through the logs
    for(auto i : logs) {
        std::cout << i.name << " -> " << i.message << "\n";
        // Using C's `tm` struct
        // Set are:
        // - tm_hour, tm_min, tm_sec, tm_year, tm_mon, tm_day
        std::cout << i.to_tm().tm_hour << "\n";
    }
}
```
