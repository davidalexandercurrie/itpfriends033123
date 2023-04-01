# itpfriends033123

Performance for ITP & Friends

Playing around with game interface for creating music in combination with live coding

gcc -o osc_send osc_send.c -llo && ./osc_send

gcc -o game game.c -lncurses -llo && ./game

chuck --dac10 --srate44100 osc.ck

## Instructions you can send via osc_send

There are 5 notes.

### Set each one like so:

n1 60 0.5 2 // sets note 1 at midi node 60, gain 0.5, duration 2 secconds

n1 repeat // repeats the note

### Play a sequence of notes:

seq 10 1 2 3 // plays the sequence of n1 n2 n3 10 times

### Change Parameters

p speed 2 // doubles the playback speed

p speed 0.5 // halves the playback speed

p kill 1 // kills any sequences

p kill 0 // needs to be reset to 0 for notes to play again

sounds will be played when also running the game

moving with wasd will play notes

moving into the squares will activate different sounds/effects
