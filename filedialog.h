#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QScrollArea>
#include <QDir>
#include <labelbutton.h>

#include <QDomDocument>
#include <QLabel>

class FileDialog;

class FileDialog : public QScrollArea
{
    Q_OBJECT
public:
    FileDialog(const QString folder, QWidget *parent = 0);

    ~FileDialog();
    void adjustGeometry();
    int buttonSize = 332;
    int buttonMargin = 10;
    int columns = 5;
    QPoint pos;

signals:
    void click(QString str);

private:
    QWidget* innerWidget;
    QVector<LabelButton*> buttons;
    QString dirName;

private slots:
    void clickSlot(QString str);

};

#endif // FILEDIALOG_H
