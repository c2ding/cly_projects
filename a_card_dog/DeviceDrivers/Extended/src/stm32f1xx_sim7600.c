/**

作者:刘东辉
  */

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_sim7600.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
//使用的UART端口
#if defined(__STM32F1XX_USART1_H__)
#define gsm_input(a, b) MX_USART1_input(a, b)
#define gsm_output(a, b) MX_USART1_output(a, b)
#define gsm_event usart1_event
#endif
#if defined(__STM32F1XX_USART2_H__)
#define gsm_input(a, b) MX_USART2_input(a, b)
#define gsm_output(a, b) MX_USART2_output(a, b)
#define gsm_event usart2_event
#endif
#if defined(__STM32F1XX_USART3_H__)
#define gsm_input(a, b) MX_USART3_input(a, b)
#define gsm_output(a, b) MX_USART3_output(a, b)
#define gsm_event usart3_event
#endif
#if defined(__STM32F1XX_UART4_H__)
#define gsm_input(a, b) MX_UART4_input(a, b)
#define gsm_output(a, b) MX_UART4_output(a, b)
#define gsm_event uart4_event
#endif
#if defined(__STM32F1XX_UART5_H__)
#define gsm_input(a, b) MX_UART5_input(a, b)
#define gsm_output(a, b) MX_UART5_output(a, b)
#define gsm_event uart5_event
#endif
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
_SIM7600 sim7600;
char sim7600_tx_buffer[1024]; //发送缓冲区
char sim7600_rx_buffer[1024]; //接收缓冲区
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
static void gsm_printf(const char *fmt, ...) //发送数据
{
    va_list ap;
    char string[128];
	uint32_t length;

	va_start(ap, fmt);
	length = vsprintf(string, fmt, ap);
	va_end(ap);
	
	gsm_output((uint8_t*)&string, length);
}
/*----------------------------------------------------------------------------*/
static GSM_res_e waiting_for_data(uint32_t wait_time) //等待数据
{
	gsm_event = NULL;
	do {
		_delay_us(1000);
		if (gsm_event != NULL) {return GSM_SUCCESS;}
	} while (--wait_time);
	return GSM_TIMEOUT;
}
/*----------------------------------------------------------------------------*/
/*
static GSM_res_e GSM_p_command_check(const char *p_command, const char *p_reply, uint32_t wait_time)
{
	char redata[32];
	uint32_t s_length;

	if (p_command != NULL) {gsm_printf(p_command);}
	if (p_reply != NULL)
	{
		for (;;)
		{
			_delay_us(1000);
			if (gsm_event == true) {break;}
			if (--wait_time == false) {return GSM_TIMEOUT;}
		}
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("\r\n%s\r\n", redata);
		if (strstr((char*)&redata, p_reply) == NULL) {return GSM_ERROR;}
	}
	return GSM_SUCCESS;
}
*/
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 等待模块初始
  * 返 回 值: GSM_SUCCESS：成功
  *           GSM_ERROR：失败
  */
GSM_res_e waiting_for_startup(void)
{
	char redata[128];
	uint32_t s_length;

	if (waiting_for_data(1000) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("\r\n%s\r\n", redata);
		if (strstr((char*)&redata, "PB DONE") != NULL) {return GSM_SUCCESS;} else {return GSM_ERROR;}
	}
	return GSM_TIMEOUT;
} 
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: 初始化并检测模块
  * 返 回 值: GSM_SUCCESS：成功
  *           GSM_ERROR：失败
  */
GSM_res_e sim7600_init(void)
{
	char redata[128];
	uint32_t s_length;

	gsm_printf("AT\r\n"); //适应波特率
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("AT:%s", redata);
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("ATE0\r\n"); //不回显字符
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("ATE0:%s", redata);
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	
	gsm_printf("ATQ0\r\n"); //返回结果码
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("ATQ0:%s", redata);
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	#if 0
	gsm_printf("AT+CGMM\r\n"); //厂家认证请求，返回模块厂家信息
	if (waiting_for_data(100) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		//if (strstr((char*)&redata, "SIMCOM_SIM800C") != NULL) {return GSM_SUCCESS;}
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	#endif
	gsm_printf("AT+CPIN?\r\n"); //没有SIM卡
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		if (strstr((char*)&redata, "READY") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+CCID\r\n"); //获取CCID
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
		strncpy(sim7600.ccid, redata+9, 20);
		sim7600.ccid[20] = '\0';
	}

	gsm_printf("AT+CSQ\r\n"); //网络信号质量
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		uint8_t s_strength;
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		if (strstr((char*)&redata, "OK") != NULL)
		{
			s_strength = (redata[8]-'0')*10 + (redata[9]-'0');
			if ((s_strength>31) || (s_strength<4)) {return GSM_ERROR;} //信号强度 不在正常范围
		} else {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+CREG?\r\n"); //查询网络注册
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		if (strstr((char*)&redata, "+CREG: 0,1") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	#if 0
	gsm_printf("AT+CGREG?\r\n"); //查询4G网络注册
	if (waiting_for_data(400) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata); //查询4G网络注册
		if (strstr((char*)&redata, "0,1") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	#endif
//------------------------------------------------------------------------------
	gsm_printf("AT+CGPS=1\r\n"); //启动GPS定位(第二次 返回ERROR)
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CGPS=1:%s", redata);
		//if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+CNETSTART\r\n"); //启动基站定位(+CNETSTART: 0第二次 返回ERROR)
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CNETSTART:%s", redata);
		//if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+CTZU=1\r\n"); //同步网络时间
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CTZU:%s", redata);
		if ((strstr((char*)&redata, "OK") == NULL) || (strstr((char*)&redata, "OK") == NULL)) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	return GSM_SUCCESS;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: sim7600功能：TCP模式初始化
  * 返 回 值: GSM_SUCCESS：成功
  *           GSM_ERROR：失败
  */
GSM_res_e sim7600_tcp_mode(void)
{
	char redata[128];
	uint32_t s_length;

	gsm_printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n"); //设置APN
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CGDCONT:%s", redata);
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+CIPMODE=1\r\n"); //设置TCP/IP模式(第二次 返回ERROR)
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CIPMODE=1:%s", redata);
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+CSOCKSETPN=1\r\n"); //选择TCP/IP应用模式
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CSOCKSETPN=1:%s", redata);
		if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+CIPMODE=0\r\n"); //选择TCP/IP应用模式(第二次 返回ERROR)
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CIPMODE=0:%s", redata);
		//if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+NETOPEN\r\n"); //开启模式(+NETOPEN: 0第二次 返回ERROR)
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("NETOPEN:%s", redata);
		//if (strstr((char*)&redata, "OK") == NULL) {return GSM_ERROR;}
		if (strstr((char*)&redata, "NETOPEN: 0") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	return GSM_SUCCESS;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: sim7600功能：TCP模式初始化
  * 返 回 值: GSM_SUCCESS：成功
  *           GSM_ERROR：失败
  */
GSM_res_e sim7600_close_tcp_mode(void)
{
	char redata[128];
	uint32_t s_length;

	gsm_printf("AT+CIPCLOSE=0\r\n"); //结束 TCP 连接 
	if (waiting_for_data(1000) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("CIPCLOSE:%s", redata);
		//if (strstr((char*)&redata, "+CIPCLOSE: 0,0") != NULL) {return GSM_SUCCESS;}
		//if (strstr((char*)&redata, "ERROR") != NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_printf("AT+NETCLOSE\r\n"); //Close socket
	if (waiting_for_data(1000) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		//if (strstr((char*)&redata, "+NETCLOSE: 0") != NULL) {return GSM_SUCCESS;}
		//if (strstr((char*)&redata, "ERROR") != NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
	return GSM_SUCCESS;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: sim7600功能：连接TCP通信
  * 输入参数: serverip：远端服务器IP地址
  *           serverport：远端服务器端口
  * 返 回 值: GSM_SUCCESS：成功
  *           GSM_ERROR：失败
  */
GSM_res_e sim7600_tcp_link(char* serverip, char* serverport)
{
	char redata[128];
	uint8_t s_freque;
	uint32_t s_length;

	gsm_printf("AT+CIPOPEN=0,\"TCP\",\"%s\",%s\r\n", serverip, serverport); //设置服务器IP和端口(第二次 返回+CIPOPEN: 0,4 ERROR)
	for (s_freque=1; s_freque<3; s_freque++) //尝试2次
	{
		printf("%d", s_freque);
		if (waiting_for_data(1000) == GSM_SUCCESS)
		{
			s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
			//printf("CIPOPEN:%s", redata);
			//if (strstr((char*)&redata, "CIPOPEN: 0,0") != NULL) {sim7600.connect[0] = true; return GSM_SUCCESS;} //连接成功
			if (strstr((char*)&redata, "0,0") != NULL) {sim7600.connect[0] = true; return GSM_SUCCESS;} //连接成功
			else if (strstr((char*)&redata, "OK") != NULL) {;}
			else if (strstr((char*)&redata, "0,4") != NULL) {return GSM_FAIL;} //连接失败
		}
	}
	return GSM_TIMEOUT;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: sim7600功能：数据发送
  * 输入参数: str：数据缓冲区
  * 返 回 值: GSM_TRUE：成功
  *           GSM_FALSE：失败
  */
GSM_res_e sim7600_send(uint32_t p_length)
{
	char redata[128];
	uint8_t s_freque;
	uint32_t s_length;

	gsm_printf("AT+CIPCLOSE?\r\n"); //判断连接
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		//char *str;
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		/*
		str = strstr((char*)&redata, "+CIPCLOSE: "); //
		if (str != NULL)
		{
			if (*(str+12) == '0') {sim7600.connect[0] = false;} 
			if (*(str+14) == '0') {sim7600.connect[1] = false;}
			if (*(str+16) == '0') {sim7600.connect[2] = false;}
			if (*(str+18) == '0') {sim7600.connect[3] = false;}
			if (*(str+20) == '0') {sim7600.connect[4] = false;}
			if (*(str+22) == '0') {sim7600.connect[5] = false;}
			if (*(str+24) == '0') {sim7600.connect[6] = false;}
			if (*(str+26) == '0') {sim7600.connect[7] = false;}
		}
		*/
		if (strstr((char*)&redata, "CIPCLOSE: 0") != NULL) {sim7600.connect[0] = false; return GSM_FAIL;}
		//printf("%d;%d;%d;%d;%d;%d;%d;%d", sim7600.connect[0], sim7600.connect[1], sim7600.connect[2], sim7600.connect[3], sim7600.connect[4], sim7600.connect[5], sim7600.connect[6], sim7600.connect[7]);
	} else {return GSM_TIMEOUT;}

	//if (sim7600.connect[0] == false) {return GSM_FAIL;}

	gsm_printf("AT+CIPSEND=0,%d\r\n", p_length);
	if (waiting_for_data(1000) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		if (strstr((char*)&redata, ">") == NULL) {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}

	gsm_output((uint8_t*)&sim7600_tx_buffer, p_length);
	//gsm_printf("%s\x1A", str); //添加十六进制数:0x1A 信息结束符号
	for (s_freque=1; s_freque<3; s_freque++) //尝试2次
	{
		printf("%d", s_freque);
		if (waiting_for_data(1000) == GSM_SUCCESS)
		{
			s_length = gsm_input((uint8_t *)&sim7600_rx_buffer, 1023), sim7600_rx_buffer[s_length] = '\0', gsm_event = NULL; //接收数据
			//printf("[%s]\r\n", sim7600_rx_buffer);
			if (strstr((char*)&sim7600_rx_buffer, "IPD") != NULL) {gsm_receiving_processing();} //收到报文
			if (strstr((char*)&sim7600_rx_buffer, ": 0,") != NULL) {return GSM_SUCCESS;} //发送成功(+CIPSEND: 0,)
			else if (strstr((char*)&sim7600_rx_buffer, "OK") != NULL) {;} //
			//else if (strstr((char*)&sim7600_rx_buffer, "ERROR") != NULL) {return GSM_ERROR;} //发送失败
		}
	}
	return GSM_TIMEOUT;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: sim7600功能：获取 时间
  * 返 回 值: GSM_SUCCESS：成功
  *           GSM_ERROR：失败
  */
GSM_res_e sim7600_get_time(void)
{
#if 0
	char redata[128];
	uint32_t s_length;
	char *str;
	
	gsm_printf("AT+CCLK?\r\n"); //获取最近一次的网络同步时间
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		str = strstr((char*)&redata, "CCLK: \""); //
		if (str != NULL)
		{
			str += strlen("CCLK: \"");
			sim7600.year = atoi(str), str += 3;//年
			sim7600.month = atoi(str), str += 3; //月
			sim7600.mday = atoi(str), str += 3; //日
			sim7600.hour = atoi(str), str += 3; //时
			sim7600.min = atoi(str), str += 3; //分
			sim7600.sec = atoi(str); //秒
			
			sim7600.year += 2000;
			
			if (sim7600.sec == 0)
			{
				minute_event = 1;
				if (sim7600.min == 0)
				{
					hour_event = 1;
					if (sim7600.hour == 0)
					{
						mday_event = 1;
					}
				}
			}
		} else {return GSM_ERROR;}
	} else {return GSM_TIMEOUT;}
#else
	gsm_printf("AT+CCLK?\r\n"); //获取最近一次的网络同步时间
#endif
	return GSM_SUCCESS;
}
/*----------------------------------------------------------------------------*/
/**
  * 函数功能: GPS功能：获取 位置
  * 返 回 值: GSM_SUCCESS：成功
  *           GSM_ERROR：失败
  */
GSM_res_e sim7600_get_position(void)
{
#if 0
	char redata[128];
	uint32_t s_length;
	char *str;
	double latitude, longitude;

	gsm_printf("AT+CGPSINFO\r\n"); //获取GPS定位
	if (waiting_for_data(200) == GSM_SUCCESS)
	{
		s_length = gsm_input((uint8_t *)&redata, 127), redata[s_length] = '\0', gsm_event = NULL; //接收数据
		//printf("%s", redata);
		if (strstr((char*)&redata, "CGPSINFO: ") != NULL) //GPS正常
		{
			latitude = atof(str + strlen("CGPSINFO: ")); //纬度
			str = strstr((char*)&redata, ",N,");
			longitude = atof(str + strlen(",N,")); //经度
			if ((latitude != 0)&&(longitude != 0))
			{
				sim7600.latitude = latitude, sim7600.longitude = longitude;
			}
		} else {return GSM_ERROR;}
			
		/*if (strstr((char*)&redata, "OK") != NULL) //GPS正常
		{
			str = strstr((char*)&redata, "O: ");
			if (str != NULL) {latitude = atof(str + strlen("O: "));} //纬度
			str = strstr((char*)&redata, ",N,");
			if (str != NULL) {longitude = atof(str + strlen(",N,"));} //经度
			if ((latitude != 0)&&(longitude != 0))
			{
				sim7600.latitude = latitude, sim7600.longitude = longitude;
			}
		} else {return GSM_ERROR;}*/
	} else {return GSM_TIMEOUT;}
#else
	gsm_printf("AT+CGPSINFO\r\n"); //获取GPS定位
#endif
	return GSM_SUCCESS;
}
/*----------------------------------------------------------------------------*/
