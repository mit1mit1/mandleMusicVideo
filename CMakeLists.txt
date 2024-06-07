project("mandelMusicalZoom")

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(craigsapp-midifile/include)
add_executable(mandelzoom mandelzoom.cpp aubioParser.cpp colors.cpp lodepng.cpp math.cpp art.cpp videoframe.cpp craigsapp-midifile/src/Binasc.cpp craigsapp-midifile/src/MidiEvent.cpp craigsapp-midifile/src/MidiEventList.cpp craigsapp-midifile/src/MidiFile.cpp craigsapp-midifile/src/MidiMessage.cpp craigsapp-midifile/src/Options.cpp)