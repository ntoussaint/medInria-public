/* medDatabaseNonPersitentReader.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:53:52 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 16:30:13 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 35
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medDatabaseNonPersistentController.h"
#include "medDatabaseController.h"
#include "medDatabaseNonPersistentItem.h"
#include "medDatabaseNonPersistentItem_p.h"
#include "medDatabaseNonPersistentReader.h"

#include <medAbstractDataImage.h>
#include <medMetaDataKeys.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>

class medDatabaseNonPersistentReaderPrivate
{
public:
    medDatabaseNonPersistentReaderPrivate(const QString& uuid):callerUuid(uuid){}
    QString file;
    bool isCancelled;
    const QString callerUuid;
};



medDatabaseNonPersistentReader::medDatabaseNonPersistentReader(const QString& file
                                                               ,const QString& callerUuid) : medJobItem(), d(new medDatabaseNonPersistentReaderPrivate(callerUuid))
{
    d->file = file;
    d->isCancelled = false;
    qDebug()<< "npr created with uuid:"<< d->callerUuid;
}

medDatabaseNonPersistentReader::~medDatabaseNonPersistentReader(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNonPersistentReader::run(void)
{
    qDebug() << DTK_PRETTY_FUNCTION;

    if (d->file.isEmpty())
        return;

    QDir dir(d->file);
    dir.setFilter(QDir::Files);

    QStringList fileList;
    if (dir.exists()) {
        QDirIterator directory_walker(d->file, QDir::Files, QDirIterator::Subdirectories);
    while (directory_walker.hasNext()) {
        fileList << directory_walker.next();
    }
    }
    else
        fileList << d->file;

    fileList.sort();

    QMap<QString, QStringList> imagesToWriteMap;

    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();

    int fileCount = fileList.count();
    int fileIndex = 0;

    QMap<QString, int> keyToInt;
    int currentIndex = 1;

    foreach (QString file, fileList) {

        emit progressed((int)(((qreal)fileIndex/(qreal)fileCount)*50.0));

        fileIndex++;

        QFileInfo fileInfo( file );

        dtkSmartPointer<dtkAbstractData> dtkdata;

        for (int i=0; i<readers.size(); i++) {
            dtkSmartPointer<dtkAbstractDataReader> dataReader;
            dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);
            if (dataReader->canRead( fileInfo.filePath() )) {
                dataReader->readInformation( fileInfo.filePath() );
                dtkdata = dataReader->data();
                break;
            }
        }

        if (!dtkdata)
            continue;

        if(!dtkdata->hasMetaData(medMetaDataKeys::PatientName.key()))
            dtkdata->addMetaData(medMetaDataKeys::PatientName.key(), QStringList() << fileInfo.baseName());

        if(!dtkdata->hasMetaData(medMetaDataKeys::StudyDescription.key()))
            dtkdata->addMetaData(medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy");

        if(!dtkdata->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
            dtkdata->addMetaData(medMetaDataKeys::SeriesDescription.key(), QStringList() << fileInfo.baseName());

    if(!dtkdata->hasMetaData(medMetaDataKeys::StudyID.key()))
            dtkdata->addMetaData(medMetaDataKeys::StudyID.key(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataKeys::SeriesID.key()))
            dtkdata->addMetaData(medMetaDataKeys::SeriesID.key(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataKeys::Orientation.key()))
            dtkdata->addMetaData(medMetaDataKeys::Orientation.key(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataKeys::SeriesNumber.key()))
            dtkdata->addMetaData(medMetaDataKeys::SeriesNumber.key(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataKeys::SequenceName.key()))
            dtkdata->addMetaData(medMetaDataKeys::SequenceName.key(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataKeys::SliceThickness.key()))
            dtkdata->addMetaData(medMetaDataKeys::SliceThickness.key(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataKeys::Rows.key()))
            dtkdata->addMetaData(medMetaDataKeys::Rows.key(), QStringList() << "");

    if(!dtkdata->hasMetaData(medMetaDataKeys::Columns.key()))
            dtkdata->addMetaData(medMetaDataKeys::Columns.key(), QStringList() << "");


    QString patientName = dtkdata->metaDataValues(medMetaDataKeys::PatientName.key())[0];
    QString studyName   = dtkdata->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];
    QString seriesName  = dtkdata->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];

    QString studyId = dtkdata->metaDataValues(medMetaDataKeys::StudyID.key())[0];
    QString seriesId = dtkdata->metaDataValues(medMetaDataKeys::SeriesID.key())[0];
    QString orientation = dtkdata->metaDataValues(medMetaDataKeys::Orientation.key())[0];
    QString seriesNumber = dtkdata->metaDataValues(medMetaDataKeys::SeriesNumber.key())[0];
    QString sequenceName = dtkdata->metaDataValues(medMetaDataKeys::SequenceName.key())[0];
    QString sliceThickness = dtkdata->metaDataValues(medMetaDataKeys::SliceThickness.key())[0];
    QString rows = dtkdata->metaDataValues(medMetaDataKeys::Rows.key())[0];
    QString columns = dtkdata->metaDataValues(medMetaDataKeys::Columns.key())[0];

    // define a unique key string to identify which volume an image belongs to.
    // we use: patientName, studyID, seriesID, orientation, seriesNumber, sequenceName, sliceThickness, rows, columns. All images of the same volume should share similar values of these parameters
    QString key = patientName+studyId+seriesId+orientation+seriesNumber+sequenceName+sliceThickness+rows+columns;
    if (!keyToInt.contains(key)) {
        keyToInt[key] = currentIndex;
        currentIndex++;
    }

    imagesToWriteMap[ key ] << fileInfo.filePath();
    }

    QMap<QString, int>::const_iterator itk = keyToInt.begin();


    // read and write images in mhd format

    QList<dtkAbstractData*> dtkDataList;

    QMap<QString, QStringList>::const_iterator it = imagesToWriteMap.begin();

    int imagesCount = imagesToWriteMap.count();
    int imageIndex = 0;

    for (it; it!=imagesToWriteMap.end(); it++) {

        emit progressed((int)(((qreal)imageIndex/(qreal)imagesCount)*50.0 + 50.0));

        imageIndex++;

        dtkSmartPointer<dtkAbstractData> imData;

        for (int i=0; i<readers.size(); i++) {
            dtkSmartPointer<dtkAbstractDataReader> dataReader;
            dataReader = dtkAbstractDataFactory::instance()->readerSmartPointer(readers[i]);

            if (dataReader->canRead( it.value() )) {

                //connect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));

                if (dataReader->read( it.value() )) {
                    imData = dataReader->data();

                    if (imData) {
                        if (!imData->hasMetaData(medMetaDataKeys::FilePaths.key()))
                            imData->addMetaData(medMetaDataKeys::FilePaths.key(), it.value());

                        if (!imData->hasMetaData(medMetaDataKeys::PatientName.key()))
                            imData->addMetaData(medMetaDataKeys::PatientName.key(), QStringList() << QFileInfo (it.value()[0]).baseName());

                        if (!imData->hasMetaData(medMetaDataKeys::StudyDescription.key()))
                            imData->addMetaData(medMetaDataKeys::StudyDescription.key(), QStringList() << "EmptyStudy");

                        if (!imData->hasMetaData(medMetaDataKeys::SeriesDescription.key()))
                            imData->addMetaData(medMetaDataKeys::SeriesDescription.key(), QStringList() << QFileInfo (it.value()[0]).baseName());

                        if(!imData->hasMetaData(medMetaDataKeys::StudyID.key()))
                            imData->addMetaData(medMetaDataKeys::StudyID.key(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataKeys::SeriesID.key()))
                            imData->addMetaData(medMetaDataKeys::SeriesID.key(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataKeys::Orientation.key()))
                            imData->addMetaData(medMetaDataKeys::Orientation.key(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataKeys::SeriesNumber.key()))
                            imData->addMetaData(medMetaDataKeys::SeriesNumber.key(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataKeys::SequenceName.key()))
                            imData->addMetaData(medMetaDataKeys::SequenceName.key(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataKeys::SliceThickness.key()))
                            imData->addMetaData(medMetaDataKeys::SliceThickness.key(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataKeys::Rows.key()))
                            imData->addMetaData(medMetaDataKeys::Rows.key(), QStringList() << "");

                        if(!imData->hasMetaData(medMetaDataKeys::Columns.key()))
                            imData->addMetaData(medMetaDataKeys::Columns.key(), QStringList() << "");

                        imData->addMetaData ("FileName", it.key() );

                        break;
                    }
                }

                //disconnect (dataReader, SIGNAL (progressed (int)), this, SLOT (setImportProgress(int)));
            }
        }

        if (!imData) {
            qDebug() << "Could not read data: " << it.value();
            continue;
        }

    dtkDataList.push_back (imData);

    }

    medDataIndex index;

    for (int i=0; i<dtkDataList.count(); i++) {

        dtkAbstractData *data = dtkDataList[i];

	QList<medDatabaseNonPersistentItem*> items = medDatabaseNonPersistentController::instance()->items();

	int     patientId   = -1;
	QString patientName = data->metaDataValues(medMetaDataKeys::PatientName.key())[0];

	// check if patient is already in the persistent database
	medDataIndex databaseIndex = medDatabaseController::instance()->indexForPatient (patientName);
	if (databaseIndex.isValid()) {
	    qDebug() << "Patient exists in the database, I reuse his Id";
	    patientId = databaseIndex.patientId();
	}
	else {
	    for (int i=0; i<items.count(); i++)
	        if (items[i]->name()==patientName) {
		    patientId = items[i]->index().patientId();
		    break;
		}
	}

	if (patientId==-1)
	    patientId = medDatabaseNonPersistentController::instance()->patientId(true);

	int     studyId   = -1;
	QString studyName = data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];

	databaseIndex = medDatabaseController::instance()->indexForStudy (patientName, studyName);
	if (databaseIndex.isValid()) {
	    qDebug() << "Study exists in the database, I reuse his Id";
	    studyId = databaseIndex.studyId();
	}
	else {
	    for (int i=0; i<items.count(); i++)
	        if (items[i]->name()==patientName && items[i]->studyName()==studyName) {
		    studyId = items[i]->index().studyId();
		    break;
		}
	}

	if (studyId==-1)
	    studyId = medDatabaseNonPersistentController::instance()->studyId(true);

	index = medDataIndex (medDatabaseNonPersistentController::instance()->dataSourceId(), patientId, studyId, medDatabaseNonPersistentController::instance()->seriesId(true), -1);

	QString seriesName = data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];

        QFileInfo info(d->file);

	medDatabaseNonPersistentItem *item = new medDatabaseNonPersistentItem;

        if(!patientName.isEmpty())
            item->d->name = patientName;
        else
            item->d->name = info.baseName();

	item->d->studyName = studyName;
	item->d->seriesName = seriesName;
	item->d->file = d->file;
	item->d->thumb = data->thumbnail();
	item->d->index = index;
	item->d->data = data;

        medDatabaseNonPersistentController::instance()->insert(index, item);
    }

    emit progressed(100);
    emit success(this);
    qDebug() << "uuid value before signal"<< d->callerUuid;
    emit nonPersistentRead(index,d->callerUuid);
}

void medDatabaseNonPersistentReader::onCancel( QObject* )
{
    d->isCancelled = true;
}


