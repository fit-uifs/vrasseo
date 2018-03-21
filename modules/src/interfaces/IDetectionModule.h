// vstup: 1..n videí
// výstup: event, slabý/silný příznak

#pragma once

#include "IProgressUpdater.h"
#include <vtapi/common/exception.h>
#include <vtapi/data/taskparams.h>
#include <vtapi/data/sequence.h>
#include <vtapi/data/interval.h>
#include <vtapi/data/intervaloutput.h>

namespace Modules
{


class IDetectionModule
{
public:
    /**
     * @brief Returns reference to singleton interface implementation
     * @return singleton instance
     */
    static IDetectionModule &instance();

    /**
     * @brief Called ALWAYS before processing
     * @param params parameters of processing task
     * Throw vtapi::RuntimeModuleException on error
     * @throws vtapi::RuntimeModuleException on failed initialization
     */
    virtual void initialize(const ::vtapi::TaskParams & params) = 0;

    /**
     * @brief Called ALWAYS after processing or failed initialization
     */
    virtual void uninitialize() noexcept = 0;

    /**
     * @brief Main event detection function
     * Throw vtapi::RuntimeModuleException on failure
     * Throw vtapi::ModuleUserAbortException on user abort
     * @param video video object
     * @param output object for outputting intervals
     * @param progress object for updating video progress
     * @throws vtapi::RuntimeModuleException on failed processing
     * @throws vtapi::ModuleUserAbortException processing error
     */
    virtual void processVideo(const vtapi::Video & video,
                              ::vtapi::IntervalOutput & output,
                              ::Modules::IProgressUpdater & progress) = 0;

    /**
     * @brief Call to this function should cause currently active processing
     * to throw a vtapi::ModuleUserAbortException
     * It is called during processVideo() function from a different thread (!)
     * It should return ASAP and not wait for processing end
     */
    virtual void stop() noexcept = 0;
};


}
