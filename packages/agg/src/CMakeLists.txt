include_directories("../include")

# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# For a large number of source files you can create it in a simpler way
# using file() function:
file(GLOB SRC *.cpp)

ADD_LIBRARY(agg STATIC ${SRC})

