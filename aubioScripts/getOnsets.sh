aubio onset "./input/LoFiTake5Drums.wav" -t 0.6 -s -45 > rhythmInstrument1Onsets.txt

aubio notes "./input/LoFiTake5Bass.wav" -s -65 > pitchedInstrument1NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument1NotesDirty.txt > pitchedInstrument1Notes.txt

aubio notes "./input/LoFiTake5Lead.wav" -s -65 > pitchedInstrument2NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument2NotesDirty.txt > pitchedInstrument2Notes.txt

aubio notes "./input/LoFiTake5PowerChordStabs.wav" -s -65 > pitchedInstrument3NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument3NotesDirty.txt > pitchedInstrument3Notes.txt

