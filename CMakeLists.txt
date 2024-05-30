project("mandelMusicalZoom")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(mandelzoom mandelzoom.cpp aubioParser.cpp colors.cpp lodepng.cpp math.cpp art.cpp)