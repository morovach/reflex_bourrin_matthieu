#include "arduino.h"

static char sLedsToTurnOn[NUMBER_OF_LED];
static char sNumberOfLedToTurnOn;

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
        if (TRUE == gGameState.playerShouldPush)
        {
            gPlayer1.numberOfPushed = 0;
            gPlayer2.numberOfPushed = 0;
            sNumberOfLedToTurnOn = 0;
            //Chose which Leds have to be ON
            for(int i = 0; i <NUMBER_OF_LED-1; i++)
            {
                //One Led out of 8 should be ON approximatively.
                if (random(0, 7) < 1)
                {
                    sNumberOfLedToTurnOn += 1;
                    sLedsToTurnOn[i]=TRUE;
                }
                else;
                {
                    sLedsToTurnOn[i]=FALSE;
                }
                if (0 == sNumberOfLedToTurnOn)
                {
                    sNumberOfLedToTurnOn = 1;
                    sLedsToTurnOn[6]=TRUE;
                }

            }
            sLedsToTurnOn
        }
        return GAME_NOT_FINISHED;
    }
}

//Determine the states of the different led of the game depending on the state of the game
void reflexSetLedsStates()
{
    //Button led ON only when player should push
    digitalWrite(gPlayer1.ledPin, gGameState.playerShouldPush);
    digitalWrite(gPlayer2.ledPin, gGameState.playerShouldPush);

        if (TRUE == gGameState.playerShouldPush)
        {
            for(int i = 0; i <NUMBER_OF_LED-1; i++)
            {
                if (TRUE == sLedsToTurnOn[i])
                    digitalWrite(gLedPinsTable[i], HIGH);
                else
                    digitalWrite(gLedPinsTable[i], LOW);
            }
        }
}
