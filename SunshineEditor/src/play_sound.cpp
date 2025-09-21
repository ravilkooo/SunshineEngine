/*==============================================================================
Play Sound Example
Copyright (c), Firelight Technologies Pty, Ltd 2004-2025.

This example shows how to simply load and play multiple sounds, the simplest 
usage of FMOD. By default FMOD will decode the entire file into memory when it
loads. If the sounds are big and possibly take up a lot of RAM it would be
better to use the FMOD_CREATESTREAM flag, this will stream the file in realtime
as it plays.

For information on using FMOD example code in your own programs, visit
https://www.fmod.com/legal
==============================================================================*/
#include "fmod.hpp"
#include "common.h"
#include <iostream>

int FMOD_Main()
{
    FMOD::System     *system;
    FMOD::Sound      *sound1, *sound2, *sound3;
    FMOD::Channel    *channel = 0;
    FMOD_RESULT       result;
    void             *extradriverdata = 0;
    
    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    /*

    std::cout << std::string(sound_path.begin(), sound_path.end()).c_str() << "\n";
    result = system->createSound(std::string(sound_path.begin(), sound_path.end()).c_str(), FMOD_DEFAULT, 0, &sound1);
    */


    std::wstring sound_path = ENGINE_ASSETS_DIR;
    sound_path = sound_path + L"Sounds/drumloop.wav";
    result = system->createSound(std::string(sound_path.begin(), sound_path.end()).c_str(), FMOD_DEFAULT, 0, &sound1);
    ERRCHECK(result);

    result = sound1->setMode(FMOD_LOOP_OFF);    /* drumloop.wav has embedded loop points which automatically makes looping turn on, */
    ERRCHECK(result);                           /* so turn it off here.  We could have also just put FMOD_LOOP_OFF in the above CreateSound call. */

    sound_path = ENGINE_ASSETS_DIR;
    sound_path = sound_path + L"Sounds/jaguar.wav";
    result = system->createSound(std::string(sound_path.begin(), sound_path.end()).c_str(), FMOD_DEFAULT, 0, &sound2);
    ERRCHECK(result);

    sound_path = ENGINE_ASSETS_DIR;
    sound_path = sound_path + L"Sounds/Pause.wav";
    result = system->createSound(std::string(sound_path.begin(), sound_path.end()).c_str(), FMOD_DEFAULT, 0, &sound3);
    ERRCHECK(result);

    /*
        Main loop
    */

    result = system->playSound(sound1, 0, false, &channel);
    ERRCHECK(result);
    result = system->playSound(sound2, 0, false, &channel);
    ERRCHECK(result);
    result = system->playSound(sound3, 0, false, &channel);
    ERRCHECK(result);

    do
    {
        Common_Update();

        result = system->update();
        ERRCHECK(result);

        {
            unsigned int ms = 0;
            unsigned int lenms = 0;
            bool         playing = 0;
            bool         paused = 0;
            int          channelsplaying = 0;

            if (channel)
            {
                FMOD::Sound *currentsound = 0;

                result = channel->isPlaying(&playing);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPaused(&paused);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }
               
                channel->getCurrentSound(&currentsound);
                if (currentsound)
                {
                    result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
                    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                    {
                        ERRCHECK(result);
                    }
                }
            }

            system->getChannelsPlaying(&channelsplaying, NULL);
        }

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    result = sound1->release();
    ERRCHECK(result);
    result = sound2->release();
    ERRCHECK(result);
    result = sound3->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}
