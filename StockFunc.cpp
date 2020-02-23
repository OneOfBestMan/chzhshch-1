// StockFunc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "include_api.h"
#include "stdio.h"
#include "vector"
#include "base.h"


BOOL WINAPI DllMain( HINSTANCE hModule, 
                       DWORD  ul_reason_for_call, 
                       PVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			if (Class_env::getInstance())
			{
				for (int i = 0; i < Class_ZhongShu::MAX_LEVEL; i++)
				{
					int size = Class_ZhongShu::zsList[i].size();
					Class_ZhongShu::zsList[i].clear();
				}
				FenXianDuan_PostOrderTravel<Class_XianDuan<7>>(true);
				delete Class_env::getInstance();
				Class_env::env = NULL;
			}
			break;
    }
    return TRUE;
}

//DLL公式的运行模式，系统系统调用该DLL时，告知公式系统该DLL公式运行在何种模式下
//返回值:	  0本DLL运行序列模式 1本DLL运行逐周期模式
__declspec(dllexport) int WINAPI RUNMODE()
{
	//本DLL运行序列模式，如果运行逐K线模式将此函数返回1，此时每执行一个K线都是调用本接口一次。
//	return 1;
	return 0;
}

//计算收盘价的均价,一个常数参数,表示计算周期
//调用方法:
//	"STOCKFUNC@MYMACLOSE"(5)

__declspec(dllexport) int WINAPI MYMACLOSE(CALCINFO* pData)
{
	if ( pData->m_pfParam1 &&				//参数1有效
		 pData->m_nParam1Start<0 &&			//参数1为常数
		 pData->m_pfParam2==NULL )			//仅有一个参数
	{
		float fParam = *pData->m_pfParam1;
		int nPeriod = (int)fParam;			//参数1
		if(nPeriod>0)
		{
			float fTotal;
			int i, j;
			for ( i = nPeriod-1; i < pData->m_nNumData; i++ )//计算nPeriod周期的均线,数据从nPeriod-1开始有效
			{
				fTotal = 0.0f;
				for ( j = 0; j < nPeriod; j++ )				//累加
					fTotal += pData->m_pData[i-j].m_fClose;
				pData->m_pResultBuf[i] = fTotal/nPeriod;	//平均
			}
			return nPeriod-1;
		}
	}

	return -1;
}


//计算均价,2个参数,参数1为待求均线的数据,参数2表示计算周期
//调用方法:
//	"STOCKFUNC@MYMAVAR"(CLOSE-OPEN,5)

__declspec(dllexport) int WINAPI MYMAVAR(CALCINFO* pData)
{
	if(pData->m_pfParam1 && pData->m_pfParam2 && 	//参数1,2有效
		pData->m_nParam1Start>=0 &&					//参数1为序列数
		pData->m_pfParam3==NULL)					//有2个参数
	{
		const float*  pValue = pData->m_pfParam1;	//参数1
		int nFirst = pData->m_nParam1Start;			//有效值起始位
		float fParam = *pData->m_pfParam2;			//参数2
		int nPeriod = (int)fParam;	
		
		if( nFirst >= 0 && nPeriod > 0 )
		{
			float fTotal;
			int i, j;
			for ( i = nFirst+nPeriod-1; i < pData->m_nNumData; i++ )
			{
				fTotal = 0.0f;
				for ( j=0; j < nPeriod; j++ )			//累加
					fTotal += pValue[i-j];
				pData->m_pResultBuf[i] = fTotal/nPeriod;//平均
			}
			return nFirst+nPeriod-1;
		}
	}
	return -1;

/*    //对于逐K线模式，的处理方式的示范代码
	if(pData->m_pfParam1 && pData->m_pfParam2)
	{
		//对于逐K线模式，由于传递数据都是数值，因此需要一个数组用来保存传递过来的参数，用来计算.
		//本例只是简单演示这个用法，如果用户在公式多次调用了接口，则会出现重复使用该变量导致计算出错
		//因此要根据情况做几个这种全局静态变量用于保存各种数据。
		static std::vector<double> arMaData;
		
		//第一个周期初始化数据
		if(pData->m_dwBarpos == 0)
		{
			arMaData.clear();
		}
		
		double dbData = *pData->m_pfParam1;

		//防止使用仅刷最后K线的反复刷新问题
		if(arMaData.size() >= pData->m_nNumData)
			arMaData[arMaData.size()-1] = dbData; //仅仅更新最后一个数据
		else
			arMaData.push_back(dbData);

		DWORD dwCyc = (DWORD) *pData->m_pfParam2;
		
		if(pData->m_dwBarpos < dwCyc-1)
		{
			//对于未到计算周期，返回无效数据
			return -1;
		}

		//先累加
		float fAdd = 0;
		for(DWORD i = pData->m_dwBarpos-(dwCyc-1); i <= pData->m_dwBarpos; i++)
		{
			fAdd += arMaData[i];
		}

		*pData->m_pResultBuf = fAdd / dwCyc;
	}
	return 1;*/
}

//计算多个序列的均值,5个参数,参数1-4为待求多个序列,参数5用于举例说明数值参数的用法，实际在此例中无需该参数
/*
调用方法:
	MA1:=MA(CLOSE,3);
	MA2:=MA(CLOSE,6);
	MA3:=MA(CLOSE,12);
	MA4:=MA(CLOSE,24);
	MYBBI: "STOCKFUNC@MYBBI"(MA1, MA2, MA3, MA4, 4);
*/

__declspec(dllexport) int WINAPI MYBBI(CALCINFO* pData)
{
	if ( pData->m_pCalcParam[0].m_nParamStart >= 0 &&
		 pData->m_pCalcParam[1].m_nParamStart >= 0 &&
		 pData->m_pCalcParam[2].m_nParamStart >= 0 &&
		 pData->m_pCalcParam[3].m_nParamStart >= 0 )			//4个序列都含有效数值
	{
		//计算返回的序列的第一个有效值位置
		int nFirst = pData->m_pCalcParam[3].m_nParamStart;		//已知返回的序列的第一个有效值位置与第4个序列一致
//若不知，则
/*
		int nFirst = pData->m_pCalcParam[0].m_nParamStart;
		if ( nFirst < pData->m_pCalcParam[1].m_nParamStart ) 
			nFirst = pData->m_pCalcParam[1].m_nParamStart;
		if ( nFirst < pData->m_pCalcParam[2].m_nParamStart ) 
			nFirst = pData->m_pCalcParam[2].m_nParamStart;
		if ( nFirst < pData->m_pCalcParam[3].m_nParamStart ) 
			nFirst = pData->m_pCalcParam[3].m_nParamStart;
 */

		const float* pValue1 = pData->m_pCalcParam[0].m_pfParam;
		const float* pValue2 = pData->m_pCalcParam[1].m_pfParam;
		const float* pValue3 = pData->m_pCalcParam[2].m_pfParam;
		const float* pValue4 = pData->m_pCalcParam[3].m_pfParam;
		int nNum = (int)(pData->m_pCalcParam[4].m_fParam);		//实际上该例中已知nNum=4，在此用于说明数值参数的用法
		for( int i = nFirst; i < pData->m_nNumData; i++ )
		{
			pData->m_pResultBuf[i] = 
				(pValue1[i] + pValue2[i] + pValue3[i] + pValue4[i])/nNum;
		}
		return nFirst;
	}
	return -1;
}

/*
 缠中说禅 分笔、分线段、确定中枢
 调用方法:
   "STOCKFUNC@CHZHSHCH(START,END,LOW,HIGH,线段层级编号[0-7, 0代表分笔,1-7代表线段],输出选择[0/1,0表示时间，1表示价格])
*/
__declspec(dllexport) int WINAPI CHZHSHCH(CALCINFO* pData)
{
	Class_env* env = Class_env::getInstance(pData);

	// 如果需要控制台输出的话，就把这两行打开
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);

	return 0; //这个返回值，返回的是结果数列中的第一个有效的值下标
}
