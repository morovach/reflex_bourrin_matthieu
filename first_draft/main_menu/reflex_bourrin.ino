#include "arduino.h"
static char sGamePointRatio = 2;

void reflexBourrinLoop()
{
    while(1)
    {
    reflexBourrinReadDeboncedPin(&gPlayer1);
    reflexBourrinReadDeboncedPin(&gPlayer2);
    if (GAME_FINISHED == reflexBourrinSetGameState())
        break;
    reflexBourrinSetLedsStates();
    }
}

//Check if the button has been really pressed and update the score of the player if so
void reflexBourrinReadDeboncedPin(Player *argPlayer)
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
                reflexBourrinReactToButtonPressure(argPlayer);
            }
        }
    }
    argPlayer->lastButtonState = reading;
}

//Determine the changing of the score when the player pushed the button
void reflexBourrinReactToButtonPressure(Player *argPlayer)
{
    //Should press a lot when the LIGHT is ON
    //Should not press when LIGHT is OFF
    if (TRUE == gGameState.playerShouldPush)
    {
        argPlayer->score +=1;
    }
    else
    {
        argPlayer->score -=1;
    }
}

char reflexBourrinSetGameState()
{
    static long sReflexGameTimer = 0;
    static long sRandomTime = 0;

    //Victory conditions:
    //When REFLEX_FAST_GAME mode: one player should have a lot of point more than the other

    //Maximum score optained player 2 won
    if (SCORE_MAXIMUM+1 <= ((gPlayer2.score - gPlayer1.score)/sGamePointRatio))
    {
        gGameState.currentState = SOMEONE_WON;
        gGameState.playerWinner = PLAYER_TWO;
        gGameState.gameFinalAnnimationTimmer = millis();
        //return to the menu loop
        return GAME_FINISHED;
    }
    //Maximum score optained player 1 won
    else if (SCORE_MAXIMUM+1 <= ((gPlayer1.score - gPlayer2.score)/sGamePointRatio))
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
void reflexBourrinSetLedsStates()
{
    //Button led ON only when player should push
    digitalWrite(gPlayer1.ledPin, gGameState.playerShouldPush);
    digitalWrite(gPlayer2.ledPin, gGameState.playerShouldPush);

    //Only one LED moving according to the score
    for(int i =0; i<NUMBER_OF_LED ; i++)
    {
        if ((gPlayer1.score - gPlayer2.score)/sGamePointRatio+6 == i)
            digitalWrite(gLedPinsTable[i], HIGH);
        else
            digitalWrite(gLedPinsTable[i], LOW);
    }
}
