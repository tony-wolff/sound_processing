#include <stdio.h>
#include "midifile.h"

void read_midi(char * midifile);

int main (int argc, char *argv[])
{
  read_midi(argv[1]);

}

char* display_note(int c)
{
    switch(c){
        case 0:
            return "C";
        case 1:
            return "C#";
        case 2:
            return "D";
        case 3:
            return "D#";
        case 4:
            return "E";
        case 5:
            return "F";
        case 6:
            return "F#";
        case 7:
            return "G";
        case 8:
            return "G#";
        case 9:
            return "A";
        case 10:
            return "A#";
        case 11:
            return "B";
        default:
            return "0";
    }
}
void read_midi(char * midifile)
{
  MidiFileEvent_t event;
  unsigned char *data;
  MidiFile_t md = MidiFile_load(midifile);
  event = MidiFile_getFirstEvent(md);

  /* boucle principale */
  while(event)
    {
        if(MidiFileEvent_isNoteStartEvent(event)){
            if(MidiFileNoteOnEvent_getChannel(event) != 10){
                //MidiFileNoteOnEvent_setNote(event, 0);
                int note = MidiFileNoteOnEvent_getNote(event);
                int velocity = MidiFileNoteOnEvent_getVelocity(event);
                long t = MidiFileEvent_getTick(event);
                long t2 = MidiFileEvent_getTick(MidiFileNoteStartEvent_getNoteEndEvent(event));
                printf("%s %d\n", display_note(note%12), note/12);
                printf("%f\n", MidiFile_getBeatFromTick(md, t2-t));
            }
        }
      // A completer : 
      // affichage du nom des notes
      // affichage de la durée des notes
      event = MidiFileEvent_getNextEventInFile(event);
    }
}