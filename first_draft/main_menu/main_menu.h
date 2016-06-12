#define TRUE 1
#define FALSE 0

#define REFLEX_FAST_GAME 0
#define FAST_GAME 1
#define REFLEX_GAME 2

#define PLAYER_ONE 1
#define PLAYER_TWO 2

#define NUMBER_OF_LED 13
#define SCORE_MAXIMUM (NUMBER_OF_LED/2)

#define DEBOUNCE_DELAY 10
#define NUMBER_OF_GAME_POSSIBLE 3

#define GAME_NOT_FINISHED 0
#define GAME_FINISHED 1

enum State {MENU, GAME_RUNNING, GAME_STARTING, SOMEONE_WON};

typedef struct GameState_t GameState_t;
struct GameState_t
{
    State currentState = MENU;
    char playerShouldPush = TRUE;   //The score increments when the player should push
    char typeOfGame = FAST_GAME;  //Could be REFLEX_FAST_GAME of FAST_GAME
    char playerWinner = 0;          //Number of the player that won    
    char gameStartingCounter = 0;
    long gameFinalAnnimationTimmer = 0;
};

typedef struct Player_t Player;
struct Player_t
{
    long lastDebounceTime;
    int lastButtonState;
    int buttonState;  
    int buttonPin;          // the number of the pushbutton pin	
    int ledPin;             // the number of the LED pin
    signed int score;
    char playerNumber;
};

/*Global variables */
GameState_t gGameState;
Player gPlayer1 = {0,0,0,50,30,0,1};
Player gPlayer2 = {0,0,0,52,34,0,2};
int gLedPinsTable[NUMBER_OF_LED] = {44,45,2,3,4,5,6,7,8,9,10,11,12};
char gMenuIndexSelect =0;
char gGamePointRatio = 3;

