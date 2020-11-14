/**
* \class CChannel
*
* \brief Declaration of CChannel interface.
* \date	October 2020
*
* \authors 3DI-DIANA Research Group (University of Malaga), in alphabetical order: M. Cuevas-Rodriguez, D. Gonzalez-Toledo, L. Molina-Tanco ||
* Coordinated by , A. Reyes-Lecuona (University of Malaga) and L.Picinali (Imperial College London) ||
* \b Contact: areyes@uma.es and l.picinali@imperial.ac.uk
*
* \b Contributions: (additional authors/contributors can be added here)
*
* \b Project: 3DTI (3D-games for TUNing and lEarnINg about hearing aids) ||
* \b Website: http://3d-tune-in.eu/
*
* \b Copyright: University of Malaga and Imperial College London - 2018
*
* \b Licence: This copy of 3dti_AudioToolkit is licensed to you under the terms described in the 3DTI_AUDIOTOOLKIT_LICENSE file included in this distribution.
*
* \b Acknowledgement: This project has received funding from the European Union's Horizon 2020 research and innovation programme under grant agreement No 644051
*/

#ifndef _CCHANNEL_H_
#define _CCHANNEL_H_

#include <Common/Buffer.h>
#include <boost/circular_buffer.hpp>

namespace Common {
	class CChannel
	{
		public: 

			/** \brief Add frame to channel buffer
			*/
			void PushBack(CMonoBuffer<float> & _buffer);

		    /** \brief Get next frame from channel buffer
			*/
			CMonoBuffer<float> PopFront() const;

			/** \brief Set Delay directly in samples 
			*/
			void SetDelayInSamples(int frames);

		private: 
			CMonoBuffer<float> buffer; 
			boost::circular_buffer<float> circular_buffer; 
			boost::circular_buffer<float>::size_type iUnread;
	};
}

#endif
