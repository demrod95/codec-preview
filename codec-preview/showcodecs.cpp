#include "showcodecs.h"
#include "ui_showcodecs.h"

ShowCodecs::ShowCodecs(QWidget *parent)
    : QWidget(parent), ui(new Ui::ShowCodecs) {
    ui->setupUi(this);

    vlcInstance = new VlcInstance(VlcCommon::args(), NULL);

    for (int i = 0; i < 4; i++) {
        // Initialise media objects
        vlcMedia[i] = new VlcMedia(VIDEO_PROTOCOLS[i] + "://@" +
                                       VIDEO_HOSTS[i] + ":" + VIDEO_PORTS[i],
                                   false, vlcInstance);

        // Initialise video displays
        vlcMediaPlayers[i] = new VlcMediaPlayer(vlcInstance);
        vlcMediaPlayers[i]->audio()->setMute(true);
        vlcMediaPlayers[i]->openOnly(vlcMedia[i]);
    }

    // Connect video widgets
    vlcMediaPlayers[0]->setVideoWidget(ui->rawVideo);
    ui->rawVideo->setMediaPlayer(vlcMediaPlayers[0]);
    vlcMediaPlayers[1]->setVideoWidget(ui->encodedVideo1);
    ui->encodedVideo1->setMediaPlayer(vlcMediaPlayers[1]);
    vlcMediaPlayers[2]->setVideoWidget(ui->encodedVideo2);
    ui->encodedVideo2->setMediaPlayer(vlcMediaPlayers[2]);
    vlcMediaPlayers[3]->setVideoWidget(ui->encodedVideo3);
    ui->encodedVideo3->setMediaPlayer(vlcMediaPlayers[3]);
}

ShowCodecs::~ShowCodecs() { delete ui; }

void ShowCodecs::broadcast(QString streamingCommand1, QString streamingCommand2,
                           QString streamingCommand3,
                           QString streamingCommand4) {
    for (int i = 0; i < 4; i++) {
        // Stop the player
        vlcMediaPlayers[i]->stop();
        // Kill streaming process
        streamingProcesses[i].kill();
        streamingProcesses[i].waitForFinished();
    }

    // Start streaming processes
    streamingProcesses[0].start(streamingCommand1);
    streamingProcesses[1].start(streamingCommand2);
    streamingProcesses[2].start(streamingCommand3);
    streamingProcesses[3].start(streamingCommand4);

    qDebug() << "Starting the players...";
    for (int i = 0; i < 4; i++) {
        vlcMediaPlayers[i]->play();
    }
}