
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Pitches.h"

#include "Questions.h"
#include "Utils.h"
#include "Leaderboard.h"

#define FRAME_DELAY 100
#define LCD_ADDRES 0x27
#define BUZZER_PIN 2
#define NUM_BUTTONS 4

#define WIDTH 16
#define HEIGHT 2

int buttons[NUM_BUTTONS] = {7, 8, 12, 13};
LiquidCrystal_I2C lcd(LCD_ADDRES, WIDTH, HEIGHT);

// We add one to account for null byte
char lines[HEIGHT][WIDTH + 1];

Leaderboard leaderBoard;
PlayerData data;

void setup()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.createChar(2, OWithCross);
    lcd.createChar(3, AWithRing);

    for (int i = 0; i < NUM_BUTTONS; i++)
    {
        pinMode(buttons[i], INPUT);
    }

    Serial.begin(9600);
    Serial.println("Made by Gargafield.\nLicensed under MIT. See LICENSE for more information.");
};

void step()
{
    switch (data.currentStage)
    {
    case (Stage::START):
        printFunc(drawStart);
        while (takeAnswers(1000 * 10) == -1)
        {
        };
        break;
    case (Stage::PICKING):
        data.currentQuestion = data.answers;
        printFunc(drawPicking);
        delay(1000);
        break;
    case (Stage::DISPLAYING):
        printFunc(drawQuestion);
        delay(3000);
        break;
    case (Stage::CAN_ANSWER):
    {
        printFunc(drawAnswer);

        int start = micros();
        data.currentAnswer = takeAnswers(10000);
        int end = micros();

        data.time += end - start;
        data.answers += 1;
        // I like this one liner
        data.correctAnswers += data.currentAnswer == Questions[data.currentQuestion].rightAnswer ? 1 : 0;
        break;
    }
    case (Stage::FINAL):
        printFunc(drawFinal);
        delay(3000);

        if (data.currentQuestion == (NUM_QUESTIONS - 1))
            data.currentStage = Stage::LEADERBOARD;
        else
            data.currentStage = Stage::PICKING;
        return;
    case (Stage::LEADERBOARD):
        printFunc(drawLeaderboard);
        delay(5000);
        leaderBoard.addPlayer({data.time, data.correctAnswers});

        // We dont have to set currentStage since its default in PlayerData
        data = PlayerData();
        return;
    default:
        data.currentStage = Stage::PICKING;
        return;
    }
    data.currentStage = (Stage)((int)data.currentStage + 1);
}

void playRightSound()
{
    tone(BUZZER_PIN, NOTE_C7, 125);
    delay(130);
    tone(BUZZER_PIN, NOTE_D7, 125);
    delay(130);
    tone(BUZZER_PIN, NOTE_G7, 125);
    delay(125);
    noTone(BUZZER_PIN);
}

void playWrongSound()
{
    tone(BUZZER_PIN, NOTE_A4, 400);
    delay(430);
    tone(BUZZER_PIN, NOTE_C4, 400);
    delay(450);
    noTone(BUZZER_PIN);
}

void playLeaderboardSound()
{
    tone(BUZZER_PIN, NOTE_E6, 125);
    delay(130);
    tone(BUZZER_PIN, NOTE_G6, 125);
    delay(130);
    tone(BUZZER_PIN, NOTE_E7, 125);
    delay(130);
    tone(BUZZER_PIN, NOTE_C7, 125);
    delay(130);
    tone(BUZZER_PIN, NOTE_D7, 125);
    delay(130);
    tone(BUZZER_PIN, NOTE_G7, 125);
    delay(125);
    noTone(BUZZER_PIN);
}

int takeAnswers(int time)
{
    auto start = millis();
    while ((millis() - start) < time)
    {
        delay(10);
        int buttonPressed = -1;
        for (int i = 0; i < NUM_BUTTONS; i++)
            if (digitalRead(buttons[i]) == HIGH)
                buttonPressed = i;

        if (buttonPressed == -1)
            continue;

        return buttonPressed;
    }
    return -1;
}

void drawWithPadding(int line, const char *str)
{
    int len = strlen(str);
    int padding = (WIDTH - len) / 2;
    memset(lines[line], ' ', padding);
    memcpy(lines[line] + padding, str, len);
    memset(lines[line] + padding + len, ' ', padding + (16 - padding - len - padding));
}

DefineDrawFunction(Start, START)
{
    memset(lines[0], ' ', WIDTH * HEIGHT);
    memcpy(lines[0], "Tryk p\3 en knap!", 16);
}

DefineDrawFunction(Picking, PICKING)
{
    memset(lines[0], ' ', WIDTH);
    drawWithPadding(1, "...");
}

DefineDrawFunction(Answer, ANSWER)
{
    auto &answers = Questions[data.currentQuestion].answers;
    sprintf(lines[0], "1: %s  2: %s", answers[0], answers[1]);
    sprintf(lines[1], "3: %s  4: %s", answers[2], answers[3]);
}

DefineDrawFunction(Question, QUESTION)
{
    memset(lines[0], ' ', WIDTH * HEIGHT);

    char *question = Questions[data.currentQuestion].question;
    int length = strlen(question);
    if (length > 16)
    {
        memcpy(lines[0], question, 16);
        memcpy(lines[1], question + 16, length - 16);
    }
    else
        memcpy(lines[0], question, strlen(question));
}

DefineDrawFunction(Final, FINAL)
{
    bool rightAnswer = data.currentAnswer == Questions[data.currentQuestion].rightAnswer;

    const char *text = rightAnswer ? "Rigtigt!" : "Forkert!";
    memset(lines[0], ' ', WIDTH * HEIGHT + 1);
    drawWithPadding(0, text);

    if (rightAnswer)
    {
        playRightSound();
    }
    else
    {
        char answer[16];
        sprintf(answer, "Svar: %s", Questions[data.currentQuestion].answers[Questions[data.currentQuestion].rightAnswer]);
        drawWithPadding(1, answer);
        playWrongSound();
    }
}

DefineDrawFunction(Leaderboard, LEADERBOARD)
{
    uint8_t place = leaderBoard.getPlace({data.time,
                                          data.correctAnswers});

    memset(lines[1], ' ', WIDTH);
    sprintf(lines[0], "Du kom top: %d", place);
    if (place <= 3)
        playLeaderboardSound();
}

void printFunc(void (*func)())
{
    func();
    printScreen();
}

void printScreen()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lines[0]);
    lcd.setCursor(0, 1);
    lcd.print(lines[1]);
}

void loop()
{
    step();
    delay(FRAME_DELAY);
};