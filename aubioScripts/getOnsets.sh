aubio onset "./input/testPiece1.m4a" -t 0.6 -s -45 > rhythmInstrument1Onsets.txt
aubio notes "./input/testPiece1.m4a" -s -45 > pitchedInstrument1NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument1NotesDirty.txt > pitchedInstrument1Notes.txt
