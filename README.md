# Import Audio DAW BAE

This is a plugin to use to conveniently export audio to Binaural Audio Editor sound tracks from your favorite DAW(digital audio work station). This way you can use advanced audio editing capabilities in your favorite DAW and spatialize the audio in Binaural Audio Editor. 

## How to install

### Linux

1. cd import-audio-DAW-BAE
2. mkdir build
3. cd build
4. cmake .. -DCMAKE_BUILD_TYPE=Release
5. make
6. cp -r import-DAW-BAE.lv2 ~/.lv2 


## How To Use

After installing the plugin, edit the file settings.txt in ~/.lv2/import-DAW-BAE.lv2.
Add the absolute path to the resources folder located in your binaural audio editor build.
It should look something like this your-build-dir/binaural-audio-editor/src/timeline-track-editor/resources.

Also open Audacity. Go to Effect->Add/Remove Plug-ins.
Enable the plugin 'Export Audio To Binaural Audio Editor'.

Then, select audio to export, simply go to Effects, and then choose 'Export Audio To Binaural Audio Editor'

Finally, simply click on the 'Import Audio DAW' button of the soundproducer track that you wish to have the audio exported from DAW.
