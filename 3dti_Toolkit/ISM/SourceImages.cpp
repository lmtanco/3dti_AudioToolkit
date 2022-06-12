#include "SourceImages.h"
#include "ISM.h"
#include <cmath>
#include <Common/BiquadFilter.h>

//#ifndef THRESHOLD_BORDER
//#define THRESHOLD_BORDER 0.2f
//#endif

namespace ISM
{
	//SourceImages::SourceImages() {}

	SourceImages::SourceImages(ISM::CISM* _ownerISM) : ownerISM{_ownerISM} {
			
	}

	void SourceImages::setLocation(Common::CVector3 _location, Common::CVector3 listenerLocation)
	{
		//Common::CTransform listenerTransform = ownerISM->GetListener()->GetListenerTransform();
		//Common::CVector3 listenerLocation = listenerTransform.GetPosition();
		sourceLocation = _location;
		updateImages(listenerLocation);
	}

	Common::CVector3 SourceImages::getLocation()
	{
		return sourceLocation;
	}

	std::vector<weak_ptr <SourceImages>> SourceImages::getImages()
	{
		vector<weak_ptr<SourceImages>> result;
		for (auto i = 0; i < images.size(); ++i) {
			result.push_back(weak_ptr<SourceImages>(images[i]));
		}
		return result;
	}

	void SourceImages::getImageLocations(std::vector<Common::CVector3> &imageSourceList)
	{
			for (int i = 0; i < images.size(); i++)
			{
				if (images.at(i)->reflectionWalls.back().isActive())
				{
					imageSourceList.push_back(images.at(i)->getLocation());
					images.at(i)->getImageLocations(imageSourceList);
				}
			}
	}


	void SourceImages::getImageData(std::vector<ImageSourceData> &imageSourceDataList)
	{
		for (int i = 0; i < images.size(); i++)
		{
			ImageSourceData temp;
			temp.location = images.at(i)->getLocation();
			temp.reflectionWalls = images.at(i)->reflectionWalls;
			temp.reflectionBands = images.at(i)->reflectionBands;
			temp.visibility = images.at(i)->visibility;
			temp.visible = images.at(i)->visible;
			imageSourceDataList.push_back(temp);  //Once created, the image source data is added to the list

			images.at(i)->getImageData(imageSourceDataList); //recurse to the next level
		}
	}


	void SourceImages::setReflectionWalls(std::vector<Wall> _reflectionWalls)
	{
		reflectionWalls = _reflectionWalls;
	}

	Wall SourceImages::getReflectionWall()
	{
		return reflectionWalls.back();
	}

	void SourceImages::createImages(Room _room, Common::CVector3 listenerLocation, int reflectionOrder)
	{
		images.clear();		
		createImages(_room, listenerLocation, reflectionOrder, reflectionWalls);		
		updateImages(listenerLocation);
	}

	void SourceImages::createImages(Room _room, Common::CVector3 listenerLocation, int reflectionOrder, std::vector<Wall> reflectionWalls)
	{
		Common::CVector3 roomCenter = ownerISM->getRoom().getCenter();

		if (reflectionOrder > 0) //if the reflection order is already 0 no more images should be created. We are at the leaves of the tree
		{
			reflectionOrder--;
			std::vector<Wall> walls = _room.getWalls();
			for (int i = 0; i < walls.size(); i++)  //for each wall an image is created
			{
				if (walls.at(i).isActive()) //if the wall is not active, its image is not created
				{
					//SourceImages tempSourceImage;
					shared_ptr<SourceImages> tempSourceImage = make_shared< SourceImages>(ownerISM);
					Common::CVector3 tempImageLocation = walls[i].getImagePoint(sourceLocation);

					//Common::CVector3 roomCenter = _room.getCenter();

					// if the image is closer to the listener than the previous original, that reflection is not real and should not be included
					// this is equivalent to determine wether source and listener are on the same side of the wall or not
					if (((listenerLocation - sourceLocation).GetDistance() < (listenerLocation - tempImageLocation).GetDistance()))
					{
						// If the image room where the candidate image source is far from the original room, so that any location in it is closer than
						// the maximum distance to any location in the original room, then the recursive tree has to stop growing
						float roomsDistance = 0.0;
						//if there are no reflection walls, the minimum distance is 0 and it is not necessary to calculate it. This way, we avoud to get
						//wall from the reflectionWalls vector, which is empty.
						if(reflectionWalls.size()>0) 
						{
								roomsDistance = walls.at(i).getMinimumDistanceFromWall(reflectionWalls.front());
						}
						if (roomsDistance <= ownerISM->getMaxDistanceImageSources())
						{
							tempSourceImage->setLocation(tempImageLocation, listenerLocation);
							reflectionWalls.push_back(walls.at(i));
							tempSourceImage->reflectionWalls = reflectionWalls;

							tempSourceImage->FilterBank.RemoveFilters();

							////////////////////// Set up an equalisation filterbank to simulate frequency dependent absortion
							float frec_init = 62.5;                //Frequency of the first band 62.5 Hz !!!!
							float samplingFrec = 44100.0;          //SAMPLING_RATE,  !!!! FIXME

							float bandFrequency = frec_init;       //First band
								  //float filterFrequencyStep = std::pow(2, 1.0f / (bandsPerOctave*filtersPerBand));
								  //float filterFrequency = bandFrequency / ((float)(trunc(filtersPerBand / 2))*filterFrequencyStep);
							float filterFrequencyStep = 2.0;
							float filterFrequency = bandFrequency;
							// Compute Q for all filters
								  //float octaveStep = 1.0f / ((float)filtersPerBand * bandsPerOctave);
							float octaveStepPow = 2.0;
							float Q_BPF = std::sqrt(octaveStepPow) / (octaveStepPow - 1.0f);

							std::vector<float> temp(NUM_BAND_ABSORTION, 1.0);	//creates band reflections and initialise them to 1.0
							tempSourceImage->reflectionBands = temp;

							for (int k = 0; k < NUM_BAND_ABSORTION; k++)
							{
								shared_ptr<Common::CBiquadFilter> filter;
								filter = tempSourceImage->FilterBank.AddFilter();
								//filter->Setup(samplingFrec, filterFrequency, Q_BPF, Common::T_filterType::BANDPASS);
								if (k == 0)
									filter->Setup(samplingFrec, filterFrequency, Q_BPF, Common::T_filterType::LOWPASS);
								else if (k == NUM_BAND_ABSORTION - 1)
									filter->Setup(samplingFrec, filterFrequency, Q_BPF, Common::T_filterType::HIGHPASS);
								else
									filter->Setup(samplingFrec, filterFrequency, Q_BPF, Common::T_filterType::BANDPASS);

								//Set the reflection coefficient of each band according to absortion coeficients of reflectionWalls
								for (int j = 0; j < reflectionWalls.size(); j++)
								{
									tempSourceImage->reflectionBands[k] *= sqrt(1 - reflectionWalls.at(j).getAbsortionB().at(k));
								}
								filter->SetGeneralGain(tempSourceImage->reflectionBands.at(k));	//FIXME: the gain per band is dulicated (inside the filters and  in reflectionBands attribute

								filterFrequency *= filterFrequencyStep;
							}
							/////////////////////////

							if (reflectionOrder > 0)  //Still higher order reflections: we need to create images of the image just created
							{
								// We need to calculate the image room before asking for all the new images
								Room tempRoom;
								for (int j = 0; j < walls.size(); j++)
								{
									Wall tempWall = walls.at(i).getImageWall(walls.at(j));
									tempRoom.insertWall(tempWall);
								}
								tempSourceImage->createImages(tempRoom, listenerLocation, reflectionOrder, reflectionWalls);
							}
							images.push_back(tempSourceImage);
							reflectionWalls.pop_back();
						}
					}
				}
			}
		}
	}

	void SourceImages::updateImages(Common::CVector3 listenerLocation)
	{	
		
		for (int i = 0; i < images.size(); i++)
		{
			images[i]->setLocation(images.at(i)->getReflectionWall().getImagePoint(sourceLocation),listenerLocation);
		}

		//Check visibility through all reflection walls and compute a visibility coeficient
		visibility = 1.0;	//We hypothesise that it is fully visible. Otherwise, this will become lower
		visible = true;

		Common::CVector3 roomCenter = ownerISM->getRoom().getCenter();
	
		float distanceImageToLisener, distAux1;
		for (int i = 0; i < images.size(); i++)
		{
			Common::CVector3 tempImageLocation = images[i]->getLocation();
			distanceImageToLisener = (listenerLocation - tempImageLocation).GetDistance();
			float maxDistanceSourcesToListener = ownerISM->getMaxDistanceImageSources();
			distAux1 = distanceImageToLisener - (maxDistanceSourcesToListener - DIST_MARGIN * 0.5)  ;
			
			if (distanceImageToLisener > (maxDistanceSourcesToListener + DIST_MARGIN * 0.5))
			{
				images[i]->visible = false; 
				images[i]->visibility = 0.0;
			}
			else if (distanceImageToLisener < (maxDistanceSourcesToListener - DIST_MARGIN * 0.5))
			{
				images[i]->visibility = 1.0;
			}
			else
			{
				images[i]->visibility = 0.5 + 0.5 * cos((distanceImageToLisener - distAux1) / DIST_MARGIN);
			}
		}
		
		for (int j = 0; j < reflectionWalls.size(); j++)
		{
			Common::CVector3 reflectionPoint = reflectionWalls.at(j).getIntersectionPointWithLine(sourceLocation, listenerLocation);
			float distanceToBorder, wallVisibility;

			reflectionWalls.at(j).checkPointInsideWall(reflectionPoint, distanceToBorder, wallVisibility);
			visibility *= wallVisibility;
			visible &= (wallVisibility > 0);
		}
		visibility = pow(visibility, (1 / (float)reflectionWalls.size()));
	}

	void SourceImages::processAbsortion(CMonoBuffer<float> inBuffer, std::vector<CMonoBuffer<float>> &imageBuffers, Common::CVector3 listenerLocation)
	{
		for (int i = 0; i < images.size();i++)  //process buffers for each of the image sources, adding the result to the output vector of buffers
		{
			
			CMonoBuffer<float> tempBuffer(inBuffer.size(), 0.0);

			if (images.at(i)->visibility > 0.00001)
			   images.at(i)->FilterBank.Process(inBuffer, tempBuffer);
			imageBuffers.push_back(tempBuffer);
			images.at(i)->processAbsortion(inBuffer, imageBuffers, listenerLocation);
		}
	}


}//namespace ISM
