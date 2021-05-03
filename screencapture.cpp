//std-c++ includes
#include <fstream>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <ctime>

//qt includes
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

#define NOMINMAX
#include <windows.h>


//user includes
#include "screencapture.h"

QPixmap Screencapture::grabScreens()
{
      QList<QScreen*> screens = QGuiApplication::screens();
      QList<QPixmap> scrs;
      int w = 0, h = 0, p = 0;

      foreach (auto scr, screens)
        {
          //QRect g = scr->geometry();
          QPixmap pix = scr->grabWindow(0);
          w += pix.width();
          h = std::max(h, pix.height());
          scrs.append(pix);
        }

      QPixmap final(w, h);
      QPainter painter(&final);
      final.fill(Qt::black);
      foreach (auto scr, scrs)
        {
          painter.drawPixmap(QPoint(p, 0), scr);
          p += scr.width();
        }

      return final;
}

inline cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true )
{
  switch ( inImage.format() )
  {
     // 8-bit, 4 channel
     case QImage::Format_ARGB32:
     case QImage::Format_ARGB32_Premultiplied:
     {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC4,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
     }

     // 8-bit, 3 channel
     case QImage::Format_RGB32:
     {
        if ( !inCloneImageData )
        {
           //qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so we don't modify the original QImage data";
        }

        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC4,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        cv::Mat  matNoAlpha;

        cv::cvtColor( mat, matNoAlpha, cv::COLOR_BGRA2BGR );   // drop the all-white alpha channel

        return matNoAlpha;
     }

     // 8-bit, 3 channel
     case QImage::Format_RGB888:
     {
        if ( !inCloneImageData )
        {
           //qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning so we don't modify the original QImage data";
        }

        QImage   swapped = inImage.rgbSwapped();

        return cv::Mat( swapped.height(), swapped.width(),
                        CV_8UC3,
                        const_cast<uchar*>(swapped.bits()),
                        static_cast<size_t>(swapped.bytesPerLine())
                        ).clone();
     }

     // 8-bit, 1 channel
     case QImage::Format_Indexed8:
     {
        cv::Mat  mat( inImage.height(), inImage.width(),
                      CV_8UC1,
                      const_cast<uchar*>(inImage.bits()),
                      static_cast<size_t>(inImage.bytesPerLine())
                      );

        return (inCloneImageData ? mat.clone() : mat);
     }

     default:
        //qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
        break;
  }

  return cv::Mat();
}

// If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
// with the cv::Mat directly
//    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
inline cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true )
{
  return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
}

Screencapture::Screencapture()
{
    //TODO fill constructor?
}


std::string gen_random(const int len) {

    std::string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    srand(time(nullptr));

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    return tmp_s;
}

void Screencapture::saveScreenshotToFile(std::string iFilename)
{
    QPixmap pm = grabScreens();
    QImage img = pm.toImage();
    cv::Mat wPrintimg = QImageToCvMat(img);
    cv::imwrite(iFilename, wPrintimg);
}

#if defined(Q_OS_WINDOWS)

void Screencapture::screenshotOverlapToFile(tagRECT* tr, std::string iFilename)
{
    QPixmap pm = grabScreens();
    QImage img = pm.toImage();

    const int bsize = 10;
    const int width = tr->right - tr->left;
    const int height = tr->bottom - tr->top;
    const int x = tr->left;
    const int y = tr->top;

    qDebug() << " i am here";

    cv::Mat wPrintimg = QImageToCvMat(img);
    cv::Rect region(x, y, width, height);

    cv::Rect wPaddedRegion(x-bsize, y-bsize, width+2*bsize, height + 2*bsize);
    cv::Mat wBlurredPaddedRegion;
    cv::GaussianBlur(wPrintimg(wPaddedRegion), wBlurredPaddedRegion, cv::Size(0,0), 4);
    cv::Mat wBlurredRegion = wBlurredPaddedRegion(cv::Rect(bsize, bsize, width, height));

    wBlurredRegion.copyTo(wPrintimg(region));
    cv::imwrite(iFilename, wPrintimg);
}

#endif
