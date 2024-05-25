aubio onset "./input/LoFiTake5Drums.wav" -t 0.6 -s -45 > ./output/rhythmInstrument1Onsets.txt

aubio notes "./input/LoFiTake5Bass.wav" -s -65 > ./output/pitchedInstrument1NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument1NotesDirty.txt > ./output/pitchedInstrument1Notes.txt

aubio notes "./input/LoFiTake5Lead.wav" -s -65 > ./output/pitchedInstrument2NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument2NotesDirty.txt > ./output/pitchedInstrument2Notes.txt

aubio notes "./input/LoFiTake5PowerChordStabs.wav" -s -65 > ./output/pitchedInstrument3NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument3NotesDirty.txt > ./output/pitchedInstrument3Notes.txt

