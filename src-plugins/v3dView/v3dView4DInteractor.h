/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <med4DAbstractViewInteractor.h>
#include "v3dViewPluginExport.h"

class v3dView4DInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

/**
    \class v3dView4DInteractor
    \brief This important interactor handles the 4th dimension
    of data interacting with the viewing process.

    \see med4DAbstractViewInteractor medViewerToolBoxTime
    \author Nicolas Toussaint and Fatih Arslan
*/

class V3DVIEWPLUGIN_EXPORT v3dView4DInteractor: public med4DAbstractViewInteractor
{

    Q_OBJECT

public:
    v3dView4DInteractor();
    virtual ~v3dView4DInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setData(dtkAbstractData *data);
    virtual void setView(dtkAbstractView *view);

    virtual void enable();
    virtual void disable();

    /** Override dtkAbstractViewInteractor method */
    virtual bool isAutoEnabledWith ( dtkAbstractData * data );

    void setCurrentTime (double time);
    double getCurrentTime()
    { return this->currentTime; }
    /** Assigns ranfe array the minimum and maximum time of the sequence */
    void sequencesRange (double* range);
    /** Returns minimum time step for the sequence */
    double sequencesMinTimeStep();

public slots:
    void onDataAdded (dtkAbstractData* data);


protected:

signals:

public slots:
	//Add image to the sequence
    virtual void appendData(dtkAbstractData *data);

private:
    v3dView4DInteractorPrivate *d;

    double currentTime;
};

dtkAbstractViewInteractor *createV3dView4DInteractor();


