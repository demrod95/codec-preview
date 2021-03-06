#ifndef CODEC_H
#define CODEC_H

#include <QList>
#include <QMap>

class Codec {
  public:
    Codec();

  protected:
    QMap<QString, QMap<QString, QString>> *parameters = new QMap<QString, QMap<QString, QString>>();
    QMap<QString, QMap<QString, QString>> *comboBoxes = new QMap<QString, QMap<QString, QString>>();
    QMap<QString, QMap<QString, QString>> *checkBoxes = new QMap<QString, QMap<QString, QString>>();
    QMap<QString, QMap<QString, QString>> *sliders = new QMap<QString, QMap<QString, QString>>();
    QMap<QString, QMap<QString, QString>> *stats = new QMap<QString, QMap<QString, QString>>();
    QList<QString> *parameterOrder;

    void addParameter(QString paramName, QString paramValue, QString paramDefault, QString width = "0");
    void addComboBox(QString paramName, QString paramValue, QMap<QString, QString> paramMap, QString paramDefault);
    void addCheckBox(QString paramName, QString command, bool state);
    void addSlider(QString paramName, QString paramValue, QString paramDefault, QString paramMin, QString paramMax);
    void addStat();

  public:
    QMap<QString, QString> getParameter(QString paramName);
    QMap<QString, QString> getComboBox(QString &paramName);
    QMap<QString, QString> getCheckBox(QString paramName);
    QMap<QString, QString> getSlider(QString paramName);
    QList<QString> getParameterKeys();
    QList<QString> getComboBoxKeys();
    QList<QString> getCheckBoxKeys();
    QList<QString> getSliderKeys();
    QList<QString> *getParameterOrder();
};

#endif // CODEC_H
