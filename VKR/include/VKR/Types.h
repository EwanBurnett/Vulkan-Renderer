#ifndef __VKRENDERER_TYPES_H
#define __VKRENDERER_TYPES_H
/**
*   @file Types.h
*   @brief A Collection of type abstractions for use within the Renderer
*   @author Ewan Burnett (EwanBurnettSK@Outlook.com)
*   @date 2024/04/23
*/

namespace VKR {
    
    /**
     * @brief Abstracts the status of a function call. 
    */
    enum Status {
        FAILED = 0x00,
        SUCCESS = 0x01, 
        NOT_IMPLEMENTED = 0xff 
    };


}


#endif