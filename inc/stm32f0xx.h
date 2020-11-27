/**
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************i
  */

#ifndef STM32F0xx_H
#define STM32F0xx_H

#if !defined  (STM32F0)
#define STM32F0
#endif

#if !defined (STM32F042x6)
  #define STM32F042x6
#endif

#if !defined  (USE_HAL_DRIVER)
  /*#define USE_HAL_DRIVER */
#endif

#include "stm32f042x6.h"

typedef enum {
    RESET = 0U,
    SET = !RESET
} FlagStatus, ITStatus;

typedef enum {
    DISABLE = 0U,
    ENABLE = !DISABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum {
    SUCCESS = 0U,
    ERROR = !SUCCESS
} ErrorStatus;

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#if defined (USE_HAL_DRIVER)
 #include "stm32f0xx_hal.h"
#endif

#endif
