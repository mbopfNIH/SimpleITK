/*=========================================================================
*
*  Copyright NumFOCUS
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
#ifndef sitkExtractImageFilter_h
#define sitkExtractImageFilter_h

#include <memory>

#include "sitkBasicFilters.h"
#include "sitkImageFilter.h"

namespace itk {
  namespace simple {

    /**\class ExtractImageFilter
\brief Decrease the image size by cropping the image to the selected region bounds.

ExtractImageFilter changes the image boundary of an image by removing pixels outside the target region. The target region must be specified.

ExtractImageFilter also collapses dimensions so that the input image may have more dimensions than the output image (i.e. 4-D input image to a 3-D output image). To specify what dimensions to collapse, the ExtractionRegion must be specified. For any dimension dim where ExtractionRegion.Size[dim] = 0, that dimension is collapsed. The index to collapse on is specified by ExtractionRegion.Index[dim]. For example, we have a image 4D = a 4x4x4x4 image, and we want to get a 3D image, 3D = a 4x4x4 image, specified as [x,y,z,2] from 4D (i.e. the 3rd "time" slice from 4D). The ExtractionRegion.Size = [4,4,4,0] and ExtractionRegion.Index = [0,0,0,2].

The number of dimension in ExtractionRegion.Size and Index must = InputImageDimension. The number of non-zero dimensions in ExtractionRegion.Size must = OutputImageDimension.

The output image produced by this filter will have the same origin as the input image, while the ImageRegion of the output image will start at the starting index value provided in the ExtractRegion parameter. If you are looking for a filter that will re-compute the origin of the output image, and provide an output image region whose index is set to zeros, then you may want to use the RegionOfInterestImageFilter . The output spacing is is simply the collapsed version of the input spacing.

Determining the direction of the collapsed output image from an larger dimensional input space is an ill defined problem in general. It is required that the application developer select the desired transformation strategy for collapsing direction cosines. It is REQUIRED that a strategy be explicitly requested (i.e. there is no working default). Direction Collapsing Strategies: 1) DirectionCollapseToUnknown(); This is the default and the filter can not run when this is set. The reason is to explicitly force the application developer to define their desired behavior. 1) DirectionCollapseToIdentity(); Output has identity direction no matter what 2) DirectionCollapseToSubmatrix(); Output direction is the sub-matrix if it is positive definite, else throw an exception.

This filter is implemented as a multithreaded filter. It provides a DynamicThreadedGenerateData() method for its implementation.

\note This filter is derived from InPlaceImageFilter . When the input to this filter matched the output requested region, like with streaming filter for input, then setting this filter to run in-place will result in no copying of the bulk pixel data.


\see CropImageFilter
\sa itk::simple::Extract for the procedural interface
\sa itk::ExtractImageFilter<InputImageType, typename InputImageType::template Rebind for the Doxygen on the original ITK class.
     */
    class SITKBasicFilters_EXPORT ExtractImageFilter : public ImageFilter {
    public:
      using Self = ExtractImageFilter;

      /** Destructor */
      virtual ~ExtractImageFilter();

      /** Default Constructor that takes no arguments and initializes
       * default parameters */
      ExtractImageFilter();

      /** Define the pixels types supported by this filter */
      using PixelIDTypeList = NonLabelPixelIDTypeList;
\

      /**
       */
      SITK_RETURN_SELF_TYPE_HEADER SetSize ( std::vector<unsigned int> Size ) { this->m_Size = std::move(Size); return *this; }

      /**
       */
      std::vector<unsigned int> GetSize() const { return this->m_Size; }\

      /**
       * 	odo the internal setting of the method needs work!!!
       */
      SITK_RETURN_SELF_TYPE_HEADER SetIndex ( std::vector<int> Index ) { this->m_Index = std::move(Index); return *this; }

      /**
       */
      std::vector<int> GetIndex() const { return this->m_Index; }

      typedef enum {DIRECTIONCOLLAPSETOUNKOWN,DIRECTIONCOLLAPSETOIDENTITY,DIRECTIONCOLLAPSETOSUBMATRIX,DIRECTIONCOLLAPSETOGUESS} DirectionCollapseToStrategyType;\

      /**
       * Backwards compatibility for enum values Set the strategy to be used to collapse physical space dimensions.

itk::itkExtractImageFilter::DIRECTIONCOLLAPSETOIDENTITY Set the strategy so that all collapsed images have an identity direction. Use this strategy when you know that retention of the physical space orientation of the collapsed image is not important.

itk::itkExtractImageFilter::DIRECTIONCOLLAPSETOGUESS Set the strategy so that all collapsed images where output direction is the sub-matrix if it is positive definite, else return identity. This is backwards compatible with ITKv3, but is highly discouraged because the results are difficult to anticipate under differing data scenarios.

itk::itkExtractImageFilter::DIRECTIONCOLLAPSETOSUBMATRIX Set the strategy so that all collapsed images where output direction is the sub-matrix if it is positive definite, else throw an exception. Use this strategy when it is known that properly identified physical space sub-volumes can be reliably extracted from a higher dimensional space. For example when the application programmer knows that a 4D image is 3D+time, and that the 3D sub-space is properly defined.
       */
      SITK_RETURN_SELF_TYPE_HEADER SetDirectionCollapseToStrategy ( DirectionCollapseToStrategyType DirectionCollapseToStrategy ) { this->m_DirectionCollapseToStrategy = DirectionCollapseToStrategy; return *this; }

      /**
       * NOTE: The SetDirectionCollapseToUknown is explicitly not defined. It is a state that a filter can be in only when it is first instantiate prior to being initialized. Get the currently set strategy for collapsing directions of physical space.
       */
      DirectionCollapseToStrategyType GetDirectionCollapseToStrategy() const { return this->m_DirectionCollapseToStrategy; }

      /** Name of this class */
      std::string GetName() const { return std::string ("ExtractImageFilter"); }

      /** Print ourselves out */
      std::string ToString() const;


      /** Execute the filter on the input image */
#ifndef SWIG
      Image Execute ( Image&& image1 );
#endif
      Image Execute ( const Image& image1 );

    private:

      /** Setup for member function dispatching */

      using MemberFunctionType = Image (Self::*)( const Image& image1 );
      template <class TImageType> Image ExecuteInternal ( const Image& image1 );

      friend struct detail::MemberFunctionAddressor<MemberFunctionType>;

      std::unique_ptr<detail::MemberFunctionFactory<MemberFunctionType> > m_MemberFactory;


      template <class TImageType, unsigned int OutputDimension>
        Image ExecuteInternal ( const TImageType *image1, unsigned int outputDimension, std::integral_constant<unsigned int, OutputDimension> meta );
      template <class TImageType>
        Image ExecuteInternal ( const TImageType *image1, unsigned int outputDimension, std::integral_constant<unsigned int, 1> meta );


      std::vector<unsigned int>  m_Size{std::vector<unsigned int>(SITK_MAX_DIMENSION, 1)};

      std::vector<int>  m_Index{std::vector<int>(4, 0)};

      DirectionCollapseToStrategyType  m_DirectionCollapseToStrategy{itk::simple::ExtractImageFilter::DIRECTIONCOLLAPSETOGUESS};


      bool m_InPlace{false};
    };

    /**\
     * \brief Decrease the image size by cropping the image to the selected region bounds.
     *
     * This function directly calls the execute method of ExtractImageFilter
     * in order to support a procedural API
     *
     * \sa itk::simple::ExtractImageFilter for the object oriented interface
     * @{
     */
#ifndef SWIG
     SITKBasicFilters_EXPORT Image Extract ( Image&& image1, std::vector<unsigned int> size = std::vector<unsigned int>(SITK_MAX_DIMENSION, 1), std::vector<int> index = std::vector<int>(4, 0), ExtractImageFilter::DirectionCollapseToStrategyType directionCollapseToStrategy = itk::simple::ExtractImageFilter::DIRECTIONCOLLAPSETOGUESS );
#endif
     SITKBasicFilters_EXPORT Image Extract ( const Image& image1, std::vector<unsigned int> size = std::vector<unsigned int>(SITK_MAX_DIMENSION, 1), std::vector<int> index = std::vector<int>(4, 0), ExtractImageFilter::DirectionCollapseToStrategyType directionCollapseToStrategy = itk::simple::ExtractImageFilter::DIRECTIONCOLLAPSETOGUESS );

     /** @} */
  }
}
#endif
