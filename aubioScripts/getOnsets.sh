# https://aubio.org/manual/latest/

aubio onset "./input/PachabelBassStrings.wav" -t 0.6 -s -45 > ./output/rhythmInstrument1Onsets.txt

aubio notes "./input/PachabelFlute.wav" -s -40 > ./output/pitchedInstrument1NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument1NotesDirty.txt > ./output/pitchedInstrument1Notes.txt

aubio notes "./input/PachabelHarp.wav" -s -40 > ./output/pitchedInstrument2NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument2NotesDirty.txt > ./output/pitchedInstrument2Notes.txt

aubio notes "./input/PachabelHorn.wav" -s -40 > ./output/pitchedInstrument3NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument3NotesDirty.txt > ./output/pitchedInstrument3Notes.txt

aubio notes "./input/PachabelBassStrings.wav" -s -40 > ./output/pitchedInstrument4NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument4NotesDirty.txt > ./output/pitchedInstrument4Notes.txt

aubio notes "./input/PachabelCounterstrings.wav" -s -40 > ./output/pitchedInstrument5NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument5NotesDirty.txt > ./output/pitchedInstrument5Notes.txt

aubio notes "./input/PachabelMelodyStrings.wav" -s -40 > ./output/pitchedInstrument6NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument6NotesDirty.txt > ./output/pitchedInstrument6Notes.txt

aubio notes "./input/PachabelTenorstrings.wav" -s -40 > ./output/pitchedInstrument7NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < ./output/pitchedInstrument7NotesDirty.txt > ./output/pitchedInstrument7Notes.txt

