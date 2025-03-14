
struct BoardPoint
{
    int time;
    int score;
};

int calculateScore(BoardPoint &point);

class Leaderboard
{
public:
    Leaderboard();

    bool addPlayer(BoardPoint point);
    void resize(int newSize);
    int getPlace(BoardPoint point);

private:
    int maxSize = 20;
    int currentIndex = 0;
    int *allocated = nullptr;
};