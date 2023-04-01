// sound file
me.sourceDir() + "sounds/Kick01.wav" => string filename;
<<<me.sourceDir()>>>;


// the patch 
SndBuf buf => dac;
// load the file
filename => buf.read;
1 => buf.rate;
1 => buf.loop;

// time loop
while( true )
{
    1::samp => now;
}