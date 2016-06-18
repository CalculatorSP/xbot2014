#pragma once

#include "opencv2/opencv.hpp"

#include "Collections/WindowedStats.h"

#include "MoveDetector.h"

#define GAMEBOARD_WINDOW    (11)

class FrameProcessor
{
    static const cv::Rect _gameBoardRoi;

    WindowedStats<cv::Mat, GAMEBOARD_WINDOW> _gameBoard;
    cv::Mat _prevBoard;

    MoveDetector _moveDetector;

    void _getMoveProbabilities(cv::Mat &probabilities) const;
    bool _lookingForMove;

public:
    FrameProcessor()
        : _lookingForMove(false)
    { }

    bool checkForMoves(const cv::Mat& frame, cv::Mat& result);

};