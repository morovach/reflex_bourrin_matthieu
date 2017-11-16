#include "arduino.h"

void reflexLoop()
{
    while(1)
    {
    reflexReadDeboncedPin(&gPlayer1);
    reflexReadDeboncedPin(&gPlayer2);
    if (GAME_FINISHED == reflexSetGameState())
        break;
    reflexSetLedsStates();
    }
}

//Check if the button has been really pressed and update the score of the player if so
void reflexReadDeboncedPin(Player *argPlayer)
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
                reflexReactToButtonPressure(argPlayer);
            }
        }
    }
    argPlayer->lastButtonState = reading;
}

//Determine the changing of the score when the player pushed the button
void reflexReactToButtonPressure(Player *argPlayer)
{
    static char sLastPlayerThatPressed = -1;
    //Should press only once when light is ON
    //The faster wins
    if (TRUE == gGameState.playerShouldPush)
    {
        argPlayer->score +=1;
        gGameState.playerShouldPush =! gGameState.playerShouldPush;
    }
    //Little trick to avoid someone pushing always
    else if (argPlayer->playerNumber == sLastPlayerThatPressed)
    {
        argPlayer->score -=1;
    }
    sLastPlayerThatPressed = argPlayer->playerNumber;
}

char reflexSetGameState()
{
    static long sReflexGameTimer = 0;
    static long sRandomTime = 0;

    //Victory conditions:
    //When in REFLEX_GAME: The first to obtain SCORE_MAXIMUM+1 wins
    if (SCORE_MAXIMUM+1 <= (gPlayer2.score))
    {
        gGameState.currentState = SOMEONE_WON;
        gGameState.playerWinner = PLAYER_TWO;
        gGameState.gameFinalAnnimationTimmer = millis();
        //return to the menu loop
        return GAME_FINISHED;
    }
    //Maximum score optained player 1 won
    else if (SCORE_MAXIMUM+1 <= (gPlayer1.score))
    {
        gGameState.currentState = SOMEONE_WON;
        gGameState.playerWinner = PLAYER_ONE;
        gGameState.gameFinalAnnimationTimmer = millis();
        //return to the menu loop
        return GAME_FINISHED;
    }
    //Determine when the player Should press
    else if ((millis() - sReflexGameTimer) > sRandomTime)
    {
        sReflexGameTimer = millis();
        sRandomTime = random(500, 4000);
        gGameState.playerShouldPush =! gGameState.playerShouldPush;
        return GAME_NOT_FINISHED;
    }    
}

//Determine the states of the different led of the game depending on the state of the game
void reflexSetLedsStates()
{
    //Button led ON only when player should push
    digitalWrite(gPlayer1.ledPin, gGameState.playerShouldPush);
    digitalWrite(gPlayer2.ledPin, gGameState.playerShouldPush);

    //The nu,ber of LEDs ON reflect the score of the player
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
