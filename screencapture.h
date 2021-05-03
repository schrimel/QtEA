#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

//std-c++ includes
#include <string>

class QPixmap;

#if defined(Q_OS_WINDOWS)
struct tagRECT;
#endif

#pragma once
#include <opencv2/opencv.hpp>
class Screencapture
{
public:
    Screencapture();
    void saveScreenshotToFile(std::string iFilename);
#if defined(Q_OS_WINDOWS)
    void screenshotOverlapToFile(tagRECT* tr, std::string iFilename);
#endif
private:
    QPixmap grabScreens();

};

#endif // SCREENCAPTURE_H
