#pragma once

namespace Modules
{


/**
 * @brief Progress reporting interface for modules
 * Call any update() method to update progress value for currently processed video
 * Methods are asynchronous and should return ASAP
 */
class IProgressUpdater
{
public:
    /**
     * @brief Updates progress within current video
     * @param progress progress value (range [0.0-1.0])
     */
    virtual void update(double progress) noexcept = 0;

    /**
     * @brief Updates progress within current video
     * @param progress progress value (range [0-100])
     */
    virtual void update(int progress) noexcept = 0;
};


}
