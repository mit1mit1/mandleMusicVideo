aubio onset "./input/wUDBB.mp3" -t 0.6 -s -45 > rhythmInstrument1Onsets.txt
aubio notes "./input/wUDPC.mp3" -s -45 > pitchedInstrument1NotesDirty.txt
# Notes file includes some notes without timestamps oddly
sed -e 's/[^[:space:][:cntrl:]]/&/15' -e t -e d < pitchedInstrument1NotesDirty.txt > pitchedInstrument1Notes.txt
