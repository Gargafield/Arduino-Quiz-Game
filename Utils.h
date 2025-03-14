enum class Stage
{
    START,
    PICKING,
    DISPLAYING,
    CAN_ANSWER,
    FINAL,
    LEADERBOARD
};

enum class BufferState
{
    UNDEFINED,
    START,
    PICKING,
    QUESTION,
    ANSWER,
    FINAL,
    LEADERBOARD
};

struct PlayerData
{
    int16_t time = 0;
    byte correctAnswers = 0;
    byte answers = 0;
    byte currentAnswer = 0;
    byte currentQuestion = 0;
    Stage currentStage = Stage::START;
    BufferState currentState = BufferState::UNDEFINED;
};

byte OWithCross[8] = {
    0b00000,
    0b00000,
    0b01110,
    0b10011,
    0b10101,
    0b11001,
    0b01110,
    0b00000};

byte AWithRing[8] = {
    0b00110,
    0b00110,
    0b00000,
    0b01110,
    0b10001,
    0b10001,
    0b01111,
};

#define DefineDrawFunction(name, state)              \
    void draw##name()                                \
    {                                                \
        if (data.currentState == BufferState::state) \
            return;                                  \
        _draw##name();                               \
        data.currentState = BufferState::state;      \
    }                                                \
    void _draw##name()