# Compiles all separate .c examples in /src as individual targets inside /bin

cmake_minimum_required(VERSION 3.10)

project(beej)

# Find all .c files in the src directory
file(
  GLOB_RECURSE # Match the given regex, "src/*.cpp"
  SOURCES # Store matched filenames in variable SOURCES
  "src/*.c"
)

# Loop through each source file and create a target for it
foreach(SOURCE_FILE ${SOURCES})
    # Get the filename without the path and extension
    get_filename_component(FILENAME ${SOURCE_FILE} NAME_WE)

    # Create an executable target for each source file
    add_executable(${FILENAME} ${SOURCE_FILE})
endforeach()
