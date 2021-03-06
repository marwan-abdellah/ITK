/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkRGBToLuminanceImageFilter_h
#define __itkRGBToLuminanceImageFilter_h

#include "itkUnaryFunctorImageFilter.h"

namespace itk
{
/** \class RGBToLuminanceImageFilter
 * \brief Converts an RGB image into a grayscale image.
 *
 * This filters converts an RGB image into a Luminance on by computing
 * pixel-wise a linear combination on the Red, Green and Blue channels. The
 * pixel type of the input image must have a GetLuminance() method. This is the
 * case of the itk::RGBPixel class.
 *
 * \ingroup IntensityImageFilters  MultiThreaded
 * \ingroup ITKImageIntensity
 */
namespace Functor
{
template< class TInput, class TOutput >
class RGBToLuminance
{
public:
  typedef typename TInput::ComponentType                         ComponentType;
  typedef typename itk::NumericTraits< ComponentType >::RealType RealType;

  RGBToLuminance() {}
  ~RGBToLuminance() {}
  bool operator!=(const RGBToLuminance &) const
  {
    return false;
  }

  bool operator==(const RGBToLuminance & other) const
  {
    return !( *this != other );
  }

  inline TOutput operator()(const TInput & A) const
  { return static_cast< TOutput >( A.GetLuminance() ); }
};
}

template< class TInputImage, class TOutputImage >
class ITK_EXPORT RGBToLuminanceImageFilter:
  public
  UnaryFunctorImageFilter< TInputImage, TOutputImage,
                           Functor::RGBToLuminance<
                             typename TInputImage::PixelType,
                             typename TOutputImage::PixelType >   >
{
public:
  /** Standard class typedefs. */
  typedef RGBToLuminanceImageFilter Self;
  typedef UnaryFunctorImageFilter<
    TInputImage, TOutputImage,
    Functor::RGBToLuminance< typename TInputImage::PixelType,
                             typename TOutputImage::PixelType > >  Superclass;

  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(RGBToLuminanceImageFilter,
               UnaryFunctorImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( InputHasNumericTraitsCheck,
                   ( Concept::HasNumericTraits< typename TInputImage::PixelType::ComponentType > ) );
  /** End concept checking */
#endif

protected:
  RGBToLuminanceImageFilter() {}
  virtual ~RGBToLuminanceImageFilter() {}

private:
  RGBToLuminanceImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);            //purposely not implemented
};
} // end namespace itk

#endif
