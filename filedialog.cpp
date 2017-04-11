#include "filedialog.h"

FileDialog::FileDialog(const QString folder, QWidget *parent) : QScrollArea(parent)
{
    pos = QPoint(100, 100);
    dirName = folder;
    innerWidget = new QWidget(/*this*/);
    int r, g, b;
    LabelButton *btn;
    QDir dir(folder);
    QStringList files = dir.entryList(QStringList("*.xml"));

    for (int i = 0; i < files.size(); i++ ) {
        btn = new LabelButton(innerWidget);
        btn->innerLabel->setGeometry(5, 5, 0, 0);
        btn->setStyleSheet("background: white");
        btn->setText(files[i]);
        QObject::connect(btn, SIGNAL(click()),          this, SLOT(hide()));
        QObject::connect(btn, SIGNAL(click()),          this, SLOT(deleteLater()));
        QObject::connect(btn, SIGNAL(click(QString)),   this, SLOT(clickSlot(QString)));

        //random color
        r = (rand() % 254) + 0;
        g = (rand() % 254) + 0;
        b = (rand() % 254) + 0;
        btn->setStyleSheet("background: rgb(" + QString::number(r) + "," +
                                                QString::number(g) + "," +
                                                QString::number(b) + ")");

        //preview
        QFile file(dirName + files[i]);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QDomDocument xml("file");
            xml.setContent(&file);
            if (!xml.elementsByTagName("preview").isEmpty()) {
                QLabel* preview = new QLabel();
                preview->setParent(btn);
                QPixmap previewImg(dirName + xml.elementsByTagName("preview").at(0).toElement().text());
                previewImg = (previewImg.width() > previewImg.height())?(previewImg.scaledToWidth(buttonSize - 10)):(previewImg.scaledToHeight(buttonSize - 45));
                preview->setPixmap(previewImg);
                preview->adjustSize();
                preview->setGeometry(5, 35, preview->width(), preview->height());
            }
            file.close();
        }
        buttons.push_back(btn);
    }

    setWidget(innerWidget);
    adjustGeometry();
}

FileDialog::~FileDialog()
{
    for (int i = 0; i < buttons.size(); i++) {
        delete buttons[i];
    }
    innerWidget->deleteLater();
}

void FileDialog::adjustGeometry()
{
    for (int i = 0; i < buttons.size(); i++) {
        buttons[i]->setGeometry((i % columns)*(buttonSize + buttonMargin) + buttonMargin, (i / columns)*(buttonSize + buttonMargin) + buttonMargin,
                                buttonSize, buttonSize);
    }

    innerWidget->adjustSize();
    setGeometry(pos.x(), pos.y(), 100, 100);
    setMinimumSize(QSize( (buttonSize+buttonMargin) * columns + buttonMargin , 880));
    //setMaximumWidth((buttonSize+buttonMargin) * columns);
}

void FileDialog::clickSlot(QString str) {
    click(dirName + str);
}

