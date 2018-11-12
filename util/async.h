#pragma once
#include "util/callback.h"
#include <circle/sched/task.h>

namespace hfh3
{
    

    /** A simple wrapper around circle's CTask for ad-hoc one-shot asynchronous 
      * tasks.
      */

    class Async : public CTask
    {
    public:
        using VoidFunction = Callback<void()>;

        template<typename F>
        Async(F&& inFunction)
            : function(inFunction)
        {

        }

        virtual void Run() override
        {
            function();
        }
    private:
        VoidFunction function;
    };
}