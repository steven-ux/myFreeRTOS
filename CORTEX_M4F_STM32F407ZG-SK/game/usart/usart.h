#ifndef USART_H
#define USART_H

/* sample
    int main(void)
    {
        RCC_Configuration();
        GPIO_Configuration();
        USART1_Configuration();

        USART1_puts("Hello World!\r\n");
        USART1_puts("Just for STM32F429I Discovery verify USART1 with USB TTL Cable\r\n");
        while(1)
        {
            while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
            char t = USART_ReceiveData(USART1);
            if ((t == '\r')) {
                while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
                USART_SendData(USART1, t);
                t = '\n';
            }
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
            USART_SendData(USART1, t);
        }

        while(1); // Don't want to exit
    } 
*/

extern void RCC_Configuration(void);

extern void GPIO_Configuration(void);

extern void USART1_Configuration(void);

extern void USART1_puts(char* s);

#endif
