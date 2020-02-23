#ifndef __STOCKFUNC_H_INCLUDE
#define __STOCKFUNC_H_INCLUDE

/*
///////////////////////////////////////////////////////////////////////////
金字塔“C语言接口”扩展程序调用接口规范V1.0
1.扩展函数可用于实现系统公式函数不能实现的特殊算法.
2.扩展函数用windows 32位动态链接库实现,建议使用Microsoft Visual C++编程.
3.调用时在公式编辑器中书写"动态库名称@函数名称"(参数表)即可,例如下面函数可以写为"STOCKFUNC@MYMACLOSE"(5)
4.动态链接库名称和函数名称可以自己定义.
5.使用时必须将动态库文件放置在与*.stk文档相同目录下使用.

从金字塔的2.34版后，接口支持逐K线模式调用，增加RUNMODE系统函数，用于告知调用的公式系统本接口的运行模式。
金字塔的公式系统运行模式分序列模式和逐K线模式，序列模式模式整个公式系统解释时只会调用一次本接口，传递和
返回序列参数数据到接口，而逐K线模式会在解释时每个K线都调用本接口，传递和返回也都是单值数据。
传统的接口是设计在序列模式下运行的，虽然在逐K线模式下金字塔也可以调用运行传统的接口，但是这是效率很低并且是不
稳定的，因此如果需要在逐K线模式下使用接口，请使用此种专用模式。
*/

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

////////////////////////////////////////////////////
//分析周期
////////////////////////////////////////////////////
enum DATA_TYPE
{
	TICK_DATA=2,				//分笔成交
	MIN1_DATA,					//1分钟线
	MIN5_DATA,					//5分钟线					
	MIN15_DATA,					//15分钟线
	MIN30_DATA,					//30分钟线
	MIN60_DATA,					//60分钟线
	DAY_DATA,					//日线
	WEEK_DATA,					//周线
	MONTH_DATA,					//月线
	YEAR_DATA,					//年线
	MULTIDAY_DATA,				//多日线
	MULTIHOUR_DATA,				//多小时线
	MULTISEC_DATA,				//多秒线
	MULTIMIN_DATA,				//多分钟线
	QUARTER_DATA,				//季度线
	SEMIYEAR_DATA,				//半年线
	SOLARTERM_DATA,				//节气线
	MIN3_DATA,					//3分钟线
	MIN10_DATA,					//10分钟线
};

////////////////////////////////////////////////////
//基本数据
////////////////////////////////////////////////////

typedef struct tagSTKDATA	
{
	DWORD	m_time;			//时间,UCT (time_t)
	float	m_fOpen;		//开盘
	float	m_fHigh;		//最高
	float	m_fLow;			//最低
	float	m_fClose;		//收盘
	float	m_fVolume;		//成交量(手)
	float	m_fAmount;		//成交额(元)/持仓(未平仓合约，仅期货有效)
	WORD	m_wAdvance;		//上涨家数(仅大盘有效)
	WORD	m_wDecline;		//下跌家数(仅大盘有效)
} STKDATA;

////////////////////////////////////////////////////
//扩展数据,用于描述分笔成交数据的买卖盘
////////////////////////////////////////////////////

typedef union tagSTKDATAEx
{
	struct
	{
		float m_fBuyPrice[3];		//买1--买3价
		float m_fBuyVol[3];			//买1--买3量
		float m_fSellPrice[3];		//卖1--卖3价	
		float m_fSellVol[3];		//卖1--卖3量
		DWORD m_dwToken;			//成交方向
	};
	float m_fDataEx[13];
} STKDATAEx;

//分笔成交数据
//注意:分笔数据的VOLUME和AMOUNT是递增的数据结构
typedef struct{
	WORD m_fBuyVolume;
	WORD m_fSellVolume;	
	
	unsigned m_bOrder : 1;			//成交方向 1买盘 0卖盘
	unsigned m_time : 31;			// UCT(time_t)
	
	float	m_fBuyPrice;								// 申买价1
	float	m_fSellPrice;							// 申卖价1
	
	float	m_fNewPrice;
	float	m_fOI;				//open interest
	float	m_fVolume;
	float	m_fAmount;
}SUBSECTION_REPORT;

////////////////////////////////////////////////////
//除权数据
////////////////////////////////////////////////////

typedef struct tagSPLITDATA	
{
	DWORD	m_time;			//时间,UCT(time_t)
	float	m_fHg;			//红股
	float	m_fPg;			//配股
	float	m_fPgj;			//配股价
	float	m_fHl;			//红利
} SPLITDATA;


////////////////////////////////////////////////////
/*财务数据顺序(m_pfFinData内容)

	序号	内容

	0	总股本(万股),
	1	国家股,
	2	发起人法人股,
	3	法人股,
	4	B股,
	5	H股,
	6	流通A股,
	7	职工股,
	8	A2转配股,
	9	总资产(千元),
	10	流动资产,
	11	固定资产,
	12	无形资产,
	13	长期投资,
	14	流动负债,
	15	长期负债,
	16	资本公积金,
	17	每股公积金,
	18	股东权益,
	19	主营收入,
	20	主营利润,
	21	其他利润,
	22	营业利润,
	23	投资收益,
	24	补贴收入,
	25	营业外收支,
	26	上年损益调整,
	27	利润总额,
	28	税后利润,
	29	净利润,
	30	未分配利润,
	31	每股未分配,
	32	每股收益,
	33	每股净资产,
	34	调整每股净资,
	35	股东权益比,
	36	净资收益率
	37  经营现金流入
	38  经营现金流出
	39  经营现金流量
	40  投资现金流入
	41  投资现金流出
	42  投资现金流量
	43  筹资现金流入
	44  筹资现金流出
	45  筹资现金流量
	46  现金及等价物
	47  应收帐款周转率
	48  存货周转率
	49  股东总数
	50  发行价
	51  速动比率
	52  主营业务增长率
	53  税后利润增长率
	54  净资产增长率
	55  总资产增长率
*/

////////////////////////////////////////////////////
//调用参数项结构
////////////////////////////////////////////////////
typedef struct tagCALCPARAM
{
	union 
	{
		float*	m_pfParam;				//序列参数，指向一个浮点型数组	
		float	m_fParam;				//数值参数	
	};
	int	m_nParamStart;			//序列参数有效起始位置
}CALCPARAM;

////////////////////////////////////////////////////
//调用接口信息数据结构
////////////////////////////////////////////////////
typedef struct tagCALCINFO
{
	const DWORD			m_dwSize;				//结构大小
	const DWORD			m_dwVersion;			//调用软件版本(V2.10 : 0x210)
	const DWORD			m_dwSerial;				//调用软件序列号
	const char*			m_strStkLabel;			//股票代码
	const BOOL			m_bIndex;				//大盘

	const int			m_nNumData;				//数据数量(pData,pDataEx,pResultBuf数据数量)
	const STKDATA*		m_pData;				//常规数据,注意:当m_nNumData==0时可能为 NULL
	const STKDATAEx*	m_pDataEx;				//扩展数据,分笔成交买卖盘,注意:可能为 NULL

	const int			m_nParam1Start;			//参数1有效起始位置
	const float*		m_pfParam1;				//调用参数1	
	const float*		m_pfParam2;				//调用参数2
	const float*		m_pfParam3;				//调用参数3
	const float*		m_pfParam4;				//调用参数4

	float*				m_pResultBuf;			//结果缓冲区,对于序列模式返回数组，对于逐K线模式，该参数返回单个数字
	const DWORD			m_dataType;				//数据类型
	const float*		m_pfFinData;			//财务数据

//以下为扩展结构

	DWORD			    m_dwBarpos;				//若为逐K线模式调用，此参数为逐K线调用传递过来的基于0序号的K线序号

	const int			m_nNumParam;			// 调用参数数量
	const CALCPARAM*	m_pCalcParam;			// 调用参数数组

	//2.55版增加,当日分笔数据
	SUBSECTION_REPORT * m_pSubData;		//当日分笔成交数据(仅DATA_TYPE != TICK_DATA时有效)
	DWORD		m_dwSubDataCount;		//当日分笔成交数量(仅DATA_TYPE != TICK_DATA时有效)
	DWORD		m_dwReservedEx[2];		// 保留

	char*				m_strStkName;			//股票名称

	SPLITDATA*			m_pSplitData;			//除权数据
	int					m_nNumSplitData;		//除权次数

} CALCINFO;

/* 
注1: 
	1.函数调用参数由m_pfParam1--m_pfParam4带入,若为NULL则表示该参数无效.
	2.当一个参数无效时,则其后的所有参数均无效.
		如:m_pfParam2为NULL,则m_pfParam3,m_pfParam4也为NULL.
	3.参数1可以是常数参数或序列数参数,其余参数只能为常数参数.
	4.若m_nParam1Start<0, 则参数1为常数参数,参数等于*m_pfParam1;
	5.若m_nParam1Start>=0,则参数1为序列数参数,m_pfParam1指向一个浮点型数组,
		数组大小为m_nNumData,数据有效范围为 m_nParam1Start 至 m_nNumData-1.
		在时间上m_pData[x] 与 m_pfParam1[x]是一致的

注2: 
	1.该扩展结构使调用参数在技术上可以是无限数目的，且每个参数都为数值序列。
	2.CALCPARAM结构用于带入参数信息和实际数据，m_pCalcParam数组大小为m_nNumParam，数据有效范围为 0 至 m_nNumParam-1.
	3.按参数的顺序，m_pCalcParam[0]为第一个参数的数据，m_pCalcParam[1]为第二个参数的数据...，为了保持兼容，原m_nParam1Start、m_pfParam1等5个属性依然有赋值。
	4.取用m_pCalcParam[i].m_pfParam，数组大小为m_nNumData,数据有效范围为m_pCalcParam[i].m_nParamStart 至 m_nNumData-1. 若m_pCalcParam[i].m_nParamStart<0, 则此数组中无有效数据。
	5.由于可以调用多个序列，许多序列的计算可以先在公式中进行，然后作为调用的参数即可。

*/


////////////////////////////////////////////////////
/* 函数输出

__declspec(dllexport) int xxxxxxxx(CALCINFO* pData);

1.函数名称需全部大写.
2.函数必须以上述形式声明,请用实际函数名称替代xxxxxxxx;
	对于C++程序还需包括在 extern "C" {   } 括号中.
3.函数计算结果用pData->m_pResultBuf带回.
4.函数返回-1表示错误或全部数据无效,对于序列模式返回第一个有效值位置,即:
	m_pResultBuf[返回值] -- m_pResultBuf[m_nNumData-1]间为有效值.逐K线模式返回值大于等于0均表示成功

*/

//DLL公式的运行模式，系统系统调用该DLL时，告知公式系统该DLL公式运行在何种模式下
//返回值:	  0本DLL运行序列模式 1本DLL运行逐周期模式
__declspec(dllexport) int WINAPI RUNMODE();


//示例函数,使用时用实际名称替换
__declspec(dllexport) int WINAPI MYMACLOSE(CALCINFO* pData);
__declspec(dllexport) int WINAPI MYMAVAR(CALCINFO* pData);
__declspec(dllexport) int WINAPI MYBBI(CALCINFO* pData);

__declspec(dllexport) int WINAPI CHZHSHCH(CALCINFO* pData);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__STOCKFUNC_H_INCLUDE
