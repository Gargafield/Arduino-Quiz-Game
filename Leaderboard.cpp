#include "Leaderboard.h"

#include <Arduino.h>
#include <stdlib.h>

int LEADERBOARD_TABLE[100];

int calculateScore(BoardPoint &point)
{
    return max((point.score * 10) - (point.time / 1000 * 2), 0);
};

Leaderboard::Leaderboard() : maxSize(100), allocated(LEADERBOARD_TABLE)
{
    memset(LEADERBOARD_TABLE, -1, maxSize);
};

bool Leaderboard::addPlayer(BoardPoint point)
{
    if (currentIndex >= maxSize)
        currentIndex = 0;
    allocated[currentIndex] = calculateScore(point);
    currentIndex += 1;
}

int Leaderboard::getPlace(BoardPoint point)
{
    auto score = calculateScore(point);
    int place = 1;
    // Lovely Omega(n) because who likes sorting?
    for (int i = 0; i < maxSize; i++)
    {
        Serial.print("Allocated was: ");
        Serial.print(allocated[i], 10);
        Serial.println();
        if (allocated[i] != -1 && allocated[i] > score)
        {
            place += 1;
        }
    }
    return place;
}
