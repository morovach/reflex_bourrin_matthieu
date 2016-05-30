#define TRUE 1
#define FALSE 0

#define REFLEX_GAME 0
#define FAST_GAME 1
#define GAME_RATIO 3

#define PLAYER_ONE 1
#define PLAYER_TWO 2

#define NUMBER_OF_LED 13
#define SCORE_MAXIMUM (NUMBER_OF_LED/2)

#define DEBOUNCE_DELAY 10

enum State {GAME_RUNNING, GAME_STARTING, SOMEONE_WON};

typedef struct GameState_t GameState_t;
struct GameState_t
{
    State currentState = GAME_STARTING;
    char playerShouldPush = TRUE;   //The score increments when the player should push
    char typeOfGame = REFLEX_GAME;  //Could be REFLEX_GAME of FAST_GAME
    char playerWinner = 0;          //Number of the player that won    
    char gameStartingCounter = 0;
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
};

GameState_t gGameState;
Player gPlayer1 = {0,0,0,46,30,0};
Player gPlayer2 = {0,0,0,47,34,0};
int gLedPinsTable[NUMBER_OF_LED] = {44,45,2,3,4,5,6,7,8,9,10,11,12,};

void setup() 
{
    pinMode(gPlayer1.buttonPin, INPUT);
    pinMode(gPlayer1.ledPin, OUTPUT);    
    pinMode(gPlayer2.ledPin, OUTPUT); 
    pinMode(gPlayer2.buttonPin, INPUT);     
    
    for (int i = 0; i<  NUMBER_OF_LED; i++)
    {
        pinMode(gLedPinsTable[i], OUTPUT);  
        digitalWrite(gLedPinsTable[i],LOW);
    }
}

void loop() 
{
    readDeboncedPin(&gPlayer1);
    readDeboncedPin(&gPlayer2);
    setGameState();
    setLedsStates();
}

//Check if the button has been really pressed and update the score of the player if so
void readDeboncedPin(Player *argPlayer)
{
    int reading = digitalRead(argPlayer->buttonPin);

    // If the switch changed, due to noise or pressing:
    if (reading != argPlayer->lastButtonState) 
    {
        // reset the debouncing timer
        argPlayer->lastDebounceTime = millis();
    }
    
    if ((millis() - argPlayer->lastDebounceTime) > DEBOUNCE_DELAY) 
    {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
    
        // if the button state has changed:
        if (reading != argPlayer->buttonState) 
        {
            argPlayer->buttonState = reading;

            if (argPlayer->buttonState == HIGH) 
            {
                //The player has pushed the button and the score should be updated
                setPlayerScore(argPlayer);
            }
        }
    }
    argPlayer->lastButtonState = reading;
}

//Determine the changing of the score when the player pushed the button
void setPlayerScore(Player *argPlayer)
{
    if (GAME_RUNNING == gGameState.currentState)
    {
        if (REFLEX_GAME == gGameState.typeOfGame)
        {
            if (TRUE == gGameState.playerShouldPush)
            {
                argPlayer->score +=1;
            }
            else
            {
                argPlayer->score -=1;
            }
        }
        else if (FAST_GAME == gGameState.typeOfGame)
        {
            if (TRUE == gGameState.playerShouldPush)
            {
                argPlayer->score +=1;
            }
        }
    }
}

void setGameState()
{
    static long sReflexGameTimer = 0;
    static long sRandomTime = 0;
    static long sGameStartingTimer = 0;
    if (GAME_RUNNING == gGameState.currentState)
    {
        //Maximum score optained player 2 won
        if      (SCORE_MAXIMUM <= ((gPlayer2.score - gPlayer1.score)/GAME_RATIO))
        {
            gGameState.currentState = SOMEONE_WON;
            gGameState.playerWinner = PLAYER_TWO;
        }
        //Maximum score optained player 1 won
        else if (SCORE_MAXIMUM <= ((gPlayer1.score - gPlayer2.score)/GAME_RATIO))
        {
            gGameState.currentState = SOMEONE_WON;
            gGameState.playerWinner = PLAYER_ONE;
        }
        else
        {
            if (REFLEX_GAME == gGameState.typeOfGame)
            {
                if ((millis() - sReflexGameTimer) > sRandomTime)
                {
                    sReflexGameTimer = millis();
                    sRandomTime = random(1500, 4000);
                    gGameState.playerShouldPush =! gGameState.playerShouldPush;
                }
            }
            else if (FAST_GAME == gGameState.typeOfGame) 
            {
                gGameState.playerShouldPush =TRUE;
            }
        }
    }
    else if(GAME_STARTING == gGameState.currentState)
    {
        if ((millis() - sGameStartingTimer) > 500)
        {
            sGameStartingTimer = millis();
            gGameState.gameStartingCounter++;
            if (3==gGameState.gameStartingCounter)
            {
                gGameState.currentState = GAME_RUNNING;
                gGameState.gameStartingCounter = 0;
            }
        }
    }
        
    //TODO: determiner ici comment relancer une partie avec les differents modes possibles
}

//Determine the states of the different led of the game depending on the state of the game
void setLedsStates()
{
    static long sPwmTimer = 0;
    static char sPwmState = 1;
    static char sPwmPower = 0;
    if (GAME_RUNNING == gGameState.currentState)
    {
        if (REFLEX_GAME == gGameState.typeOfGame)
        {        
            digitalWrite(gPlayer1.ledPin, gGameState.playerShouldPush);
            digitalWrite(gPlayer2.ledPin, gGameState.playerShouldPush);
        }
        else if (FAST_GAME == gGameState.typeOfGame)
        {
            blinkLed100ms(gPlayer1.ledPin);
            blinkLed100ms(gPlayer2.ledPin);
        }
        
        for(int i =0; i<NUMBER_OF_LED ; i++)
        {
            if ((gPlayer1.score - gPlayer2.score)/GAME_RATIO+6 == i)
                digitalWrite(gLedPinsTable[i], HIGH);
            else
                digitalWrite(gLedPinsTable[i], LOW);
        }

    }
    else if (SOMEONE_WON == gGameState.currentState)
    {
        if (PLAYER_ONE == gGameState.playerWinner)
        {
            blinkLed300ms(gPlayer1.ledPin);
            digitalWrite(gPlayer2.ledPin, LOW);
        }
        else
        {
            blinkLed300ms(gPlayer2.ledPin);
            digitalWrite(gPlayer1.ledPin, LOW);
        }
        
        //All the LEDS are going to "shine"
        if ((millis()-sPwmTimer)>2)
        {
            sPwmTimer = millis();
            for (int i=0; i<NUMBER_OF_LED; i++)
                analogWrite(gLedPinsTable[i],sPwmPower);
            if ((255 == sPwmPower)|| (0 == sPwmPower))
                sPwmState = (-sPwmState);
            sPwmPower += sPwmState;
        }
    }
    //TODO: turn on the LED like a counter to indicate when the game STARTS
    else if (GAME_STARTING == gGameState.currentState)
    {
        for(int i =0; i<NUMBER_OF_LED ; i++)
        {
            digitalWrite(gLedPinsTable[i], LOW);
        }        
    }
}

//Blink a led every 300 ms
void blinkLed300ms(int argLedPinNumber)
{   
    static long sBlinkCounter300ms = 0;
    static char sBlinkState = LOW;
    digitalWrite(argLedPinNumber, sBlinkState);
    if ((millis() - sBlinkCounter300ms) > 300)
    {
        sBlinkCounter300ms = millis();
        sBlinkState =! sBlinkState;
    }
}

//Blink a led every 100 ms
void blinkLed100ms(int argLedPinNumber)
{
    static long sBlinkCounter100ms = 0;
    static char sBlinkState = LOW;
    digitalWrite(argLedPinNumber, sBlinkState);
    if ((millis() - sBlinkCounter100ms) > 100)
    {
        sBlinkCounter100ms = millis();
        sBlinkState =! sBlinkState;
    }
}
