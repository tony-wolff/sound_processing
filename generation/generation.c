#include <stdio.h>
#include <stdbool.h>
#include "midifile.h"

void read_midi(char * midifile);

int main (int argc, char *argv[])
{
  read_midi(argv[1]);

}

int likelihood_matrix[12][12] = {0};

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
  int previous_note = 0;
  bool first_note=true;

  /* boucle principale */
  while(event)
    {
        if(MidiFileEvent_isNoteStartEvent(event)){
            if(MidiFileNoteOnEvent_getChannel(event) != 10){
                int note = MidiFileNoteOnEvent_getNote(event);
                if(first_note){
                    first_note = false;
                    previous_note = note;
                }
                else{
                    likelihood_matrix[previous_note%12][note%12]+=1;
                    previous_note = note;
                }
            }
        }
      event = MidiFileEvent_getNextEventInFile(event);
    }
    for(int i=0; i<12; i++){
        for(int j=0; j<12; j++){
            printf("%d ", likelihood_matrix[i][j]);
        }
        printf("\n");
    }
}

void modify_midi(char *midifile)
{
    MidiFileEvent_t event;
    unsigned char *data;
    MidiFile_t md = MidiFile_load(midifile);
    event = MidiFile_getFirstEvent(md);

    while(event)
    {
        if(MidiFileEvent_isnoteStartEvent(event) && MidiFileNoteOneEvent_getChannel(event) != 10){
            int note = MidiFileNoteOnEvent_getNote(event);
            int sum=0;
            for(int i=0; i<12; i++)
            {
                sum+=likelihood_matrix[note][i];
            }
        }
    }
}