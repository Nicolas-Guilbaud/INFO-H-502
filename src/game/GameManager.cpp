#pragma once
#include <algorithm> 
#include <iostream>
#include <vector>

#include "../objects/rigidObject.cpp"

class GameManager{

private:
    bool gameEnded = false;
    bool firstShot = true;
    int currentRound = 0;
    int score[10][2];

public:

    bool isBallMoving = false;

    GameManager(){
        restartGame();
    }

    bool hasGameEnded(){
        return gameEnded;
    }

    bool hasStartedNewRound(){
        return firstShot;
    }

    void nextRound(){
        firstShot = true;
        currentRound++;
    }

    void restartGame(){
        gameEnded = false;
        firstShot = true;
        currentRound = 0;
        //clear score
        for(int i = 0; i < 10; i++){
            score[i][0] = 0;
            score[i][1] = 0;
        }
        std::cout << "A new game has started !" << std::endl;
    }

    void updateGameState(std::vector<rigidObject>& pins){
        //Simple scoring (without strike and spares logic)

        int fallenPins = this->computeFallenPins(pins);
        printScore(fallenPins);

        if(firstShot){
            score[currentRound][0] = fallenPins;
            firstShot = false;
            if(fallenPins == 10)
                nextRound();
        }else{
            score[currentRound][1] = fallenPins - score[currentRound][0];
            nextRound();
        }

        gameEnded = currentRound == 10;

        if(gameEnded)
            dispScore();
    }

    void printScore(int fallenPins){
        //print whole table
        std::cout << "Round " << currentRound+1 << ": " << fallenPins << " pins fallen !" << std::endl;

    }

    void dispScore() {
        int totalScore = 0;
        std::cout << "\nGame over! Here are the results: " << std::endl;
        for (int i = 0; i < 10; i++) {
            totalScore += score[i][0] + score[i][1];
            std::cout << "Trial " << i + 1 << ": " << score[i][0] << " pins down, " << score[i][1] << " additional pins down" << std::endl;
        }
        std::cout << "Total score: " << totalScore << std::endl;
    }

    int computeFallenPins(std::vector<rigidObject>& PINS) {
        int res = 0;
        for (rigidObject& pin : PINS) {
            if (pin.isOnTheSide()) {
                res++;
            }
        }
        return res;
    }
    
};