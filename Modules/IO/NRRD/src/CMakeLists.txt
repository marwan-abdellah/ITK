set(ITKIONRRD_SRC
itkNrrdImageIOFactory.cxx
itkNrrdImageIO.cxx
)

add_library(ITKIONRRD ${ITKIONRRD_SRC})
target_link_libraries(ITKIONRRD  ${ITKNrrdIO_LIBRARIES} ${ITKIOImageBase_LIBRARIES})
itk_module_target(ITKIONRRD)
