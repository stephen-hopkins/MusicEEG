#include "headset.h"
#include <iostream>
#include "windows.h"
#include <QCoreApplication>
#include <cmath>

using namespace std;

Headset::Headset()
{
    if (EE_EngineRemoteConnect("127.0.0.1", 3008) != EDK_OK) {
        cerr << "Emotiv Engine start up failed";
        cerr << "Error code: " << EE_EngineRemoteConnect("127.0.0.1", 3008) << endl;
    }

    trackPlaying = false;
    event = EE_EmoEngineEventCreate();
    emoState	= EE_EmoStateCreate();

}

Headset::~Headset()
{
    EE_EmoStateFree(emoState);
    EE_EmoEngineEventFree(event);
    EE_EngineDisconnect();
}

void Headset::initialise(QString u, QString a, QString t)
{
    user = u;
    artist = a;
    track = t;
    userID = 0;
    engagement.clear();
    excitement.clear();
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
            excitement << ES_AffectivGetExcitementShortTermScore(emoState);
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
    QList< QList<float> > rawEmoData;
    rawEmoData.append(engagement);
    rawEmoData.append(excitement);
    rawEmoData.append(frustration);
    rawEmoData.append(meditation);

    // calculate averages, change in averages, and std devs
    QList<float> averages;
    QList<float> changes;
    QList<float> stddevs;

    for (int n = 0 ; n < 4 ; n++) {
        averages.append(calcAverage(rawEmoData[n]));
    }

    for (int n = 0 ; n < 4 ; n++) {
        float avebeginning = calcAverage(rawEmoData[n].mid(0, 30));
        float aveend = calcAverage(rawEmoData[n].mid((rawEmoData[n].length())-30, 30 ));
        changes.append(aveend - avebeginning);
    }

    for (int e = 0 ; e < 4 ; e++) {
        float totalsqdiffs = 0;
        for (int n = 0 ; n < rawEmoData[e].size() ; n++) {
            float sqdiff = (rawEmoData[e][n] - averages[e]);
            sqdiff *= sqdiff;
            totalsqdiffs += sqdiff;
        }
        stddevs.append(sqrt(totalsqdiffs));
    }
    QList< QList<float> > stats;
    stats.append(averages);
    stats.append(changes);
    stats.append(stddevs);

    emit newUserTrack(user, artist, track, rawEmoData, stats);
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
    excitement.clear();
    frustration.clear();
    meditation.clear();
    return;
}

float Headset::calcAverage(QList<float> input)
{
   float total = 0;
   int items = input.size();
   while (!input.isEmpty()) {
       total += input.takeFirst();
   }
   return total / items;
}

