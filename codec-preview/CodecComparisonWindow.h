#ifndef CODECCOMPARISONWINDOW_H
#define CODECCOMPARISONWINDOW_H

#include "constants.h"
#include <CodecParametersWidget.h>
#include <QDebug>
#include <QLabel>
#include <QProcess>
#include <QQueue>
#include <QString>
#include <QWidget>
#include <VLCQtCore/Audio.h>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Enums.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtCore/Stats.h>

#include "FFmpegCommand.h"

namespace Ui {
class CodecComparisonWindow;
}

class CodecComparisonWindow : public QWidget {
    Q_OBJECT

  private:
    Ui::CodecComparisonWindow *ui;
    VlcInstance *vlcInstance;
    VlcMedia *vlcMedia[4];
    VlcMediaPlayer *vlcMediaPlayers[4];
    QProcess streamingProcess;
    QProcess frameProbes[4];
    QProcess streamProbes[4];
    QQueue<char> framesQueues[4];

    QLabel *original;
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;

    QVector<CodecParametersWidget *> codecWidgets;

  public:
    explicit CodecComparisonWindow(QWidget *parent = 0);
    ~CodecComparisonWindow();
    void stream(QString streamingCommand);
    void closeEvent(QCloseEvent *event);
    void setManagers(int one, int two, int three);
    CodecParametersWidget *getManager(int i);
    void compareWindowStream(int first, int second, int third);
    static QString buildMultipleStreamingCommands(QString inputParameters, QString inputLocation,
                                                  QVector<QString> outputPrameters, QVector<QString> outputLocations);
    int previousFirst;
    int previousSecond;
    int previousThird;

    QString inputParameters;
    QString inputLocation;

    CodecParametersWidget *h261Manager = new CodecParametersWidget("h261", "h261", "matroska", this);
    CodecParametersWidget *h264Manager = new CodecParametersWidget("h264", "libx264", "matroska", this);
    CodecParametersWidget *h265Manager = new CodecParametersWidget("h265", "libx265", "matroska", this);
    CodecParametersWidget *mjpegManager = new CodecParametersWidget("mjpeg", "mjpeg", "matroska", this);
    CodecParametersWidget *mpeg1Manager = new CodecParametersWidget("mpeg1", "mpeg1video", "mpegts", this);
    CodecParametersWidget *mpeg2Manager = new CodecParametersWidget("mpeg2", "mpeg2video", "mpegts", this);

    void setInputLocation(QString location);
    void setInputParameters(QString parameters);
    QString getInputLocation();
    QString getInputParameters();

  private slots:
    void whilePlaying1();
    void whilePlaying2();
    void whilePlaying3();
    void compareWindowStream();

  signals:
    void statsChanged1(VlcStats *stats);
    void statsChanged2(VlcStats *stats);
    void statsChanged3(VlcStats *stats);
};

#endif // CODECCOMPARISONWINDOW_H
