typedef struct
{
    char *question;
    char *answers[4];
    int rightAnswer;
} Question;

#define NUM_QUESTIONS 10

extern const Question Questions[NUM_QUESTIONS];
