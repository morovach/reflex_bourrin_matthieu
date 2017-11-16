#include "main_menu.h"
char sMenuIndexSelect =0;

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

//Determine what happens when button is pressed
void reactToButtonPressure(Player *argPlayer)
{
    static char lastPlayerThatPressed = 200;

    if (MENU == gGameState.currentState)
    {
        //select button has been pushed
        if (1 == argPlayer->playerNumber)
        {
            sMenuIndexSelect = (sMenuIndexSelect+1)%(NUMBER_OF_GAME_POSSIBLE);
        }
        //Start button has been pushed
        else
        {
            if (sMenuIndexSelect < NUMBER_OF_GAME_POSSIBLE)
            {
                gGameState.currentState = GAME_STARTING;
                switch (sMenuIndexSelect)
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

    //Launches an "animation" with the LED that lasts 4 seconds where LEDS are turned ON one after the Other
    //To indicated when the game starts
    if(GAME_STARTING == gGameState.currentState)
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

                //Animation finished: Start the game
                if (REFLEX_FAST_GAME == gGameState.typeOfGame)
                    reflexBourrinLoop();
                else if (FAST_GAME        == gGameState.typeOfGame)
                    bourrinLoop();
                else if (REFLEX_GAME      == gGameState.typeOfGame)
                    reflexLoop();
            }
        }
    }
    else if (MENU == gGameState.currentState)
    {
        sGameStartingTimer = millis();
    }
    else if (SOMEONE_WON == gGameState.currentState)
    {
        if(millis() - gGameState.gameFinalAnnimationTimmer >10000)
            gGameState.currentState = MENU;
    }
}

//Determine the states of the different led of the game depending on the state of the game
void setLedsStates()
{
    static long sPwmTimer = 0;
    static char sPwmState = 1;
    static char sPwmPower = 0;

    if (SOMEONE_WON == gGameState.currentState)
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
        digitalWrite(gLedPinsTable[sMenuIndexSelect], HIGH);

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
