#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h" // from URI of LV2 specification http://lv2plug.in/ns/lv2core

#include "sndfile.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#define IMPORT_DAW_BAE_URI "http://adfxz.com/plugins/importDAWBAE"

//ports are referred to by index, the order of which defined
typedef enum {
        DAW_MONO_INPUT  = 0,
        DAW_MONO_OUTPUT = 1,
        DAW_STEREO_INPUT = 2,
        DAW_STEREO_OUTPUT = 3,
} PortIndex;

//where data associated with the plugin is stored
//depends on index
typedef struct {
        // Port buffers
        float* input[2];
        float* output[2];
        
         // Instantiation settings
        uint32_t n_channels;
        double   rate;
        
} ImporterDAWBAE;


//function called by host to create a new plugin interface
//The host passes the plugin descriptor, sample rate, and bundle path for plugins that need to load additional resources (e.g. waveforms)
static LV2_Handle instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
		
	(void)descriptor;   // Unused variable
	(void)bundle_path;  // Unused variable

	ImporterDAWBAE* importer = (ImporterDAWBAE*)malloc(sizeof(ImporterDAWBAE));
	
	if(!importer)
	{
		return NULL;
	}
	// Decide which variant to use depending on the plugin URI
	if (!strcmp(descriptor->URI, IMPORT_DAW_BAE_URI "#Stereo")) 
	{
			importer->n_channels = 2;
	} 
	else if (!strcmp(descriptor->URI, IMPORT_DAW_BAE_URI "#Mono")) 
	{
			importer->n_channels = 1;
	} 
	else 
	{
		free(importer);
		return NULL;
	}
	
	importer->rate = rate;

	return (LV2_Handle)importer;
}

//function called by host to connect a particular port to a buffer
//The plugin must store the data location, but data may not be accessed except in run().
//called in the same context as run
static void connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	ImporterDAWBAE* importer = (ImporterDAWBAE*)instance;

	switch ((PortIndex)port) 
	{
		case DAW_MONO_INPUT:
			importer->input[0] = (float*)data;
			break;
		case DAW_MONO_OUTPUT:
			importer->output[0] = (float*)data;
			break;
		case DAW_STEREO_INPUT:
			importer->input[1] = (float*)data;
			break;
		case DAW_STEREO_OUTPUT:
			importer->output[1] = (float*)data;
			break;
	}
}

//The activate() method is called by the host to initialise and prepare the plugin instance for running. 
//The plugin must reset all internal state except for buffer locations set by connect_port(). 
//Since this plugin has no other internal state, this method does nothing.
//This method is in the “instantiation” threading class, so no other methods on this instance will be called concurrently with it.
static void activate(LV2_Handle instance)
{
	//do nothing
}

std::string ReadResourcesDirPathFromSettingsFile()
{
	//For reading text file
    //Creating ifstream class object
    std::string inputFilePath = "/home/pab/.lv2/import-DAW-BAE.lv2/settings.txt";
    std::ifstream in_file {inputFilePath};
	
	std::string param_description;
	std::string resources_dir_path = "";
    if(in_file.is_open())
    {
		std::cout << "\nRead settings.txt file successfully!\n";
        //Assuming we know what is & how it is stored in the file
        //Using formatted technique
        
        getline(in_file, param_description); 
        getline(in_file, resources_dir_path);
        
        std::cout << "\n resources dir file:" << resources_dir_path << std::endl;
    } 
    else 
    {
        printf("\nCannot read settings.txt file!\n");
    }
 
    //Closing file
    in_file.close();
    
    return resources_dir_path;
}


#define		SAMPLE_RATE			44100
//The run() method is the main process function of the plugin. 
//It processes a block of audio in the audio context. 
//Since this plugin is lv2:hardRTCapable, run() must be real-time safe, 
//so blocking (e.g. with a mutex) or memory allocation are not allowed.

static void run(LV2_Handle instance, uint32_t n_samples)
{
        const ImporterDAWBAE* importer = (const ImporterDAWBAE*)instance;
        
        std::string resources_dir_path = ReadResourcesDirPathFromSettingsFile();
        
        std::cout << "\nresources dir:" << resources_dir_path << std::endl;
        //if(resources_dir_path == ""){return;}
        
        std::string filename = resources_dir_path +  std::string("/imported_audio_DAW.wav");
        
        std::cout << "\nfilename of export: " << filename << std::endl;
        
        //imposing export output restrictions
		SF_INFO sfinfo;
		
		sfinfo.frames = n_samples*importer->n_channels;
		std::cout << "\nsfinfo.frames: " << sfinfo.frames << std::endl;
		sfinfo.channels = importer->n_channels; 
		std::cout << "\nsfinfo.channels: " << sfinfo.channels << std::endl;
		sfinfo.samplerate = importer->rate;
		std::cout << "\nsfinfo.rate: " << sfinfo.samplerate << std::endl;
		sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_16 ); 
		std::cout << "\nsfinfo.format: " << sfinfo.format << std::endl;
		
		SNDFILE * outFile;

		// Open the stream file
		if (! ( outFile = sf_open (filename.c_str(), SFM_WRITE, &sfinfo)))
		{	
			std::cout << "Not able to open file for writing " << outFile << std::endl;
			puts (sf_strerror (NULL)) ;
			return;
		} 
		
		std::vector <float> exportOut;
		exportOut.resize(n_samples);
		
		//if data has only 1 channel
		if(importer->n_channels == 1)
		{
			std::cout << "Exporting mono track...\n";
			
			float* mono_input  = importer->input[0];
			float* mono_output = importer->output[0];
			
			for (uint32_t pos = 0; pos < n_samples; pos++) 
			{
				//mono_output[pos] = mono_input[pos];	
				exportOut[pos] = mono_input[pos];
			}
			
			std::cout << "Finished exporting mono track!\n";
		}
		//else if data has 2 channels
		else if(importer->n_channels == 2)
		{
			std::cout << "Exporting stereo track...\n";
			
			float* stereo_input_left  = importer->input[0];
			float* stereo_input_right  = importer->input[1];
			
			float* stereo_output = importer->output[1];
			
			for (uint32_t pos = 0; pos < n_samples; pos = pos + 2) 
			{
				//stereo_output[pos] = stereo_input[pos];	
				exportOut[pos] = stereo_input_left[pos];
				exportOut[pos + 1] = stereo_input_right[pos];
			}
			
			std::cout << "Finished exporting stereo track!\n";
		}
		else
		{
			return;
		}
		
		
		//write data
		size_t readSize = n_samples;
		sf_count_t write_count = 0; 
		size_t count_buffer = 0;

		write_count = sf_write_float(outFile, &exportOut.front(), readSize);

		sf_close(outFile);
}


//The deactivate() method is the opposite of activate().
//It is called by the host after running the plugin. 
//It indicates that the host will not call run() again until another call to activate() 
//and is mainly useful for more advanced plugins with “live” characteristics such as those with auxiliary processing threads. 
//As with activate(), this plugin has no use for this information so this method does nothing.

//This method is in the “instantiation” threading class, so no other methods on this instance will be called concurrently with it.

static void deactivate(LV2_Handle instance)
{
	
}

//destroys a plugin instance
static void cleanup(LV2_Handle instance)
{
	free(instance);
}

//The extension_data() function returns any extension data supported by the plugin. 
//Note that this is not an instance method, but a function on the plugin descriptor. 
//It is usually used by plugins to implement additional interfaces. This plugin does not have any extension data, so this function returns NULL.

//This method is in the “discovery” threading class, so no other functions or methods in this plugin library will be called concurrently with it.
static const void* extension_data(const char* uri)
{
	return NULL;
}

//Every plugin must define an LV2_Descriptor. 
//It is best to define descriptors statically to avoid leaking memory and non-portable shared library constructors and destructors to clean up properly.
static const LV2_Descriptor descriptor_mono = {
        IMPORT_DAW_BAE_URI "#Mono",
        instantiate,
        connect_port,
        activate,
        run,
        deactivate,
        cleanup,
        extension_data
};

static const LV2_Descriptor descriptor_stereo = {
        IMPORT_DAW_BAE_URI "#Stereo",
        instantiate,
        connect_port,
        activate,
        run,
        deactivate,
        cleanup,
        extension_data
};

//The lv2_descriptor() function is the entry point to the plugin library. 
//The host will load the library and call this function repeatedly with increasing indices 
//to find all the plugins defined in the library. 
//The index is not an indentifier, the URI of the returned descriptor is used to determine the identify of the plugin.

//This method is in the “discovery” threading class, so no other functions or methods in this plugin library will be called concurrently with it.
LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
        switch (index) 
        {
			case 0:  
				return &descriptor_mono;
			case 1:  
				return &descriptor_stereo;
			default: 
				return NULL;
        }
}
