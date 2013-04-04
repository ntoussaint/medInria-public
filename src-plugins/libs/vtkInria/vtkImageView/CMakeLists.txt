project(vtkImageView)

set(${PROJECT_NAME}_HEADERS
    vtkImageView.h
    vtkImageView2D.h
    vtkImageView3D.h
    vtkInteractorStyleImageView2D.h
    vtkImageView2DCommand.h
    vtkImageViewCornerAnnotation.h
    vtkAxes2DWidget.h
    vtkDataSet2DWidget.h
    vtkImageViewCollection.h
    vtkImageView3DCroppingBoxCallback.h
    vtkSmartVolumeMapper.h
)

set(${PROJECT_NAME}_SOURCES
    vtkImageView.cxx
    vtkImageView2D.cxx
    vtkImageView3D.cxx
    vtkInteractorStyleImageView2D.cxx
    vtkImageView2DCommand.cxx
    vtkImageViewCornerAnnotation.cxx
    vtkAxes2DWidget.cxx
    vtkDataSet2DWidget.cxx
    vtkImageViewCollection.cxx
    vtkImageView3DCroppingBoxCallback.cxx
    vtkSmartVolumeMapper.cxx
)
if(VTKINRIA_USE_PRECOMPILED_HEADERS)
    add_precompiled_header(${PROJECT_NAME}_SOURCES_PCH 
    "vtkInriaPch/vtkInriaPch.h" "../vtkInriaPch/vtkInriaPch.cpp" 
    ${${PROJECT_NAME}_SOURCES})
endif(VTKINRIA_USE_PRECOMPILED_HEADERS)

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_SOURCES_PCH}
    ${${PROJECT_NAME}_SOURCES}
    ${${PROJECT_NAME}_HEADERS}
)

target_link_libraries(${PROJECT_NAME}
    vtkRendering
    vtkWidgets
    vtkWidgetsAddOn
    vtkVolumeRendering
    vtkVisuManagement
    ITKCommon
    ITKBasicFilters
)

IF(VTKINRIA_BUILD_TESTING)
    ADD_SUBDIRECTORY(Testing)
ENDIF(VTKINRIA_BUILD_TESTING)

# include header files in group for visual studio
SOURCE_GROUP( "Header Files" FILES ${${PROJECT_NAME}_HEADERS})

