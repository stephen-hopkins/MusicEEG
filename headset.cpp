#include "headset.h"
#include <iostream>
#include "windows.h"

using namespace std;

Headset::Headset()
{
    if (EE_EngineConnect() != EDK_OK)
        cerr << "Emotiv Engine start up failed";
    trackPlaying = true;
}

Headset::~Headset()
{
    EE_EngineDisconnect();
}

void Headset::run()
{
    EmoEngineEventHandle event = EE_EmoEngineEventCreate();
    EmoStateHandle emoState	= EE_EmoStateCreate();
    unsigned int userID = 0;
    trackPlaying  = true;

    while (trackPlaying) {

        int state = EE_EngineGetNextEvent(event);

        // New event needs to be handled
        if (state == EDK_OK) {
            EE_Event_t eventType = EE_EmoEngineEventGetType(event);
            EE_EmoEngineEventGetUserId(event, &userID);

            // Log the EmoState if it has been updated
            if (eventType == EE_EmoStateUpdated) {
                EE_EmoEngineEventGetEmoState(event, emoState);
                const float timestamp = ES_GetTimeFromStart(emoState);
                cout << "New EmoState from user" << timestamp << userID << endl;
                logEmoState(emoState);
            }
        }
        else if (state != EDK_NO_EVENT) {
            cerr << "Internal error in Emotiv Engine!" << endl;
            break;
        }
        Sleep(1000);
    }

    EE_EmoStateFree(emoState);
    EE_EmoEngineEventFree(event);

    return;
}

void Headset::logEmoState(EmoStateHandle emoState)
{
    engagement << ES_AffectivGetEngagementBoredomScore(emoState);
    excitementST << ES_AffectivGetExcitementShortTermScore(emoState);
    excitementLT << ES_AffectivGetExcitementLongTermScore(emoState);
    frustration << ES_AffectivGetFrustrationScore(emoState);
    meditation << ES_AffectivGetMeditationScore(emoState);
    return;
}

void Headset::trackFinished() {
    trackPlaying = false;
}

void Headset::setUserArtistTrack(QString u, QString a, QString t)
{
    user = u;
    artist = a;
    track = t;
}
