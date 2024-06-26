#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#include "DummyLibrary.h"

#define ALL_PORTS_CLK (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | \
                       RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | \
                       RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF)

PORT_InitTypeDef PORT_InitStructure;

void Init_All_Ports(void)
{
    RST_CLK_PCLKcmd(ALL_PORTS_CLK, ENABLE);

    PORT_StructInit(&PORT_InitStructure);
    PORT_Init(MDR_PORTA, &PORT_InitStructure);
    PORT_Init(MDR_PORTB, &PORT_InitStructure);
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
    PORT_Init(MDR_PORTD, &PORT_InitStructure);
    PORT_Init(MDR_PORTE, &PORT_InitStructure);
    PORT_Init(MDR_PORTF, &PORT_InitStructure);

    RST_CLK_PCLKcmd(ALL_PORTS_CLK, DISABLE);
}

int main(void)
{
    int dummy_variable = DummyFunction();

    Init_All_Ports();

    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);

    PORT_InitStructure.PORT_Pin = (PORT_Pin_2 | PORT_Pin_4);
    PORT_InitStructure.PORT_OE = PORT_OE_OUT;
    PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
    PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;

    PORT_Init(MDR_PORTA, &PORT_InitStructure);

    while (1)
    {
        PORT_SetBits(MDR_PORTA, PORT_Pin_2);
        PORT_SetBits(MDR_PORTA, PORT_Pin_4);

        for (int i = 0; i < 8000000; i++)
        {
        }

        PORT_ResetBits(MDR_PORTA, PORT_Pin_2);
        PORT_ResetBits(MDR_PORTA, PORT_Pin_4);

        for (int i = 0; i < 8000000; i++)
        {
        }
    }
}
