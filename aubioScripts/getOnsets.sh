aubio onset "./input/SushiRestaurantShortStrumming.mp3" -t 0.6 -s -45 > rhythmInstrument1Onsets.txt

aubio notes "./input/SushiRestaurantShortBass.mp3" -s -65 > pitchedInstrument1NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument1NotesDirty.txt > pitchedInstrument1Notes.txt

aubio notes "./input/SushiRestaurantShortMelody.mp3" -s -65 > pitchedInstrument2NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument2NotesDirty.txt > pitchedInstrument2Notes.txt

aubio notes "./input/SushiRestaurantShortFingerPicking.mp3" -s -65 > pitchedInstrument3NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument3NotesDirty.txt > pitchedInstrument3Notes.txt

