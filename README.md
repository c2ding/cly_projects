# 项目说明

## 结构组成
>该项目涵盖以下几个子项目：**A_CARD**、**A_Box**、**B_Card**、**C_Slot**、**Dog**。
>
>
>
>| 序号 |     名称     |       备注       |
>| :--: | :----------: | :--------------: |
>|  1   | ***A_Card*** |       卡式       |
>|  2   | ***A_Box***  | 铝制型材外壳项目 |
>|  3   | ***B_Card*** |  大尺寸电路板卡  |
>|  4   | ***C_Slot*** |  7个插槽的项目   |
>|  5   |  ***Dog***   |    看门狗项目    |

## 说明
> 1. 当前 ***仓库***  只含有 ***Dog*** 看门狗项目的PCB设计文档、说明文档等内容，最早版本是2020-01-13，今天加入了看门狗用STM32芯片的刷写方法。2020-02-26
>
> 2. 刷写器的连接方式
>
> | 线序 | 板卡上     | 白色刷写器 |          |
> | ---- | ---------- | ---------- | -------- |
> | 标识 | 七针接口   | 四针接口   | 五针接口 |
> | 1    | NET_JTMS_1 | NULL       | RST      |
> | 2    | PA9        | NULL       | NULL     |
> | 3    | PA10       | NULL       | NULL     |
> | 4    | JTMS_SWDIO | SWIO       | SWIO     |
> | 5    | GND        | GND        | GND      |
> | 6    | VCC_3.3V   | 3.3V       | 3.3V     |
> | 7    | JTMS_SWCLK | CLK        | CLK      |
>
> 3. 看门狗工作命令
>
>    ~~~c
>    "StartOn" 		//激活看门狗功能；
>    "Hearbeat" 		//心跳功能，喂狗功能；
>    "StartOff"		//关闭看门狗功能；
>    "“SW_time"		//开关间隔参数设置；
>    "HB_time"		//心跳间隔参数设置。
>    
>    ~~~
>
>    ### 增加了如何使用Keil uVision编译并刷写固件

> 1. 当前 ***仓库***  只含有 ***Dog*** 看门购项目的PCB设计文档、说明文档等内容，最早版本是2020-01-13，今天加入了看门狗用STM32芯片的刷写方法。2020-02-26


## 附属文件

