#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

//std-c++ includes
#include <string>

class QPixmap;

#if defined(Q_OS_WINDOWS)
struct tagRECT;
#endif

#if defined(Q_OS_LINUX)
struct tagRECT{
    long top;
    long bottom;
    long left;
    long right;
};
#endif

#pragma once
#include <opencv2/opencv.hpp>
class Screencapture
{
public:
    Screencapture();
    void saveScreenshotToFile(std::string iFilename);
#if defined(Q_OS_LINUX) || defined(Q_OS_WINDOWS)
    void screenshotOverlapToFile(tagRECT* tr, std::string iFilename);
    void screenshotOverlappingBlurred(std::vector<tagRECT*> liTr);
#endif
private:
    QPixmap grabScreens();

};

#endif // SCREENCAPTURE_H
