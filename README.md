# NUC125BSP_USCI_printf_UART_Rx
 NUC125BSP_USCI_printf_UART_Rx

udpate @ 2024/06/28

1. use USCI for printf 

	- PB7 : TX , USCI0_DAT1

	- PB6 : RX , USCI0_DAT0;

2. use UART0 RXD(PB.0) and TXD(PB.1) , to filter UART command


![image](https://github.com/released/NUC125BSP_USCI_printf_UART_Rx/blob/main/log_uart_cmd_parse1.jpg)


![image](https://github.com/released/NUC125BSP_USCI_printf_UART_Rx/blob/main/log_uart_cmd_parse2.jpg)


![image](https://github.com/released/NUC125BSP_USCI_printf_UART_Rx/blob/main/log_uart_cmd_parse3.jpg)


![image](https://github.com/released/NUC125BSP_USCI_printf_UART_Rx/blob/main/log_uart_cmd_parse4.jpg)


