#include "VideoStatisticsWidget.h"
#include "ui_VideoStatisticsWidget.h"

VideoStatisticsWidget::VideoStatisticsWidget(QWidget *parent) : QWidget(parent), ui(new Ui::VideoStatisticsWidget) {
    ui->setupUi(this);

    // redirect stream probe output to file (cannot read it in any other way
    // unfortunately)
    streamProbeProcess.setStandardOutputFile(PROBE_FILE_NAME);

    // react to frame probe output with parseFrameProbeOutput
    connect(&frameProbeProcess, &QProcess::readyRead, this, &VideoStatisticsWidget::parseFrameProbeOutput);

    // react to stream probe output with parseStreamProbeOutput
    connect(&streamProbeProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(parseStreamProbeOutput(int, QProcess::ExitStatus)));

    // timer ze o ja pierdole
    interval = 200;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStats()));
    ui->lBitrate->setToolTip(paramManager.getHint("Bitrate"));

    ui->frameRate->setFixedWidth(42);
    ui->bitRate->setFixedWidth(218);
    ui->resolution->setFixedWidth(98);
    ui->quantization->setFixedWidth(102);
    ui->frameTypes->setFixedWidth(110);
}

VideoStatisticsWidget::~VideoStatisticsWidget() { delete ui; }

void VideoStatisticsWidget::stopProbe() {
    frameProbeProcess.kill();
    streamProbeProcess.kill();
    frameProbeProcess.waitForFinished();
    streamProbeProcess.waitForFinished();
}

void VideoStatisticsWidget::parseFrameProbeOutput() {

    // read output line by line
    while (frameProbeProcess.canReadLine()) {
        QString output = frameProbeProcess.readLine();

        // find frame type
        if (output.startsWith("pict_type=")) {
            // put single character describing frame type into queue
            framesQueue.enqueue(output.toUtf8().constData()[10]);

            // throw out oldest frame types if over queue size limit
            if (framesQueue.size() > 16)
                stopProbe();
            // framesQueue.dequeue();

            // create snapshot of queue to display in widget
            QString framesQueueSnapshot;

            QListIterator<char> i(framesQueue);

            while (i.hasNext()) {
                framesQueueSnapshot.append(i.next());
            }

            ui->frameTypes->setText(framesQueueSnapshot);
        }

        // find frame width
        if (output.startsWith("width=")) {
            width = output.mid(6, output.length()).toInt();
        }

        // find frame height
        if (output.startsWith("height=")) {
            height = output.mid(7, output.length()).toInt();
        }
    }
}

void VideoStatisticsWidget::clearFrameQueue() {
    framesQueue.clear();
    ui->frameTypes->setText("");
}

void VideoStatisticsWidget::startFrameProbe(QString command) { frameProbeProcess.start(command); }

void VideoStatisticsWidget::startStreamProbe(QString command) { streamProbeProcess.start(command); }

void VideoStatisticsWidget::setFrameTypeText(QString text) { ui->frameTypes->setText(text); }

void VideoStatisticsWidget::parseStreamProbeOutput(int a, QProcess::ExitStatus b) {
    // to silence unused warning
    (void)a;
    (void)b;

    // read from data dumped into file
    std::ifstream myReadFile;
    myReadFile.open(PROBE_FILE_NAME.toUtf8().data());
    char output[100];
    if (myReadFile.is_open()) {
        while (!myReadFile.eof()) {

            myReadFile >> output;

            QString fileOutput = QString(output);

            // find frame rate
            if (fileOutput.startsWith("avg_frame_rate=")) {
                QRegExp rx("\\d+");
                rx.indexIn(fileOutput, 15);
                QString num = rx.cap();
                rx.lastIndexIn(fileOutput);
                QString den = rx.cap();
                float frameRate = float(num.toInt()) / den.toInt();
                ui->frameRate->setText(QString::number(frameRate));
            }
        }
    } else {
        qDebug() << "ERROR: cannot access file";
    }
    myReadFile.close();
}

void VideoStatisticsWidget::onStatsChange(VlcStats *stats) {
    // ui->decodedBlocks->setText(QString::number(stats->decoded_video));
    // ui->bitRate->setText(QString::number(stats->input_bitrate*10000));
    // bitrate is set in the another way
    ui->framesDropped->setText(QString::number(stats->lost_pictures));
    // ui->bytesRead->setText(QString::number(stats->read_bytes / 100.0));
    ui->framesCount->setText(QString::number(stats->displayed_pictures));
    if (!timer->isActive())
        timer->start(interval);
}

QString VideoStatisticsWidget::getBitrate(QString line) {
    // QString bitrate;
    QRegExp rx = QRegExp("avg_br=\\s*[0-9]+.?[0-9]+kbits/s"); // avg_br= 29.9kbits/s
    QRegExp rx2 = QRegExp("[0-9]+.?[0-9]+kbits/s");           // 29.9kbits/s
    rx.indexIn(line);
    rx2.indexIn(rx.capturedTexts().at(0));
    return rx2.capturedTexts().at(0);
}

QString VideoStatisticsWidget::getDelay(QString *lines, const int OUTS) {
    QString times[OUTS];
    QRegExp rx = QRegExp("time=\\s*[0-9]+.?[0-9]+"); // time= 0.88
    QRegExp rx2 = QRegExp("[0-9]+.?[0-9]+");         // 0.88
    for (int i = 0; i < OUTS; i++) {
        rx.indexIn(lines[i]);
        rx2.indexIn(rx.capturedTexts().at(0));
        times[i] = rx2.capturedTexts().at(0);
    }
    double delay = qFabs(times[1].toDouble() - times[0].toDouble());
    return QString::number(delay) + "s";
}

int VideoStatisticsWidget::getOut(QString line) {
    QRegExp rx = QRegExp("out=\\s*[0-1]");
    rx.indexIn(line);
    QString out = rx.capturedTexts().at(0);
    out = out[out.length() - 1];
    int outNo = out.toInt();
    return outNo;
}

QString VideoStatisticsWidget::getStreamSize(QString line) {
    QRegExp rx = QRegExp("s_size=\\s*[0-9]+(.?[0-9]+)?(kB)?(MB)?");
    QRegExp rx2 = QRegExp("[0-9]+(.?[0-9]+)?(kB)?(MB)?");
    rx.indexIn(line);
    rx2.indexIn(rx.capturedTexts().at(0));
    return rx2.capturedTexts().at(0);
}

QString VideoStatisticsWidget::getFrameSize(QString line) {
    QRegExp rx = QRegExp("f_size=\\s*[0-9]+(.?[0-9]+)?");
    QRegExp rx2 = QRegExp("[0-9]+(.?[0-9]+)?");
    rx.indexIn(line);
    rx2.indexIn(rx.capturedTexts().at(0));
    return rx2.capturedTexts().at(0) + "B";
}

QString VideoStatisticsWidget::getTimeElapsed(QString line) {
    QRegExp rx = QRegExp("time=\\s*[0-9]+(.?[0-9]+)?");
    QRegExp rx2 = QRegExp("[0-9]+(.?[0-9]+)?");
    rx.indexIn(line);
    rx2.indexIn(rx.capturedTexts().at(0));
    return rx2.capturedTexts().at(0);
}

QString VideoStatisticsWidget::getQuantization(QString line) {
    QRegExp rx = QRegExp("q=\\s*[0-9]+(.?[0-9]+)?");
    QRegExp rx2 = QRegExp("[0-9]+(.?[0-9]+)?");
    rx.indexIn(line);
    rx2.indexIn(rx.capturedTexts().at(0));
    return rx2.capturedTexts().at(0);
}

void VideoStatisticsWidget::updateStats() {
    // XD
    const int OUTS = 2;
    QString lines[OUTS];
    QString lastLines[OUTS];
    QFile inputFile(STATS_FILE_NAME);
    if (inputFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&inputFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.length() >= MIN_LENGTH_LINE) {
                int out = getOut(line);
                lines[out] = line;
                if (out == (OUTS - 1)) {
                    for (int i = 0; i < OUTS; i++)
                        lastLines[i] = lines[i];
                }
            }
        }
    }

    inputFile.close();
    ui->bitRate->setText(getBitrate(lastLines[1]));
    ui->delay->setText(getDelay(lastLines, OUTS));
    ui->frameSize->setText(getFrameSize(lastLines[1]));
    ui->streamSize->setText(getStreamSize(lastLines[1]));
    ui->timeElapsed->setText(getTimeElapsed(lastLines[1]) + "s");
    ui->quantization->setText(getQuantization(lastLines[1]));

    ui->resolution->setText(QString::number(width) + "x" + QString::number(height));
}

QSize VideoStatisticsWidget::getFrameSize() {
    QSize frameSize(width, height);
    return frameSize;
}
