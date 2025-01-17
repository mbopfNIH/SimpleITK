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

#if defined(_MSC_VER) && _MSC_VER == 1700
// VS11 (Visual Studio 2012) has limited variadic argument support for
// std::bind, the following increases the number of supported
// arguments.
// https://connect.microsoft.com/VisualStudio/feedback/details/723448/very-few-function-arguments-for-std-bind

 #if defined(_VARIADIC_MAX) && _VARIADIC_MAX < 10
  #error "_VARIADIC_MAX already defined. Some STL classes may have insufficient number of template parameters."
 #else
  #define _VARIADIC_MAX 10
 #endif
#endif


#include "sitkImage.hxx"
#include "sitkMemberFunctionFactory.h"

namespace itk
{
  namespace simple
  {

  /** An addressor of AllocateInternal to be utilized with
   * registering member functions with the factory.
   */
  struct AllocateMemberFunctionAddressor
  {
    template <typename TImageType>
    auto
    operator()() const
    {
      return &Image::template AllocateInternal<TImageType>;
    }
  };

  struct DispatchedInternalInitialiationAddressor
  {
    template <typename TImageType>
    auto
    operator()() const
    {
      return &Image::template DispatchedInternalInitialization<TImageType>;
    }
  };

  void
  Image::InternalInitialization(PixelIDValueType type, unsigned int dimension, itk::DataObject * image)
  {
    using PixelIDTypeList = AllPixelIDTypeList;
    using Addressor = DispatchedInternalInitialiationAddressor;


    typedef PimpleImageBase * ( Self::*MemberFunctionType )( itk::DataObject * );

    detail::MemberFunctionFactory<MemberFunctionType> memberFactory(this);

    memberFactory.RegisterMemberFunctions<PixelIDTypeList, 2, SITK_MAX_DIMENSION, Addressor>();

    this->m_PimpleImage.reset(memberFactory.GetMemberFunction(type, dimension)(image));
  }

  void Image::Allocate ( const std::vector<unsigned int> &_size, PixelIDValueEnum ValueEnum, unsigned int numberOfComponents )
    {
      // initialize member function factory for allocating images

      // The pixel IDs supported
      using PixelIDTypeList = AllPixelIDTypeList;

      typedef void ( Self::*MemberFunctionType )( const std::vector<unsigned int> &, unsigned int );

      using AllocateAddressor = AllocateMemberFunctionAddressor;

      detail::MemberFunctionFactory< MemberFunctionType > allocateMemberFactory( this );
      allocateMemberFactory.RegisterMemberFunctions< PixelIDTypeList, 2, SITK_MAX_DIMENSION, AllocateAddressor > ();

      if ( ValueEnum == sitkUnknown )
        {
        sitkExceptionMacro( "Unable to construct image of unsupported pixel type" );
        }

      if (_size.size() < 2 || _size.size() > SITK_MAX_DIMENSION)
        {
        sitkExceptionMacro("Unsupported number of dimensions specified by size: " << _size << "!\n"
                           << "The maximum supported Image dimension is " << SITK_MAX_DIMENSION << "." );
        }

      allocateMemberFactory.GetMemberFunction( ValueEnum, _size.size() )( _size, numberOfComponents );
    }
  }
}
