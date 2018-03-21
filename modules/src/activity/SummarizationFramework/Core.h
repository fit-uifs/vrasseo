#pragma once
#ifndef CORE_H
#define CORE_H

#include "CommonTypes.h"
#include "Exceptions.h"

#include "Model/Scenario.h"
#include "Model/Tube.h"
#include "Model/TubeCollection.h"
#include "Model/TubeFrame.h"
#include "Model/TubeFrameData.h"
#include "Model/IVideoSource.h"
#include "Model/BufferedVideoSource.h"
#include "Model/VideoSequence.h"
#include "Model/NullVideoSequence.h"

#include "Optimization/ScenarioSolver/IScenarioSolver.h"
#include "Optimization/ScenarioSolver/BaseSynopsisScenarioSolver.h"
#include "Optimization/ScenarioSolver/FixedLengthSynopsisScenarioSolver.h"
#include "Optimization/ScenarioSolver/RandomSynopsisScenarioSolver.h"
#include "Optimization/ScenarioSolver/StaticScenarioSolver.h"
#include "Optimization/BackgroundSolver/IBackgroundSolver.h"
#include "Optimization/BackgroundSolver/LinearMogBackgroundSolver.h"

#include "Presentation/FrameWindow.h"
#include "Presentation/IRenderer.h"
#include "Presentation/StaticRenderer.h"
#include "Presentation/SynopsisRenderer.h"
#include "Presentation/TubesPlayer.h"

#include "Persistence/IPersistable.h"
#include "Persistence/IDataSource.h"
#include "Persistence/PreloadedDataSource.h"
#include "Persistence/FileDataSource/Core.h"
#include "Persistence/V100DataSource/Core.h"

#include "ImageProcessing/FrameFilters/FrameFilter.h"
#include "ImageProcessing/FrameFilters/FrameBlender.h"
#include "ImageProcessing/FrameFilters/SimpleFrameBlender.h"
#include "ImageProcessing/FrameFilters/SeamlessFrameBlender.h"
#include "ImageProcessing/FrameFilters/FullFrameBlender.h"
#include "ImageProcessing/VideoFilters/IntervalVideoFilter.h"
#include "ImageProcessing/VideoFilters/MedianVideoFilter.h"
#include "ImageProcessing/VideoFilters/ResamplingVideoFilter.h"
#include "ImageProcessing/VideoFilters/MogBackgroundExtractorVideoFilter.h"
#include "ImageProcessing/VideoFilters/TimelapseVideoFilter.h"
#include "ImageProcessing/TubesExtraction/ITubesExtractor.h"
#include "ImageProcessing/TubesExtraction/MogTubesExtractor.h"
#include "ImageProcessing/TubesExtraction/IOutputHandler.h"
#include "ImageProcessing/TubesExtraction/CollectionOutputHandler.h"
#include "ImageProcessing/seamless_cloning.hpp"

#include "Helpers/Strings.h"
#include "Helpers/Geometry.h"
#include "Helpers/ScenarioIndexing/IScenarioIndexer.h"
#include "Helpers/ScenarioIndexing/SimpleScenarioIndexer.h"

#include "TubeFeatures/Features/EmptyFeature.h"
#include "TubeFeatures/Features/RandomFeature.h"
#include "TubeFeatures/Features/SimpleSubtractionFeature.h"
#include "TubeFeatures/Features/GradientFeature.h"
#include "TubeFeatures/Features/ITubeFeature.h"
#include "TubeFeatures/Features/ITubeFeatureExtractor.h"
#include "TubeFeatures/Processors/IProcessor.h"
#include "TubeFeatures/Processors/TubeProcessor.h"
#include "TubeFeatures/Processors/VideoSequenceProcessor.h"

#endif // CORE_H
