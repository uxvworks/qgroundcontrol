#include "QGCUASFileViewMulti.h"
#include "ui_QGCUASFileViewMulti.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "QGCUASFileView.h"

QGCUASFileViewMulti::QGCUASFileViewMulti(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCUASFileViewMulti)
{
    ui->setupUi(this);
    setMinimumSize(600, 80);
    connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(systemCreated(UASInterface*)));
    connect(UASManager::instance(), SIGNAL(activeUASSet(int)), this, SLOT(systemSetActive(int)));

    if (UASManager::instance()->getActiveUAS()) {
        systemCreated(UASManager::instance()->getActiveUAS());
        systemSetActive(UASManager::instance()->getActiveUAS()->getUASID());
    }

}

void QGCUASFileViewMulti::systemDeleted(QObject* uas)
{
    UASInterface* mav = dynamic_cast<UASInterface*>(uas);
    if (mav)
    {
        int id = mav->getUASID();
        QGCUASFileView* list = lists.value(id, NULL);
        if (list)
        {
            delete list;
            lists.remove(id);
        }
    }
}

void QGCUASFileViewMulti::systemCreated(UASInterface* uas)
{
    if (!uas) {
        return;
    }

    QGCUASFileView* list = new QGCUASFileView(ui->stackedWidget, uas->getFileManager());
    lists.insert(uas->getUASID(), list);
    ui->stackedWidget->addWidget(list);
    // Ensure widget is deleted when system is deleted
    connect(uas, SIGNAL(destroyed(QObject*)), this, SLOT(systemDeleted(QObject*)));
}

void QGCUASFileViewMulti::systemSetActive(int uas)
{
    QGCUASFileView* list = lists.value(uas, NULL);
    if (list) {
        ui->stackedWidget->setCurrentWidget(list);
    }
}

QGCUASFileViewMulti::~QGCUASFileViewMulti()
{
    delete ui;
}

void QGCUASFileViewMulti::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
