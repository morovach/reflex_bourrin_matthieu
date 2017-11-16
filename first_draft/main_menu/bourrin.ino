#include "arduino.h"
static char sBourrinGamePointRatio = 2;

void bourrinLoop()
{
    while(1)
    {
    bourrinReadDeboncedPin(&gPlayer1);
    bourrinReadDeboncedPin(&gPlayer2);
    if (GAME_FINISHED == bourrinSetGameState())
        break;
    bourrinSetLedsStates();
    }
}

//Check if the button has been really pressed and update the score of the player if so
void bourrinReadDeboncedPin(Player *argPlayer)
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
                bourrinReactToButtonPressure(argPlayer);
            }
        }
    }
    argPlayer->lastButtonState = reading;
}

//Determine the changing of the score when the player pushed the button
void bourrinReactToButtonPressure(Player *argPlayer)
{
    //Players should press a lot, as fast as possible
    argPlayer->score +=1;
}

char bourrinSetGameState()
{

    //Victory conditions:
    //When FAST_GAME mode: one player should have a lot of point more than the other

    //Maximum score optained player 2 won
    if (SCORE_MAXIMUM+1 <= ((gPlayer2.score - gPlayer1.score)/sBourrinGamePointRatio))
    {
        gGameState.currentState = SOMEONE_WON;
        gGameState.playerWinner = PLAYER_TWO;
        gGameState.gameFinalAnnimationTimmer = millis();
        //return to the menu loop
        return GAME_FINISHED;
    }
    //Maximum score optained player 1 won
    else if (SCORE_MAXIMUM+1 <= ((gPlayer1.score - gPlayer2.score)/sBourrinGamePointRatio))
    {
        gGameState.currentState = SOMEONE_WON;
        gGameState.playerWinner = PLAYER_ONE;
        gGameState.gameFinalAnnimationTimmer = millis();
        //return to the menu loop
        return GAME_FINISHED;
    }
    else
    {
        return GAME_NOT_FINISHED;
    }
}

//Determine the states of the different led of the game depending on the state of the game
void bourrinSetLedsStates()
{
    //Button led ON only when player should push
    blinkLed100ms(gPlayer1.ledPin);
    blinkLed100ms(gPlayer2.ledPin);

    //Only one LED moving according to the score
    for(int i =0; i<NUMBER_OF_LED ; i++)
    {
        if ((gPlayer2.score - gPlayer1.score)/sBourrinGamePointRatio+6 == i)
            digitalWrite(gLedPinsTable[i], HIGH);
        else
            digitalWrite(gLedPinsTable[i], LOW);
    }
}
