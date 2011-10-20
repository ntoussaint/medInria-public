/*
 * v3dViewAnnIntImageMaskHelper.cpp
 *
 *  Created on: 19 oct. 2011 08:34:58
 *      Author: jstark
 */

#include "v3dViewAnnIntImageMaskHelper.h"

#include "medImageMaskAnnotationData.h"

#include "v3dView.h"
#include "v3dViewAnnotationInteractor.h"

#include "vtkImageView2D.h"

#include <vtkCommand.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>


class v3dViewAnnIntImageMaskHelperPrivate {
public:
};

v3dViewAnnIntImageMaskHelper::v3dViewAnnIntImageMaskHelper(v3dViewAnnotationInteractor * annInt)
    : v3dViewAnnIntHelper(annInt) ,
    d(new v3dViewAnnIntImageMaskHelperPrivate)
{
}

v3dViewAnnIntImageMaskHelper::~v3dViewAnnIntImageMaskHelper()
{
    delete d; d = NULL;
}

bool v3dViewAnnIntImageMaskHelper::addAnnotation( medAnnotationData * annData )
{
    medImageMaskAnnotationData * imad = qobject_cast<medImageMaskAnnotationData*>(annData);
    if ( !imad ) 
        return false;

    medAbstractDataImage * dataImage = imad->maskData();
    v3dView * view = this->getV3dView();

    int numLayers = view->layerCount();
    view->setData(dataImage, numLayers);

    QList<double> scalars;
    QList<QColor> colors;
    for( medImageMaskAnnotationData::ColorMapType::const_iterator it(imad->colorMap().begin()), end(imad->colorMap().end());
        it != end; ++it ) 
    {
        scalars.push_back(it->first);
        colors.push_back(it->second);
    }
        
    int oldLayer = view->currentLayer();
    view->setCurrentLayer(numLayers);
    view->setColorLookupTable(scalars,colors);
    view->setCurrentLayer(oldLayer);

    this->annotationModified(annData);
    return true;
}


void v3dViewAnnIntImageMaskHelper::removeAnnotation( medAnnotationData * annData )
{
    v3dView * view = this->getV3dView();
    int numLayers = view->layerCount();
    for ( int i(0); i<numLayers; ++i) {
        if ( view->dataInList(i) == annData ) {
            view->removeOverlay(i);
            break;
        }
    }
}

void v3dViewAnnIntImageMaskHelper::annotationModified( medAnnotationData * annData )
{
    medImageMaskAnnotationData * imad = qobject_cast<medImageMaskAnnotationData*>(annData);
    v3dView * view = this->getV3dView();

    if ( !imad ) 
        return;

    int layer(-1);
    for ( int i(0), end(view->layerCount()); i<end; ++i ) {
        if ( view->dataInList(i) == imad->maskData() ) {
            layer = i;
            break;
        }
    }
    if ( layer < 0 )
        return;

    view->setData(imad, layer);
    view->setData(imad->maskData(), layer);
    view->update();
}

