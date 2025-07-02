cmake_minimum_required(VERSION 3.10)

# Set the project name
project(SnakeGame)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Find the SDL2 library
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Add the terminal version executable
#add_executable(main
#    main.c
#    snake.c
#)

# Add the graphic version executable
add_executable(graphic
    graphic.c
    snake.c
    utils.c
)

# Link the SDL2 and math libraries to the graphic version
target_link_libraries(graphic ${SDL2_LIBRARIES} m)
