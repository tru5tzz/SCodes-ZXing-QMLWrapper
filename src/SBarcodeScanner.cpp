#include "SBarcodeScanner.h"

#include <QtGlobal>

SBarcodeScanner::SBarcodeScanner(QObject *parent)
    : QVideoSink(parent) 
    , web_player(nullptr) 
{

    connect(&m_decoder, &SBarcodeDecoder::capturedChanged, this, &SBarcodeScanner::setCaptured);
    connect(this, &QVideoSink::videoFrameChanged, this, &SBarcodeScanner::handleFrameCaptured);

    worker = new Worker(this);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &SBarcodeScanner::process, worker, &Worker::process);
    workerThread.start();

    initPlayer();
}

SBarcodeDecoder *SBarcodeScanner::getDecoder()
{
    return &m_decoder;
}

void SBarcodeScanner::initPlayer() {
    web_player = new QMediaPlayer(this);

    web_player->setSource(QUrl("http://192.168.1.101:8080/video"));
    web_player->setVideoOutput(this);
    qDebug() << "Name of the output object is " << web_player->videoOutput()->objectName();

    m_decoder.setResolution(this->videoSize().rwidth(), this->videoSize().rheight());
    qDebug() << "Size of the video get from QMediaPlayer is W: " << 
                this->videoSize().width() <<
                " H: "  <<
                this->videoSize().height();

    connect(web_player, &QMediaPlayer::errorOccurred, this, &SBarcodeScanner::mediaPlayerErrorHandle);
    connect(web_player, &QMediaPlayer::mediaStatusChanged, this, &SBarcodeScanner::mediaPlayerStatusHandle);
    
    web_player->play();
}

SBarcodeScanner::~SBarcodeScanner()
{
    workerThread.quit();
    workerThread.wait();
}

void SBarcodeScanner::handleFrameCaptured(const QVideoFrame &frame) {
    //qDebug() << "Frame Changed";
    if(m_processing) {
        emit process(m_decoder.videoFrameToImage(frame, captureRect().toRect()));

        if(m_videoSink) {
            m_videoSink->setVideoFrame(frame);
        }
    }
    pauseProcessing();
}

void SBarcodeScanner::imageProcess(SBarcodeDecoder *decoder, const QImage &image, ZXing::BarcodeFormats formats) {
    decoder->process(image, formats);
    continueProcessing();
}

void SBarcodeScanner::setProcessing(bool p) {
    m_processing = p;
}

void SBarcodeScanner::pauseProcessing() {
    disconnect(this, &QVideoSink::videoFrameChanged, this, &SBarcodeScanner::handleFrameCaptured);
}

void SBarcodeScanner::continueProcessing() {
    connect(this, &QVideoSink::videoFrameChanged, this, &SBarcodeScanner::handleFrameCaptured);
}

void SBarcodeScanner::setCaptured(const QString &captured)
{
    if (m_captured == captured) {
        return;
    }

    m_captured = captured;
    emit capturedChanged(m_captured);
}

QString SBarcodeScanner::captured() const
{
    return m_captured;
}

QRectF SBarcodeScanner::captureRect() const {
    return m_captureRect;
}

void SBarcodeScanner::setCaptureRect(const QRectF &captureRect)
{
    if (captureRect == m_captureRect) {
        return;
    }

    m_captureRect = captureRect;
    emit captureRectChanged(m_captureRect);
}

QVideoSink *SBarcodeScanner::videoSink() const {
    return m_videoSink.get();
}

void SBarcodeScanner::setVideoSink(QVideoSink *videoSink) {
    if (m_videoSink == videoSink) {
        return;
    }

    m_videoSink = videoSink;
    emit videoSinkChanged();
}

void SBarcodeScanner::mediaPlayerErrorHandle(QMediaPlayer::Error error, const QString &errorString) {
    qDebug() << "Media Player Error changed into: " << errorString;
}

void SBarcodeScanner::mediaPlayerStatusHandle(QMediaPlayer::MediaStatus status) {
    qDebug() << "Media Player Status changed into: " << status;
}

