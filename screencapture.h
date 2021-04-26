#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

//std-c++ includes
#include <string>

class QPixmap;

#pragma once
#include <opencv2/opencv.hpp>
class Screencapture
{
public:
    Screencapture();
    void saveScreenshotToFile(std::string iFilename);
private:
    QPixmap grabScreens();

};

#endif // SCREENCAPTURE_H
