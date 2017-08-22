
#include "bsp.h"



static  OS_TYPE_InterruptEntry  BSP_interrupt_vector_table[BSP_INT_SOURCE_NUMBER];

static  void  BSP_interrupt_handler( uint16  p_interrupt_id);
static  void  BSP_interrupt_handler_dummy( void );

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
void  BSP_interrupt_handler_init( void )
{
    uint16  p_interrupt_id;

    for( p_interrupt_id = 0; p_interrupt_id < BSP_INT_SOURCE_NUMBER; p_interrupt_id++ )
    {
        BSP_interrupt_handler_register( p_interrupt_id, BSP_interrupt_handler_dummy);
    }
}

void  BSP_interrupt_handler_register
( uint16                    p_interrupt_id,
  OS_TYPE_InterruptEntry    p_ISR )
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    if (p_interrupt_id < BSP_INT_SOURCE_NUMBER)
    {
        CPU_ATOMIC_ENTER();
        BSP_interrupt_vector_table[p_interrupt_id] = p_ISR;
        CPU_ATOMIC_EXIT();
    }
}

/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
void  BSP_interrupt_handler_WWDG          (void)  { BSP_interrupt_handler(BSP_INT_ID_WWDG);            }
void  BSP_interrupt_handler_PVD           (void)  { BSP_interrupt_handler(BSP_INT_ID_PVD);             }
void  BSP_interrupt_handler_TAMPER        (void)  { BSP_interrupt_handler(BSP_INT_ID_TAMPER);          }
void  BSP_interrupt_handler_RTC           (void)  { BSP_interrupt_handler(BSP_INT_ID_RTC);             }
void  BSP_interrupt_handler_FLASH         (void)  { BSP_interrupt_handler(BSP_INT_ID_FLASH);           }
void  BSP_interrupt_handler_RCC           (void)  { BSP_interrupt_handler(BSP_INT_ID_RCC);             }
void  BSP_interrupt_handler_EXTI0         (void)  { BSP_interrupt_handler(BSP_INT_ID_EXTI0);           }
void  BSP_interrupt_handler_EXTI1         (void)  { BSP_interrupt_handler(BSP_INT_ID_EXTI1);           }
void  BSP_interrupt_handler_EXTI2         (void)  { BSP_interrupt_handler(BSP_INT_ID_EXTI2);           }
void  BSP_interrupt_handler_EXTI3         (void)  { BSP_interrupt_handler(BSP_INT_ID_EXTI3);           }
void  BSP_interrupt_handler_EXTI4         (void)  { BSP_interrupt_handler(BSP_INT_ID_EXTI4);           }
void  BSP_interrupt_handler_DMA1_CH1      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA1_CH1);        }
void  BSP_interrupt_handler_DMA1_CH2      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA1_CH2);        }
void  BSP_interrupt_handler_DMA1_CH3      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA1_CH3);        }
void  BSP_interrupt_handler_DMA1_CH4      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA1_CH4);        }
void  BSP_interrupt_handler_DMA1_CH5      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA1_CH5);        }
void  BSP_interrupt_handler_DMA1_CH6      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA1_CH6);        }
void  BSP_interrupt_handler_DMA1_CH7      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA1_CH7);        }
void  BSP_interrupt_handler_ADC1_2        (void)  { BSP_interrupt_handler(BSP_INT_ID_ADC1_2);          }
void  BSP_interrupt_handler_CAN1_TX       (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN1_TX);         }
void  BSP_interrupt_handler_CAN1_RX0      (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN1_RX0);        }
void  BSP_interrupt_handler_CAN1_RX1      (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN1_RX1);        }
void  BSP_interrupt_handler_CAN1_SCE      (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN1_SCE);        }
void  BSP_interrupt_handler_EXTI9_5       (void)  { BSP_interrupt_handler(BSP_INT_ID_EXTI9_5);         }
void  BSP_interrupt_handler_TIM1_BRK      (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM1_BRK);        }
void  BSP_interrupt_handler_TIM1_UP       (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM1_UP);         }
void  BSP_interrupt_handler_TIM1_TRG_COM  (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM1_TRG_COM);    }
void  BSP_interrupt_handler_TIM1_CC       (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM1_CC);         }
void  BSP_interrupt_handler_TIM2          (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM2);            }
void  BSP_interrupt_handler_TIM3          (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM3);            }
void  BSP_interrupt_handler_TIM4          (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM4);            }
void  BSP_interrupt_handler_I2C1_EV       (void)  { BSP_interrupt_handler(BSP_INT_ID_I2C1_EV);         }
void  BSP_interrupt_handler_I2C1_ER       (void)  { BSP_interrupt_handler(BSP_INT_ID_I2C1_ER);         }
void  BSP_interrupt_handler_I2C2_EV       (void)  { BSP_interrupt_handler(BSP_INT_ID_I2C2_EV);         }
void  BSP_interrupt_handler_I2C2_ER       (void)  { BSP_interrupt_handler(BSP_INT_ID_I2C2_ER);         }
void  BSP_interrupt_handler_SPI1          (void)  { BSP_interrupt_handler(BSP_INT_ID_SPI1);            }
void  BSP_interrupt_handler_SPI2          (void)  { BSP_interrupt_handler(BSP_INT_ID_SPI2);            }
void  BSP_interrupt_handler_USART1        (void)  { BSP_interrupt_handler(BSP_INT_ID_USART1);          }
void  BSP_interrupt_handler_USART2        (void)  { BSP_interrupt_handler(BSP_INT_ID_USART2);          }
void  BSP_interrupt_handler_USART3        (void)  { BSP_interrupt_handler(BSP_INT_ID_USART3);          }
void  BSP_interrupt_handler_EXTI15_10     (void)  { BSP_interrupt_handler(BSP_INT_ID_EXTI15_10);       }
void  BSP_interrupt_handler_RTCAlarm      (void)  { BSP_interrupt_handler(BSP_INT_ID_RTC_ALARM);       }
void  BSP_interrupt_handler_USBWakeUp     (void)  { BSP_interrupt_handler(BSP_INT_ID_OTG_FS_WKUP);     }
void  BSP_interrupt_handler_TIM5          (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM5);            }
void  BSP_interrupt_handler_SPI3          (void)  { BSP_interrupt_handler(BSP_INT_ID_SPI3);            }
void  BSP_interrupt_handler_USART4        (void)  { BSP_interrupt_handler(BSP_INT_ID_USART4);          }
void  BSP_interrupt_handler_USART5        (void)  { BSP_interrupt_handler(BSP_INT_ID_USART5);          }
void  BSP_interrupt_handler_TIM6          (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM6);            }
void  BSP_interrupt_handler_TIM7          (void)  { BSP_interrupt_handler(BSP_INT_ID_TIM7);            }
void  BSP_interrupt_handler_DMA2_CH1      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA2_CH1);        }
void  BSP_interrupt_handler_DMA2_CH2      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA2_CH2);        }
void  BSP_interrupt_handler_DMA2_CH3      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA2_CH3);        }
void  BSP_interrupt_handler_DMA2_CH4      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA2_CH4);        }
void  BSP_interrupt_handler_DMA2_CH5      (void)  { BSP_interrupt_handler(BSP_INT_ID_DMA2_CH5);        }
void  BSP_interrupt_handler_ETH           (void)  { BSP_interrupt_handler(BSP_INT_ID_ETH);             }
void  BSP_interrupt_handler_ETHWakeup     (void)  { BSP_interrupt_handler(BSP_INT_ID_ETH_WKUP);        }
void  BSP_interrupt_handler_CAN2_TX       (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN2_TX);         }
void  BSP_interrupt_handler_CAN2_RX0      (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN2_RX0);        }
void  BSP_interrupt_handler_CAN2_RX1      (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN2_RX1);        }
void  BSP_interrupt_handler_CAN2_SCE      (void)  { BSP_interrupt_handler(BSP_INT_ID_CAN2_SCE);        }
void  BSP_interrupt_handler_OTG           (void)  { BSP_interrupt_handler(BSP_INT_ID_OTG_FS);          }


/*
************************************************************************************************************************
*
************************************************************************************************************************
*/
static  void  BSP_interrupt_handler( uint16  p_interrupt_id )
{
    CPU_ATOMIC_REGISTER_ALLOCATE();

    OS_TYPE_InterruptEntry  t_ISR;

    if( OS_Status != OS_ENUM_OsStatus_Run )
    {
        return;
    }

    CPU_ATOMIC_ENTER();
    OS_Counter_ISRNest++;
    CPU_ATOMIC_EXIT();

    t_ISR = BSP_interrupt_vector_table[p_interrupt_id];
    if (t_ISR != 0)
    {
        t_ISR();
    }

    CPU_ATOMIC_ENTER();
    OS_Counter_ISRNest--;
    CPU_ATOMIC_EXIT();

    OS_Schedule();
}

static  void  BSP_interrupt_handler_dummy( void )
{

}

