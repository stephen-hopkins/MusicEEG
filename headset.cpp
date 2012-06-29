#include "headset.h"
#include <iostream>
#include "windows.h"
#include "meexception.h"
#include <QCoreApplication>

using namespace std;

Headset::Headset()
{
    if (EE_EngineConnect() != EDK_OK)
        cerr << "Emotiv Engine start up failed";
    trackPlaying = false;
}

Headset::~Headset()
{
    EE_EngineDisconnect();
}

void Headset::initialise(QString u, QString a, QString t)
{
    user = u;
    artist = a;
    track = t;
    event = EE_EmoEngineEventCreate();
    emoState	= EE_EmoStateCreate();
    userID = 0;
    engagement.clear();
    excitementST.clear();
    excitementLT.clear();
    frustration.clear();
    meditation.clear();
}

void Headset::logEmoState()
{
    int state = EE_EngineGetNextEvent(event);

    // New event needs to be handled
    if (state == EDK_OK) {
        EE_Event_t eventType = EE_EmoEngineEventGetType(event);
        EE_EmoEngineEventGetUserId(event, &userID);

        // Log the EmoState if it has been updated
        if (eventType == EE_EmoStateUpdated) {
            EE_EmoEngineEventGetEmoState(event, emoState);
            const float timestamp = ES_GetTimeFromStart(emoState);
            cout << "New EmoState from user" << timestamp << " " << userID << endl;
            engagement << ES_AffectivGetEngagementBoredomScore(emoState);
            excitementST << ES_AffectivGetExcitementShortTermScore(emoState);
            excitementLT << ES_AffectivGetExcitementLongTermScore(emoState);
            frustration << ES_AffectivGetFrustrationScore(emoState);
            meditation << ES_AffectivGetMeditationScore(emoState);
        }
    }
    else if (state != EDK_NO_EVENT) {
        cerr << "Internal error in Emotiv Engine!";
    }
    return;
}

void Headset::writeData()
{
    emit newUserTrack(user, artist, track, engagement, excitementST, excitementLT, frustration, meditation);
    discardData();
    return;
}

void Headset::discardData()
{
    user = "NOT SET";
    artist = "NOT SET";
    track = "NOT SET";
    userID = 0;
    engagement.clear();
    excitementST.clear();
    excitementLT.clear();
    frustration.clear();
    meditation.clear();
    EE_EmoStateFree(emoState);
    EE_EmoEngineEventFree(event);
    return;
}



