/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkGrayscaleGrindPeakImageFilter.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrindPeakImageFilter_h
#define __itkGrindPeakImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk {

/** \class GrayscaleGrindPeakImageFilter
 * \brief Remove local maxima not connected to the boundary of the image.
 *
 * GrayscaleGrindPeakImageFilter removes peaks in a grayscale image.
 * Peaks are local maxima in the grayscale topography that are not
 * connected to boundaries of the image. Gray level values adjacent to
 * a peak are extrapolated through the peak.
 *
 * This filter is used to smooth over local maxima without affecting
 * the values of local minima.  If you take the difference between the
 * output of this filter and the original image (and perhaps threshold
 * the difference above a small value), you'll obtain a map of the
 * local maxima.
 *
 * This filter uses the GrayscaleGeodesicDilateImageFilter.  It
 * provides its own input as the "mask" input to the geodesic
 * erosion.  The "marker" image for the geodesic erosion is
 * constructed such that boundary pixels match the boundary pixels of
 * the input image and the interior pixels are set to the minimum
 * pixel value in the input image.
 *
 * This filter is the dual to the GrayscaleFillholeImageFilter which
 * implements the Fillhole algorithm.  Since it is a dual, it is
 * somewhat superfluous but is provided as a convenience.
 *
 * Geodesic morphology and the Fillhole algorithm is described in
 * Chapter 6 of Pierre Soille's book "Morphological Image Analysis:
 * Principles and Applications", Second Edition, Springer, 2003.
 *
 * \sa GrayscaleGeodesicDilateImageFilter
 * \sa MorphologyImageFilter, GrayscaleDilateImageFilter, GrayscaleFunctionDilateImageFilter, BinaryDilateImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */
template<class TInputImage, class TOutputImage>
class ITK_EXPORT GrayscaleGrindPeakImageFilter : 
  public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef GrayscaleGrindPeakImageFilter Self;
  typedef ImageToImageFilter<TInputImage, TOutputImage>
    Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename InputImageType::Pointer         InputImagePointer;
  typedef typename InputImageType::ConstPointer    InputImageConstPointer;
  typedef typename InputImageType::RegionType      InputImageRegionType;
  typedef typename InputImageType::PixelType       InputImagePixelType;
  typedef typename OutputImageType::Pointer        OutputImagePointer;
  typedef typename OutputImageType::ConstPointer   OutputImageConstPointer;
  typedef typename OutputImageType::RegionType     OutputImageRegionType;
  typedef typename OutputImageType::PixelType      OutputImagePixelType;
  
  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(GrayscaleGrindPeakImageFilter, 
               ImageToImageFilter);
  
  /** Get the number of iterations used to produce the current
   * output. */
  itkGetMacro(NumberOfIterationsUsed, unsigned long);

protected:
  GrayscaleGrindPeakImageFilter();
  ~GrayscaleGrindPeakImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** GrayscaleGrindPeakImageFilter needs the entire input be
   * available. Thus, it needs to provide an implementation of
   * GenerateInputRequestedRegion(). */
  void GenerateInputRequestedRegion() ;

  /** GrayscaleGrindPeakImageFilter will produce the entire output. */
  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));
  
  /** Single-threaded version of GenerateData.  This filter delegates
   * to GrayscaleGeodesicDilateImageFilter. */
  void GenerateData();
  

private:
  GrayscaleGrindPeakImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  unsigned long m_NumberOfIterationsUsed;
} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGrayscaleGrindPeakImageFilter.txx"
#endif

#endif


