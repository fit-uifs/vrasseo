/*
 * Activity module include file.
 * by: Lukas Klicnar (iklicnar[at]fit.vutbr.cz)
 */

#pragma once

#include <string>
#include <vtapi/common/exception.h>

namespace Modules
{
namespace Activity
{


static const std::string def_param_keyfreq = "keyframe_freq";
static const std::string def_param_min_width = "min_width";
static const std::string def_param_max_width = "max_width";
static const std::string def_param_min_height = "min_height";
static const std::string def_param_max_height = "max_height";

static const std::string def_key_event = "event";


struct DetectionParams
{
    int keyframeFreq;
    double minWidth;
    double minHeight;
    double maxWidth;
    double maxHeight;
};

class ActivityModuleException : public vtapi::RuntimeModuleException
{
public:
    explicit ActivityModuleException(const std::string& error)
        : vtapi::RuntimeModuleException("Activity: " + error) {}
    virtual ~ActivityModuleException() noexcept {}
};


}}

