aubio onset "./input/SushiRestaurantShortBass.mp3" -t 0.6 -s -45 > rhythmInstrument1Onsets.txt
aubio notes "./input/SushiRestaurantShortBass.mp3" -s -45 > pitchedInstrument1NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument1NotesDirty.txt > pitchedInstrument1Notes.txt

aubio notes "./input/SushiRestaurantShortReverseSince.mp3" -s -45 > pitchedInstrument2NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument2NotesDirty.txt > pitchedInstrument2Notes.txt

