#ifndef __ALGORITHM_METRIC_GRADIENT_H__
#define __ALGORITHM_METRIC_GRADIENT_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmMetricGradient : public AbstractAlgorithm
    {
        AlgorithmMetricGradient();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmMetricGradient(ProgressObject* myProgObj,
                                SurfaceFile* mySurf,
                                const MetricFile* myMetricIn,
                                MetricFile* myMetricOut,
                                MetricFile* myVectorsOut = NULL,
                                const float myPresmooth = -1.0f,
                                const bool myAvgNormals = false,
                                const MetricFile* myRoi = NULL,
                                const int32_t myColumn = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmMetricGradient> AutoAlgorithmMetricGradient;

}

#endif //__ALGORITHM_METRIC_GRADIENT_H__
