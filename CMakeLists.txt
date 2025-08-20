cmake_minimum_required(VERSION 2.8)
project("mandelMusicalZoom")


set(CMAKE_CXX_STANDARD 11)
set (CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/util/FindTBB
    ${CMAKE_MODULE_PATH})
add_compile_options (-fexperimental-library)
add_compile_options (-g)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(craigsapp-midifile/include)
# target_link_libraries(tbb)
add_executable(mandelzoom mandelzoom.cpp aubioParser.cpp colors.cpp lodepng.cpp math.cpp art.cpp videoframe.cpp craigsapp-midifile/src/Binasc.cpp craigsapp-midifile/src/MidiEvent.cpp craigsapp-midifile/src/MidiEventList.cpp craigsapp-midifile/src/MidiFile.cpp craigsapp-midifile/src/MidiMessage.cpp craigsapp-midifile/src/Options.cpp)
# -ltbb -DPARALLEL