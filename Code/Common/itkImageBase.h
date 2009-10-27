/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageBase_h
#define __itkImageBase_h

#include "itkDataObject.h"

#include "itkImageRegion.h"
#include "itkIndex.h"
#include "itkObjectFactory.h"
#include "itkOffset.h"
#include "itkPoint.h"
#include "itkSize.h"
#include "itkFixedArray.h"
#include "itkPoint.h"
#include "itkMatrix.h"
#include "itkContinuousIndex.h"
#include "itkImageHelper.h"
#include <vnl/vnl_matrix_fixed.txx>

#include "itkImageRegion.h"

#ifdef ITK_USE_TEMPLATE_META_PROGRAMMING_LOOP_UNROLLING
#include "itkImageTransformHelper.h"
#endif

namespace itk
{

/**
 * Due to a bug in MSVC, an enum value cannot be accessed out of a template
 * parameter until the template class opens.  In order for templated classes
 * to access the dimension of an image template parameter in defining their
 * own dimension, this class is needed as a work-around.
 */
template <typename TImage>
struct GetImageDimension
{
  itkStaticConstMacro(ImageDimension, unsigned int, TImage::ImageDimension);
};

/** \class ImageBase
 * \brief Base class for templated image classes.
 *
 * ImageBase is the base class for the templated Image
 * classes. ImageBase is templated over the dimension of the image. It
 * provides the API and ivars that depend solely on the dimension of
 * the image.  ImageBase does not store any of the image (pixel) data.
 * Storage for the pixels and the pixel access methods are defined in
 * subclasses of ImageBase, namely Image and ImageAdaptor.
 *
 * There are three sets of meta-data describing an image. These are "Region"
 * objects that define a portion of an image via a starting index for the
 * image array and a size. The ivar LargestPossibleRegion defines the size
 * and starting index of the image dataset. The entire image dataset, however,
 * may not be resident in memory. The region of the image that is resident in
 * memory is defined by the "BufferedRegion". The Buffer is a contiguous block
 * of memory.  The third set of meta-data defines a region of interest, called
 * the "RequestedRegion". The RequestedRegion is used by the pipeline
 * execution model to define what a filter is requested to produce.
 *
 * [RegionIndex, RegionSize] C [BufferIndex, BufferSize]
 *                           C [ImageIndex, ImageSize]
 *
 * \ingroup ImageObjects
 * \ingroup ITKSystemObjects
 *
 */
template<unsigned int VImageDimension=2>
class ITK_EXPORT ImageBase : public DataObject
{
public:
  /** Standard typedefs. */
  typedef ImageBase                     Self;
  typedef DataObject                    Superclass;
  typedef SmartPointer<Self>            Pointer;
  typedef SmartPointer<const Self>      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageBase, DataObject);

  /** Dimension of the image.  This constant is used by functions that are
   * templated over image type (as opposed to being templated over pixel
   * type and dimension) when they need compile time access to the dimension
   * of the image. */
  itkStaticConstMacro(ImageDimension, unsigned int, VImageDimension );

  /** Index typedef support. An index is used to access pixel values. */
  typedef Index<VImageDimension>                  IndexType;
  typedef typename IndexType::IndexValueType      IndexValueType;

  /** Offset typedef support. An offset represent relative position
   * between indices. */
  typedef Offset<VImageDimension>                 OffsetType;
  typedef typename OffsetType::OffsetValueType    OffsetValueType;

  /** Size typedef support. A size is used to define region bounds. */
  typedef Size<VImageDimension>                   SizeType;
  typedef typename SizeType::SizeValueType        SizeValueType;

  /** Region typedef support. A region is used to specify a subset of an image. */
  typedef ImageRegion<VImageDimension>            RegionType;

  /** Spacing typedef support.  Spacing holds the size of a pixel.  The
   * spacing is the geometric distance between image samples. ITK only
   * supports positive spacing value: negative values may cause
   * undesirable results.  */
  typedef double                                    SpacingValueType;
  typedef Vector<SpacingValueType, VImageDimension> SpacingType;

  /** Origin typedef support.  The origin is the geometric coordinates
   * of the index (0,0). */
  typedef double                                    PointValueType;
  typedef Point<PointValueType, VImageDimension>    PointType;

  /** Direction typedef support.  The Direction is a matix of
   * direction cosines that specify the direction between samples.
   * */
  typedef Matrix<double, VImageDimension, VImageDimension> DirectionType;

  /** Restore object to initialized state. */
  void Initialize();

  /** Image dimension. The dimension of an image is fixed at construction. */
  static unsigned int GetImageDimension()
    { return VImageDimension; }

  /** Set the origin of the image. The origin is the geometric
   * coordinates of the image origin.  It is stored internally
   * as double but may be set from float.
   * \sa GetOrigin() */
  itkSetMacro(Origin, PointType);
  virtual void SetOrigin( const double origin[VImageDimension] );
  virtual void SetOrigin( const float origin[VImageDimension] );

  /** Set the direction cosines of the image. The direction cosines
   * are vectors that point from one pixel to the next.
   *
   * One row of the matrix indicates the direction cosines of the unit vector
   * that is parallel to the lines of the image grid corresponding to that
   * dimension. For example, and image with Direction matrix
   *
   *    0.866   0.500
   *   -0.500   0.866
   *
   * has an image grid were the fastest changing index (dimension[0]) walks
   * over a line that in Physical space is oriented parallel to the vector
   * (0.866,0.5). The second fastest changing index (dimension[1]) walks along
   * a line that in Physical space is oriented parallel to the vector
   * (-0.5,0.866)
   *
   * The vectors whose direction cosines are stored in the Direction matrix,
   * are expected to be orthogonal to each other, and they are expected to form
   * a right handed coordinate system, but this is not checked nor enforced in
   * the itk::ImageBase.
   *
   * For details, please see:
   *
   * http://www.itk.org/Wiki/Proposals:Orientation#Some_notes_on_the_DICOM_convention_and_current_ITK_usage
   *
   * \sa GetDirection() */
  virtual void SetDirection( const DirectionType direction );

  /** Get the direction cosines of the image. The direction cosines
   * are vectors that point from one pixel to the next.
   * For ImageBase and Image, the default direction is identity. */
  itkGetConstReferenceMacro(Direction, DirectionType);

  /** Get the spacing (size of a pixel) `of the image. The
   * spacing is the geometric distance between image samples.
   * The value returned is a pointer to a double array.
   * For ImageBase and Image, the default data spacing is unity. */
  itkGetConstReferenceMacro(Spacing, SpacingType);

  /** Get the origin of the image. The origin is the geometric
   * coordinates of the index (0,0).  The value returned is a pointer
   * to a double array.  For ImageBase and Image, the default origin is
   * 0. */
  itkGetConstReferenceMacro(Origin, PointType);

  /** Allocate the image memory. The size of the image must
   * already be set, e.g. by calling SetRegions().
   *
   * This method should be pure virtual, if backwards compatibility
   *  was not required.
   */
  virtual void Allocate() {};

  /** Set the region object that defines the size and starting index
   * for the largest possible region this image could represent.  This
   * is used in determining how much memory would be needed to load an
   * entire dataset.  It is also used to determine boundary
   * conditions.
   * \sa ImageRegion, SetBufferedRegion(), SetRequestedRegion() */
  virtual void SetLargestPossibleRegion(const RegionType &region);

  /** Get the region object that defines the size and starting index
   * for the largest possible region this image could represent.  This
   * is used in determining how much memory would be needed to load an
   * entire dataset.  It is also used to determine boundary
   * conditions.
   * \sa ImageRegion, GetBufferedRegion(), GetRequestedRegion() */
  virtual const RegionType& GetLargestPossibleRegion() const
    { return m_LargestPossibleRegion;};

  /** Set the region object that defines the size and starting index
   * of the region of the image currently loaded in memory.
   * \sa ImageRegion, SetLargestPossibleRegion(), SetRequestedRegion() */
  virtual void SetBufferedRegion(const RegionType &region);

  /** Get the region object that defines the size and starting index
   * of the region of the image currently loaded in memory.
   * \sa ImageRegion, SetLargestPossibleRegion(), SetRequestedRegion() */
  virtual const RegionType& GetBufferedRegion() const
  { return m_BufferedRegion;};

  /** Set the region object that defines the size and starting index
   * for the region of the image requested (i.e., the region of the
   * image to be operated on by a filter). Setting the RequestedRegion
   * does not cause the object to be modified. This method is called
   * internally by the pipeline and therefore bypasses the modified
   * time calculation.
   * \sa ImageRegion, SetLargestPossibleRegion(), SetBufferedRegion() */
  virtual void SetRequestedRegion(const RegionType &region);

  /** Set the requested region from this data object to match the requested
   * region of the data object passed in as a parameter.  This method
   * implements the API from DataObject. The data object parameter must be
   * castable to an ImageBase. Setting the RequestedRegion does not cause
   * the object to be modified. This method is called internally by
   * the pipeline and therefore bypasses the modified time
   * calculation. */
  virtual void SetRequestedRegion(DataObject *data);

  /** Get the region object that defines the size and starting index
   * for the region of the image requested (i.e., the region of the
   * image to be operated on by a filter).
   * \sa ImageRegion, SetLargestPossibleRegion(), SetBufferedRegion() */
  virtual const RegionType& GetRequestedRegion() const
  { return m_RequestedRegion;};

  /** Get the offset table.  The offset table gives increments for
   * moving from one pixel to next in the current row, column, slice,
   * etc..  This table if of size [VImageDimension+1], because its
   * values are computed progressively as: {1, N1, N1*N2,
   * N1*N2*N3,...,(N1*...*Nn)} Where the values {N1,...,Nn} are the
   * elements of the BufferedRegion::Size array.  The last element of
   * the OffsetTable is equivalent to the BufferSize.  Having a
   * [VImageDimension+1] size array, simplifies the implementation of
   * some data accessing algorithms. The entries in the offset table
   * are only valid after the BufferedRegion is set. */
  const OffsetValueType *GetOffsetTable() const { return m_OffsetTable; };

  /** Compute an offset from the beginning of the buffer for a pixel
   * at the specified index. The index is not checked as to whether it
   * is inside the current buffer, so the computed offset could
   * conceivably be outside the buffer. If bounds checking is needed,
   * one can call ImageRegion::IsInside(ind) on the BufferedRegion
   * prior to calling ComputeOffset. */


#ifdef ITK_USE_TEMPLATE_META_PROGRAMMING_LOOP_UNROLLING
  inline OffsetValueType ComputeOffset(const IndexType &ind) const
    {
    OffsetValueType offset = 0;
    ImageHelper<VImageDimension,VImageDimension>::ComputeOffset(this->GetBufferedRegion().GetIndex(),
                                                                ind,
                                                                m_OffsetTable,
                                                                offset);
    return offset;
    }
#else
  OffsetValueType ComputeOffset(const IndexType &ind) const
    {
    // need to add bounds checking for the region/buffer?
    OffsetValueType offset=0;
    const IndexType &bufferedRegionIndex = this->GetBufferedRegion().GetIndex();

    // data is arranged as [][][][slice][row][col]
    // with Index[0] = col, Index[1] = row, Index[2] = slice
    for (int i=VImageDimension-1; i > 0; i--)
      {
      offset += (ind[i] - bufferedRegionIndex[i])*m_OffsetTable[i];
      }
    offset += (ind[0] - bufferedRegionIndex[0]);

    return offset;
    }
#endif
  /** Compute the index of the pixel at a specified offset from the
   * beginning of the buffered region. Bounds checking is not
   * performed. Thus, the computed index could be outside the
   * BufferedRegion. To ensure a valid index, the parameter "offset"
   * should be between 0 and the number of pixels in the
   * BufferedRegion (the latter can be found using
   * ImageRegion::GetNumberOfPixels()). */
#ifdef ITK_USE_TEMPLATE_META_PROGRAMMING_LOOP_UNROLLING
  inline IndexType ComputeIndex(OffsetValueType offset) const
    {
    IndexType index;
    const IndexType &bufferedRegionIndex = this->GetBufferedRegion().GetIndex();
    ImageHelper<VImageDimension,VImageDimension>::ComputeIndex(bufferedRegionIndex,
                                                               offset,
                                                               m_OffsetTable,
                                                               index);
    return index;
    }
#else
  IndexType ComputeIndex(OffsetValueType offset) const
    {
    IndexType index;
    const IndexType &bufferedRegionIndex = this->GetBufferedRegion().GetIndex();

    for (int i=VImageDimension-1; i > 0; i--)
      {
      index[i] = static_cast<IndexValueType>(offset / m_OffsetTable[i]);
      offset -= (index[i] * m_OffsetTable[i]);
      index[i] += bufferedRegionIndex[i];
      }
    index[0] = bufferedRegionIndex[0] + static_cast<IndexValueType>(offset);

    return index;
    }
#endif

  /** Set the spacing (size of a pixel) of the image. The
   * spacing is the geometric distance between image samples.
   * It is stored internally as double, but may be set from
   * float. These methods also pre-compute the Index to Physical
   * point transforms of the image.
   * \sa GetSpacing() */
  virtual void SetSpacing (const SpacingType & spacing);
  virtual void SetSpacing (const double spacing[VImageDimension]);
  virtual void SetSpacing (const float spacing[VImageDimension]);


  /** Get the index (discrete) of a voxel from a physical point.
   * Floating point index results are rounded to integers
   * if ITK_USE_CENTERED_PIXEL_COORDINATES_CONSISTENTLY is on
   * and truncated otherwise.
   * Returns true if the resulting index is within the image, false otherwise
   * \sa Transform */
#ifdef ITK_USE_TEMPLATE_META_PROGRAMMING_LOOP_UNROLLING
  template<class TCoordRep>
  bool TransformPhysicalPointToIndex(
    const Point<TCoordRep, VImageDimension>& point,
    IndexType & index ) const
    {
      ImageTransformHelper<VImageDimension,VImageDimension-1,VImageDimension-1>::TransformPhysicalPointToIndex(
        this->m_PhysicalPointToIndex, this->m_Origin, point, index);

    // Now, check to see if the index is within allowed bounds
    const bool isInside = this->GetLargestPossibleRegion().IsInside( index );
    return isInside;
    }
#else
  template<class TCoordRep>
  bool TransformPhysicalPointToIndex(
            const Point<TCoordRep, VImageDimension>& point,
            IndexType & index                                ) const
    {
    for (unsigned int i = 0; i < VImageDimension; i++)
      {
      TCoordRep sum = NumericTraits<TCoordRep>::Zero;
      for (unsigned int j = 0; j < VImageDimension; j++)
        {
        sum += this->m_PhysicalPointToIndex[i][j] * (point[j] - this->m_Origin[j]);
        }
#ifdef ITK_USE_CENTERED_PIXEL_COORDINATES_CONSISTENTLY
      index[i] = Math::RoundHalfIntegerUp< IndexValueType>( sum );
#else
      index[i] = static_cast< IndexValueType>( sum );
#endif
      }

    // Now, check to see if the index is within allowed bounds
    const bool isInside = this->GetLargestPossibleRegion().IsInside( index );

    return isInside;
    }
#endif

  /** \brief Get the continuous index from a physical point
   *
   * Returns true if the resulting index is within the image, false otherwise.
   * \sa Transform */
  template<class TCoordRep>
  bool TransformPhysicalPointToContinuousIndex(
              const Point<TCoordRep, VImageDimension>& point,
              ContinuousIndex<TCoordRep, VImageDimension>& index   ) const
    {
    Vector<double, VImageDimension> cvector;

    for( unsigned int k = 0; k < VImageDimension; k++ )
      {
      cvector[k] = point[k] - this->m_Origin[k];
      }
    cvector = m_PhysicalPointToIndex * cvector;
    for( unsigned int i = 0; i < VImageDimension; i++ )
      {
      index[i] = static_cast<TCoordRep>(cvector[i]);
      }

    // Now, check to see if the index is within allowed bounds
    const bool isInside = this->GetLargestPossibleRegion().IsInside( index );

    return isInside;
    }


  /** Get a physical point (in the space which
   * the origin and spacing infomation comes from)
   * from a continuous index (in the index space)
   * \sa Transform */
  template<class TCoordRep>
  void TransformContinuousIndexToPhysicalPoint(
            const ContinuousIndex<TCoordRep, VImageDimension>& index,
            Point<TCoordRep, VImageDimension>& point        ) const
    {
    for( unsigned int r=0; r<VImageDimension; r++)
      {
      TCoordRep sum = NumericTraits<TCoordRep>::Zero;
      for( unsigned int c=0; c<VImageDimension; c++ )
        {
        sum += this->m_IndexToPhysicalPoint(r,c) * index[c];
        }
      point[r] = sum + this->m_Origin[r];
      }
    }

  /** Get a physical point (in the space which
   * the origin and spacing infomation comes from)
   * from a discrete index (in the index space)
   *
   * \sa Transform */
#ifdef ITK_USE_TEMPLATE_META_PROGRAMMING_LOOP_UNROLLING
  template<class TCoordRep>
  void TransformIndexToPhysicalPoint(
                      const IndexType & index,
                      Point<TCoordRep, VImageDimension>& point ) const
    {
      ImageTransformHelper<VImageDimension,VImageDimension-1,VImageDimension-1>::TransformIndexToPhysicalPoint(
        this->m_IndexToPhysicalPoint, this->m_Origin, index, point);
    }
#else
  template<class TCoordRep>
  void TransformIndexToPhysicalPoint(
                      const IndexType & index,
                      Point<TCoordRep, VImageDimension>& point ) const
    {
    for (unsigned int i = 0; i < VImageDimension; i++)
      {
      point[i] = this->m_Origin[i];
      for (unsigned int j = 0; j < VImageDimension; j++)
        {
        point[i] += m_IndexToPhysicalPoint[i][j] * index[j];
        }
      }
    }
#endif


  /** Get a physical point (in the space which
   * the origin and spacing infomation comes from)
   * from a discrete index (in the index space)
   *
   * \sa Transform */

  /** Take a vector or covariant vector that has been computed in the
   * coordinate system parallel to the image grid and rotate it by the
   * direction cosines in order to get it in terms of the coordinate system of
   * the image acquisition device.  This implementation in the OrientedImage
   * multiply the array (vector or covariant vector) by the matrix of Direction
   * Cosines. The arguments of the method are of type FixedArray to make
   * possible to use this method with both Vector and CovariantVector.
   * The Method is implemented differently in the itk::Image.
   *
   * \sa Image
   */ 
  template<class TCoordRep>
  void TransformLocalVectorToPhysicalVector(
    const FixedArray<TCoordRep, VImageDimension> & inputGradient,
          FixedArray<TCoordRep, VImageDimension> & outputGradient ) const
    {
    //
    // This temporary implementation should be replaced with Template MetaProgramming.
    // 
#ifdef ITK_USE_ORIENTED_IMAGE_DIRECTION
    const DirectionType & direction = this->GetDirection();
    for (unsigned int i = 0; i < VImageDimension; i++)
      {
      typedef typename NumericTraits<TCoordRep>::AccumulateType CoordSumType;
      CoordSumType sum = NumericTraits<CoordSumType>::Zero;
      for (unsigned int j = 0; j < VImageDimension; j++)
        {
        sum += direction[i][j] * inputGradient[j];
        }
      outputGradient[i] = static_cast<TCoordRep>( sum );
      }
#else
    for (unsigned int i = 0; i < VImageDimension; i++)
      {
      outputGradient[i] = inputGradient[i];
      }
#endif
    }

  /** Copy information from the specified data set.  This method is
   * part of the pipeline execution model. By default, a ProcessObject
   * will copy meta-data from the first input to all of its
   * outputs. See ProcessObject::GenerateOutputInformation().  Each
   * subclass of DataObject is responsible for being able to copy
   * whatever meta-data it needs from from another DataObject.
   * ImageBase has more meta-data than its DataObject.  Thus, it must
   * provide its own version of CopyInformation() in order to copy the
   * LargestPossibleRegion from the input parameter. */
  virtual void CopyInformation(const DataObject *data);

  /** Graft the data and information from one image to another. This
   * is a convenience method to setup a second image with all the meta
   * information of another image and use the same pixel
   * container. Note that this method is different than just using two
   * SmartPointers to the same image since separate DataObjects are
   * still maintained. This method is similar to
   * ImageSource::GraftOutput(). The implementation in ImageBase
   * simply calls CopyInformation() and copies the region ivars.
   * Subclasses of ImageBase are responsible for copying the pixel
   * container. */
  virtual void Graft(const DataObject *data);

  /** Update the information for this DataObject so that it can be used
   * as an output of a ProcessObject.  This method is used the pipeline
   * mechanism to propagate information and initialize the meta data
   * associated with a DataObject. This method calls its source's
   * ProcessObject::UpdateOutputInformation() which determines modified
   * times, LargestPossibleRegions, and any extra meta data like spacing,
   * origin, etc. */
  virtual void UpdateOutputInformation();

  /** Set the RequestedRegion to the LargestPossibleRegion.  This
   * forces a filter to produce all of the output in one execution
   * (i.e. not streaming) on the next call to Update(). */
  virtual void SetRequestedRegionToLargestPossibleRegion();

  /** Determine whether the RequestedRegion is outside of the
   * BufferedRegion. This method returns true if the RequestedRegion
   * is outside the BufferedRegion (true if at least one pixel is
   * outside). This is used by the pipeline mechanism to determine
   * whether a filter needs to re-execute in order to satisfy the
   * current request.  If the current RequestedRegion is already
   * inside the BufferedRegion from the previous execution (and the
   * current filter is up to date), then a given filter does not need
   * to re-execute */
  virtual bool RequestedRegionIsOutsideOfTheBufferedRegion();

  /** Verify that the RequestedRegion is within the
   * LargestPossibleRegion.  If the RequestedRegion is not within the
   * LargestPossibleRegion, then the filter cannot possible satisfy
   * the request. This method returns true if the request can be
   * satisfied and returns fails if the request cannot. This method is
   * used by PropagateRequestedRegion().  PropagateRequestedRegion()
   * throws a InvalidRequestedRegionError exception is the requested
   * region is not within the LargestPossibleRegion. */
  virtual bool VerifyRequestedRegion();

  /** INTERNAL This method is used internally by filters to copy meta-data from
   * the output to the input. Users should not have a need to use this method.
   *
   * Filters that override the ProcessObject's GenerateOutputInformation()
   * should generally have the following line if they want to propagate meta-
   * data for both Image and VectorImage
   * \code
   * outputImage->SetNumberOfComponentsPerPixel(
   *    inputImage->GetNumberOfComponentsPerPixel() )
   * \endcode
   *
   * \sa ImageBase, VectorImage
   *
   * Returns/Sets the number of components in the image. Note that for all
   * images this is 1. Even for Image< RGBPixel< T >, 3 >.
   * This is >= 1 only for time-series images such as itk::VectorImage. */
  virtual unsigned int GetNumberOfComponentsPerPixel() const;
  virtual void SetNumberOfComponentsPerPixel( unsigned int );

protected:
  ImageBase();
  ~ImageBase();
  virtual void PrintSelf(std::ostream& os, Indent indent) const;

  /** Calculate the offsets needed to move from one pixel to the next
   * along a row, column, slice, volume, etc. These offsets are based
   * on the size of the BufferedRegion. This should be called after
   * the BufferedRegion is set. */
  void ComputeOffsetTable();

  /** Compute helper matrices used to transform Index coordinates to
   * PhysicalPoint coordinates and back. This method is virtual and will be
   * overloaded in derived classes in order to provide backward compatibility
   * behavior in classes that did not used to take image orientation into
   * account.  */ 
  virtual void ComputeIndexToPhysicalPointMatrices();

protected:
  /** Origin and spacing of physical coordinates. This variables are
   * protected for efficiency.  They are referenced frequently by
   * inner loop calculations. */
  SpacingType         m_Spacing;
  PointType           m_Origin;
  DirectionType       m_Direction;

  /** Matrices intended to help with the conversion of Index coordinates
   *  to PhysicalPoint coordinates */
  DirectionType       m_IndexToPhysicalPoint;
  DirectionType       m_PhysicalPointToIndex;

  /** Restores the buffered region to it's default state
   *  This method does not call Modify because Initialization is
   *  called by ReleaseData and can not modify the MTime 
   * \sa  ReleaseData, Initialize, SetBufferedRegion */
  virtual void InitializeBufferedRegion(void);

private:
  ImageBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  OffsetValueType  m_OffsetTable[VImageDimension+1];

  RegionType          m_LargestPossibleRegion;
  RegionType          m_RequestedRegion;
  RegionType          m_BufferedRegion;

};

} // end namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_ImageBase(_, EXPORT, x, y) namespace itk { \
  _(1(class EXPORT ImageBase< ITK_TEMPLATE_1 x >)) \
  namespace Templates { typedef ImageBase< ITK_TEMPLATE_1 x > ImageBase##y; } \
  }

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkImageBase+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkImageBase.txx"
#endif

#endif
