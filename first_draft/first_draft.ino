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

enum State {MENU, GAME_RUNNING, GAME_STARTING, SOMEONE_WON};

typedef struct GameState_t GameState_t;
struct GameState_t
{
    State currentState = MENU;
    char playerShouldPush = TRUE;   //The score increments when the player should push
    char typeOfGame = FAST_GAME;  //Could be REFLEX_FAST_GAME of FAST_GAME
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
    char playerNumber;
};

GameState_t gGameState;
Player gPlayer1 = {0,0,0,50,30,0,1};
Player gPlayer2 = {0,0,0,52,34,0,2};
int gLedPinsTable[NUMBER_OF_LED] = {44,45,2,3,4,5,6,7,8,9,10,11,12};
char gMenuIndexSelect =0;
char gGamePointRatio = 3;
    
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

            if (argPlayer->buttonState == LOW) 
            {
                //The player has pushed the button and the score should be updated
                reactToButtonPressure(argPlayer);
            }
        }
    }
    argPlayer->lastButtonState = reading;
}

//Determine the changing of the score when the player pushed the button
void reactToButtonPressure(Player *argPlayer)
{
    static char lastPlayerThatPressed = 200;

    if (GAME_RUNNING == gGameState.currentState)
    {
        //Should press a lot when the LIGHT is ON
        //Should not press when LIGHT is OFF
        if (REFLEX_FAST_GAME == gGameState.typeOfGame)
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
        
        //Should press a lot, as fast as possible
        else if (FAST_GAME == gGameState.typeOfGame)
        {
            if (TRUE == gGameState.playerShouldPush)
            {
                argPlayer->score +=1;
            }
        }
        
        //Should press only once when light is ON
        //The faster wins
        if (REFLEX_GAME == gGameState.typeOfGame)
        {
            if (TRUE == gGameState.playerShouldPush)
            {
                argPlayer->score +=1;
                gGameState.playerShouldPush =! gGameState.playerShouldPush;
            }
            //Little trick to avoid someone pushing always
            else if (argPlayer->playerNumber == lastPlayerThatPressed)
            {
                argPlayer->score -=1;
            }
        }        
        
        lastPlayerThatPressed = argPlayer->playerNumber;
    }
    else if (MENU == gGameState.currentState)
    {
        if (1 == argPlayer->playerNumber)
        {
            gMenuIndexSelect = (gMenuIndexSelect+1)%(NUMBER_OF_LED);
        }
        else
        {
            if (gMenuIndexSelect < NUMBER_OF_GAME_POSSIBLE)
            {
                gGameState.currentState = GAME_STARTING;
                switch (gMenuIndexSelect)
                {
                    case 0  : gGameState.typeOfGame = REFLEX_FAST_GAME; gGamePointRatio=2;  break;
                    case 1  : gGameState.typeOfGame = FAST_GAME;        gGamePointRatio=2;  break;
                    case 2  : gGameState.typeOfGame = REFLEX_GAME;      gGamePointRatio=1;  break;
                    default : gGameState.typeOfGame = REFLEX_FAST_GAME; gGamePointRatio=3;  break;
                }
                gGameState.gameStartingCounter = 0;
            }
        }
    }
}

void setGameState()
{
    static long sReflexGameTimer = 0;
    static long sRandomTime = 0;
    static long sGameStartingTimer = 0;
    static long sGameSmoneWonTimer = 0;
    if (GAME_RUNNING == gGameState.currentState)
    {
        //Victory conditions:
        
        //When REFLEX_FAST_GAME or FAST_GAME one player should have a lot of point more than the other
        if ((REFLEX_FAST_GAME == gGameState.typeOfGame) ||  (FAST_GAME == gGameState.typeOfGame))
        {
            //Maximum score optained player 2 won
            if      (SCORE_MAXIMUM+1 <= ((gPlayer2.score - gPlayer1.score)/gGamePointRatio))
            {
                gGameState.currentState = SOMEONE_WON;
                gGameState.playerWinner = PLAYER_TWO;
                sGameSmoneWonTimer = millis();
            }
            //Maximum score optained player 1 won
            else if (SCORE_MAXIMUM+1 <= ((gPlayer1.score - gPlayer2.score)/gGamePointRatio))
            {
                gGameState.currentState = SOMEONE_WON;
                gGameState.playerWinner = PLAYER_ONE;
                sGameSmoneWonTimer = millis();
            }
        }
        //When in REFLEX_GAME: The first to obtain SCORE_MAXIMUM+1 wins
        else if (REFLEX_GAME == gGameState.typeOfGame)
        {
            if      (SCORE_MAXIMUM+1 <= ((gPlayer2.score)/gGamePointRatio))
            {
                gGameState.currentState = SOMEONE_WON;
                gGameState.playerWinner = PLAYER_TWO;
                sGameSmoneWonTimer = millis();
            }
            //Maximum score optained player 1 won
            else if (SCORE_MAXIMUM+1 <= ((gPlayer1.score)/gGamePointRatio))
            {
                gGameState.currentState = SOMEONE_WON;
                gGameState.playerWinner = PLAYER_ONE;
                sGameSmoneWonTimer = millis();
            }            
        }

        //Determine when the player Should press
        if ((REFLEX_FAST_GAME == gGameState.typeOfGame) ||  (REFLEX_GAME == gGameState.typeOfGame))
        {
            if ((millis() - sReflexGameTimer) > sRandomTime)
            {
                sReflexGameTimer = millis();
                sRandomTime = random(500, 4000);
                gGameState.playerShouldPush =! gGameState.playerShouldPush;
            }
        }
        else if (FAST_GAME == gGameState.typeOfGame) 
        {
            gGameState.playerShouldPush =TRUE;
        }
    }

    //Launches an "animation" with the LED that lasts 4 seconds where LEDS are turned ON one after the Other
    //To indicated when the game starts
    else if(GAME_STARTING == gGameState.currentState)
    {
        if ((millis() - sGameStartingTimer) > 1000)
        {
            sGameStartingTimer = millis();
            gGameState.gameStartingCounter++;
            if (4==gGameState.gameStartingCounter)
            {
                gPlayer1.score = 0;
                gPlayer2.score = 0;
                gGameState.currentState = GAME_RUNNING;
                gGameState.gameStartingCounter = 0;
            }
        }
    }
    else if (MENU == gGameState.currentState)
    {
        sGameStartingTimer = millis();
    }
    else if (SOMEONE_WON == gGameState.currentState)
    {
        if(millis() - sGameSmoneWonTimer >10000)
            gGameState.currentState = MENU;
    }
}

//Determine the states of the different led of the game depending on the state of the game
void setLedsStates()
{
    static long sPwmTimer = 0;
    static char sPwmState = 1;
    static char sPwmPower = 0;
    
    if (GAME_RUNNING == gGameState.currentState)
    {
        //Button led ON only when player should push
        if ((REFLEX_FAST_GAME == gGameState.typeOfGame) ||  (REFLEX_GAME == gGameState.typeOfGame))
        {        
            digitalWrite(gPlayer1.ledPin, gGameState.playerShouldPush);
            digitalWrite(gPlayer2.ledPin, gGameState.playerShouldPush);
        }
        //Button LED blinking fast
        else if (FAST_GAME == gGameState.typeOfGame)
        {
            blinkLed100ms(gPlayer1.ledPin);
            blinkLed100ms(gPlayer2.ledPin);
        }
        
        //Only one LED moving according to the score
        if ((REFLEX_FAST_GAME == gGameState.typeOfGame) ||  (FAST_GAME == gGameState.typeOfGame))
        {
            for(int i =0; i<NUMBER_OF_LED ; i++)
            {
                if ((gPlayer1.score - gPlayer2.score)/gGamePointRatio+6 == i)
                    digitalWrite(gLedPinsTable[i], HIGH);
                else
                    digitalWrite(gLedPinsTable[i], LOW);
            }
        }
        
        //The goal is to activate all the LEDS
        else if (REFLEX_GAME == gGameState.typeOfGame)
        {
            digitalWrite(gLedPinsTable[NUMBER_OF_LED/2], HIGH);
            for(int i =0; i<NUMBER_OF_LED ; i++)
            {
            if ((i >= NUMBER_OF_LED/2-gPlayer1.score) && (i <NUMBER_OF_LED/2))
                digitalWrite(gLedPinsTable[i], HIGH);
            else if ((i <= NUMBER_OF_LED/2+gPlayer2.score) && (i >NUMBER_OF_LED/2))
                digitalWrite(gLedPinsTable[i], HIGH);
            else
                digitalWrite(gLedPinsTable[i], LOW);
            }            
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
    else if (GAME_STARTING == gGameState.currentState)
    {
        for(int i =0; i<NUMBER_OF_LED ; i++)
        {
            //Turn ON LED 6, then 4,5,6,7,8 then 2,3,4,5,6,7,8,9,10 finally 0,1,2,3,4,5,6,7,8,9,10,11,12
            if ((i >= NUMBER_OF_LED/2-gGameState.gameStartingCounter*2) && (i <= NUMBER_OF_LED/2+gGameState.gameStartingCounter*2))
                digitalWrite(gLedPinsTable[i], HIGH);
            else
                digitalWrite(gLedPinsTable[i], LOW);
        }        
    }
    
    //Turn ON the LED that indicates the GAME that could be launched
    else if (MENU == gGameState.currentState)
    {
        for(int i =0; i<NUMBER_OF_LED ; i++)
        {
            digitalWrite(gLedPinsTable[i], LOW);
        }
        digitalWrite(gLedPinsTable[gMenuIndexSelect], HIGH);
        
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
