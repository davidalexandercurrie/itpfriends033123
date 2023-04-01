OscIn oin;
OscMsg msg;
1234 => oin.port;
oin.addAddress( "foo" );
oin.addAddress( "test" );
oin.addAddress( "n1" );
oin.addAddress( "n2" );
oin.addAddress( "n3" );
oin.addAddress( "n4" );
oin.addAddress( "n5" );
oin.addAddress( "p" );
oin.addAddress( "seq" );
oin.addAddress( "movement" );
oin.addAddress( "box1" );
oin.addAddress( "box2" );
oin.addAddress( "box3" );
oin.addAddress( "box4" );
oin.addAddress( "box5" );
0 => int box1;
0 => int box2;
0 => int box3;
0 => int box4;
0 => int box5;
0 => int sequencer;
0.125 => float beat;

JCRev r => Gain g => dac;


// sound file
me.sourceDir() + "sounds/Kick01.wav" => string filename;
<<<me.sourceDir()>>>;
// the patch 
SndBuf kick => dac;
// load the file
filename => kick.read;
0 => kick.rate;
0 => kick.loop;
// sound file
me.sourceDir() + "sounds/Clap01.wav" => string filename2;
<<<me.sourceDir()>>>;
// the patch 
SndBuf clap => r;
// load the file
filename2 => clap.read;
0 => clap.rate;
0 => clap.loop;
me.sourceDir() + "sounds/Tom01.wav" => string filename3;
<<<me.sourceDir()>>>;
// the patch 
SndBuf tom => r;
// load the file
filename3 => tom.read;
0 => tom.rate;
0 => tom.loop;




r.mix(0.3);
float data[1][1];
string commands[0][0];
[0.5] @=> data["rev"];
[0.5] @=> data["level"];
[1.0] @=> data["speed"];
[0.0] @=> data["kill"];
[0.125] @=> data["beat"];

[48.0, 0.5, 0.25] @=> data["n1"];
[42.0, 0.5, 0.125] @=> data["n2"];
[49.0, 0.5, 0.125] @=> data["n3"];
[60.0, 0.5, 0.25] @=> data["n4"];
[37.0, 0, 0.125] @=> data["n5"];

spork ~ atmos();

// infinite event loop
while( true )
{
    // wait for event to arrive
    oin => now;
    1::samp => now;
    // grab the next message from the queue. 
    
    while( oin.recv(msg) )
    {
        // print stuff

        
        cherr <= "received OSC message: \"" <= msg.address <= "\" "
              <= "typetag: \"" <= msg.typetag <= "\" "
              <= "arguments: " <= msg.numArgs() <= IO.newline();
        <<< msg.address >>>;
        if(msg.address == "movement"){
            <<< "it moved " + msg.getInt(0) + " " + msg.getInt(1) >>>;
            spork ~ moveSound(msg.getInt(0), msg.getInt(1));
        }
        if(msg.address == "box1"){
            if(msg.getInt(0) == 1){
                1 => box1;
                0 => sequencer;
            }else{
                0 => box1;
            }
            <<< "box1 " + box1 >>>;
        }
        if(msg.address == "box2"){
            if(msg.getInt(0) == 1){
                0 => sequencer;
                1 => box2;
            }else{
                0 => box2;
            }
            <<< "box2 " + box2 >>>;
        }
        if(msg.address == "box3"){
            if(msg.getInt(0) == 1){
                0 => sequencer;
                1 => box3;
            }else{
                0 => box3;
            }
            <<< "box3 " + box3 >>>;
        }
        if(msg.address == "box4"){
            if(msg.getInt(0) == 1){
                0 => sequencer;
                1 => box4;
            }else{
                0 => box4;
            }
            <<< "box4 " + box4 >>>;
        }
        if(msg.address == "box5"){
            if(msg.getInt(0) == 1){
                0 => sequencer;
                1 => box5;
            }else{
                0 => box5;
            }
            <<< "box5 " + box5 >>>;
        }
        if(msg.address == "n1" || msg.address == "n2" || msg.address == "n3" || msg.address == "n4" || msg.address == "n5"){
            if(msg.getString(0) == "debug\n"){
                <<< "debug" >>>;
                debug(msg.address);
                break;
            }else if(msg.getString(0) == "repeat\n"){
                <<< "repeating saved note" >>>;
                // data[msg.address] @=> float randFreq[];
                [data[msg.address][0], data[msg.address][1], data[msg.address][2]] @=> float randFreq[];
                randFreq[0] * Math.random2f(0.99, 1.01) => randFreq[0];
                playNote(randFreq);
                break;
            }else{
                <<< "saving note" >>>;
                saveData(msg);
                playNote(data[msg.address]);
            }
        }else if(msg.address == "p"){
                <<< "command saved" >>>;
                [0.0] @=> data[msg.getString(0)];
                3.5 => data[msg.getString(0)][0];
                if(msg.getInt(1) != 0){
                    msg.getInt(1) * 1.0 => data[msg.getString(0)][0];
                }else {
                    msg.getFloat(1) => data[msg.getString(0)][0];
                }
                chout <= msg.getString(0) <= " " <= data[msg.getString(0)][0] <= IO.newline();
        }else if(msg.address == "seq"){
                 if(msg.getString(0) == "debug\n"){
                <<< "debug" >>>;
                debug(msg.address);
                break;
                 }else {
                    saveData(msg);
                    spork ~ playSeq(data[msg.address]);
                 }
        }
        
        
        params();
    }
}



fun void params(){
    data["rev"][0] => r.mix;
    data["level"][0] => g.gain;
    data["beat"][0] => beat;
}

fun void debug(string address){
    chout <= "debug: " <= address <= IO.newline();
   
    if(data[address] != NULL){
        for(0 => int i; i < data[address].size(); i++){
            chout <= data[address][i] <= IO.newline();
        }
    }else {
        chout <= "null" <= IO.newline();
    }
}

fun void saveData(OscMsg msg){
    [0.0] @=> data[msg.address];
        msg.numArgs() => data[msg.address].size;
        for(0 => int j; j < msg.numArgs(); j++){
            if(msg.getInt(j) != 0){
                msg.getInt(j) * 1.0 => data[msg.address][j];
            }
            else if(msg.getFloat(j) != 0){
                msg.getFloat(j) => data[msg.address][j];
            }
        }
        for(0 => int i; i < data.size(); i++){
            chout <= data[msg.address][i] <= IO.newline();
        }
        <<< "data saved" >>>;
}

fun void playNote(float noteValues[]){
    spork ~ note(noteValues[0], noteValues[1], noteValues[2]);
}

fun void note(float freq, float amp, float duration){
    if(amp > 0.5){
        0.5 => amp;
    }
    <<< "playing note", freq, amp, duration >>>;
    if(!box3){
        SinOsc s => r;
    Std.mtof(freq) => s.freq;
    amp => s.gain;
    (duration / data["speed"][0])::second => now;
    0 => s.gain;
    <<< "done playing note" >>>;
    }else {
        SawOsc s => r;
    Std.mtof(freq) => s.freq;
    amp => s.gain;
    (duration / data["speed"][0])::second => now;
    0 => s.gain;
    <<< "done playing note" >>>;
    }
    
}

fun void playSeq(float seq[]){
    for(0 => int j; j < seq[0]; j++){
        if(data["kill"][0] == 1){
            break;
        }
        for(1 => int i; i < seq.size(); i++){
            if(data["kill"][0] == 1){
                break;
            }
            if(seq[i] == 1){
                note(data["n1"][0], data["n1"][1], data["n1"][2]);
            }else if(seq[i] == 2){
                note(data["n2"][0], data["n2"][1], data["n2"][2]);
            }else if(seq[i] == 3){
                note(data["n3"][0], data["n3"][1], data["n3"][2]);
            }else if(seq[i] == 4){
                note(data["n4"][0], data["n4"][1], data["n4"][2]);
            }else if(seq[i] == 5){
                note(data["n5"][0], data["n5"][1], data["n5"][2]);
            }
        }
    }
}

fun void moveSound(int x, int y){
    SawOsc s => HPF hpf => r; 
    s.freq(40);
    s.gain(0.25);
    hpf.freq(x*y);
    0.125::second => now;
}


fun void atmos(){

    SawOsc s => r;
    s.gain(0.0);
    SawOsc s2 => r;
    s2.gain(0.0);
    42.0 => float b1freq;
    53.0 => float b1freq2;
    [42.0, 53.0, 40.0, 53.0, 57.0] @=> float notes[];
    while(true){
        if(box1){
            <<< "box1" >>>;
            Std.mtof(b1freq) * Math.random2f(0.98, 1.02) * Math.random2(1,3) => s.freq;
            if(Math.random2f(0, 1) > 0.9){
                notes[Math.random2(0, notes.size()-1)] => b1freq;
            }
            if(Math.random2f(0, 1) > 0.9){
                notes[Math.random2(0, notes.size()-1)] => b1freq2;
            }
            if(Math.random2f(0, 1) > 0.8){
                s.freq() * 2 => s.freq;
            }
            s.gain(Math.random2f(0, 0.25));
            Std.mtof(b1freq+15) * Math.random2f(0.98, 1.02) * Math.random2(1,2) => s2.freq;
            s2.gain(Math.random2f(0, 0.25));
            if(Math.random2f(0, 1) > 0.8){
                s.gain(0);
            }
            if(Math.random2f(0,1)> 0.8){
                s2.gain(0);
            }
            Math.random2f(0, 0.8) => float chance;
            beat::second => now;
        }else if(box2){
            <<< "box2" >>>;
            kick.pos(0);
            kick.rate(Math.random2f(0.9, 1.2));
            kick.play();
            beat::second => now;
        }else if(box3){
            <<< "box3" >>>;
            beat::second => now;
        }else if(box4){
            <<< "box4" >>>;
            kick.rate(Math.random2f(0.9, 1.1));
            kick.pos(0);
            kick.play();
            if(sequencer > 16 && sequencer % 4 == 3){
                clap.rate(Math.random2f(0.9, 1.2));
                clap.pos(0);
                clap.play();
            }
            beat*4::second => now;
        }else if(box5){
            <<< "box5" >>>;
            tom.pos(0);
            tom.play();
            tom.rate(Math.random2f(0.5, 1.5));
            beat::second => now;
        }
        else{
            1::second => now;
            s.gain(0);
            s2.gain(0);
        }
        sequencer++;
        <<< "sequencer", sequencer >>>;
    }
}