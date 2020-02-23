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

//DLL��ʽ������ģʽ��ϵͳϵͳ���ø�DLLʱ����֪��ʽϵͳ��DLL��ʽ�����ں���ģʽ��
//����ֵ:	  0��DLL��������ģʽ 1��DLL����������ģʽ
__declspec(dllexport) int WINAPI RUNMODE()
{
	//��DLL��������ģʽ�����������K��ģʽ���˺�������1����ʱÿִ��һ��K�߶��ǵ��ñ��ӿ�һ�Ρ�
//	return 1;
	return 0;
}

//�������̼۵ľ���,һ����������,��ʾ��������
//���÷���:
//	"STOCKFUNC@MYMACLOSE"(5)

__declspec(dllexport) int WINAPI MYMACLOSE(CALCINFO* pData)
{
	if ( pData->m_pfParam1 &&				//����1��Ч
		 pData->m_nParam1Start<0 &&			//����1Ϊ����
		 pData->m_pfParam2==NULL )			//����һ������
	{
		float fParam = *pData->m_pfParam1;
		int nPeriod = (int)fParam;			//����1
		if(nPeriod>0)
		{
			float fTotal;
			int i, j;
			for ( i = nPeriod-1; i < pData->m_nNumData; i++ )//����nPeriod���ڵľ���,���ݴ�nPeriod-1��ʼ��Ч
			{
				fTotal = 0.0f;
				for ( j = 0; j < nPeriod; j++ )				//�ۼ�
					fTotal += pData->m_pData[i-j].m_fClose;
				pData->m_pResultBuf[i] = fTotal/nPeriod;	//ƽ��
			}
			return nPeriod-1;
		}
	}

	return -1;
}


//�������,2������,����1Ϊ������ߵ�����,����2��ʾ��������
//���÷���:
//	"STOCKFUNC@MYMAVAR"(CLOSE-OPEN,5)

__declspec(dllexport) int WINAPI MYMAVAR(CALCINFO* pData)
{
	if(pData->m_pfParam1 && pData->m_pfParam2 && 	//����1,2��Ч
		pData->m_nParam1Start>=0 &&					//����1Ϊ������
		pData->m_pfParam3==NULL)					//��2������
	{
		const float*  pValue = pData->m_pfParam1;	//����1
		int nFirst = pData->m_nParam1Start;			//��Чֵ��ʼλ
		float fParam = *pData->m_pfParam2;			//����2
		int nPeriod = (int)fParam;	
		
		if( nFirst >= 0 && nPeriod > 0 )
		{
			float fTotal;
			int i, j;
			for ( i = nFirst+nPeriod-1; i < pData->m_nNumData; i++ )
			{
				fTotal = 0.0f;
				for ( j=0; j < nPeriod; j++ )			//�ۼ�
					fTotal += pValue[i-j];
				pData->m_pResultBuf[i] = fTotal/nPeriod;//ƽ��
			}
			return nFirst+nPeriod-1;
		}
	}
	return -1;

/*    //������K��ģʽ���Ĵ���ʽ��ʾ������
	if(pData->m_pfParam1 && pData->m_pfParam2)
	{
		//������K��ģʽ�����ڴ������ݶ�����ֵ�������Ҫһ�������������洫�ݹ����Ĳ�������������.
		//����ֻ�Ǽ���ʾ����÷�������û��ڹ�ʽ��ε����˽ӿڣ��������ظ�ʹ�øñ������¼������
		//���Ҫ�����������������ȫ�־�̬�������ڱ���������ݡ�
		static std::vector<double> arMaData;
		
		//��һ�����ڳ�ʼ������
		if(pData->m_dwBarpos == 0)
		{
			arMaData.clear();
		}
		
		double dbData = *pData->m_pfParam1;

		//��ֹʹ�ý�ˢ���K�ߵķ���ˢ������
		if(arMaData.size() >= pData->m_nNumData)
			arMaData[arMaData.size()-1] = dbData; //�����������һ������
		else
			arMaData.push_back(dbData);

		DWORD dwCyc = (DWORD) *pData->m_pfParam2;
		
		if(pData->m_dwBarpos < dwCyc-1)
		{
			//����δ���������ڣ�������Ч����
			return -1;
		}

		//���ۼ�
		float fAdd = 0;
		for(DWORD i = pData->m_dwBarpos-(dwCyc-1); i <= pData->m_dwBarpos; i++)
		{
			fAdd += arMaData[i];
		}

		*pData->m_pResultBuf = fAdd / dwCyc;
	}
	return 1;*/
}

//���������еľ�ֵ,5������,����1-4Ϊ����������,����5���ھ���˵����ֵ�������÷���ʵ���ڴ���������ò���
/*
���÷���:
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
		 pData->m_pCalcParam[3].m_nParamStart >= 0 )			//4�����ж�����Ч��ֵ
	{
		//���㷵�ص����еĵ�һ����Чֵλ��
		int nFirst = pData->m_pCalcParam[3].m_nParamStart;		//��֪���ص����еĵ�һ����Чֵλ�����4������һ��
//����֪����
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
		int nNum = (int)(pData->m_pCalcParam[4].m_fParam);		//ʵ���ϸ�������֪nNum=4���ڴ�����˵����ֵ�������÷�
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
 ����˵�� �ֱʡ����߶Ρ�ȷ������
 ���÷���:
   "STOCKFUNC@CHZHSHCH(START,END,LOW,HIGH,�߶β㼶���[0-7, 0����ֱ�,1-7�����߶�],���ѡ��[0/1,0��ʾʱ�䣬1��ʾ�۸�])
*/
__declspec(dllexport) int WINAPI CHZHSHCH(CALCINFO* pData)
{
	Class_env* env = Class_env::getInstance(pData);

	// �����Ҫ����̨����Ļ����Ͱ������д�
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);

	return 0; //�������ֵ�����ص��ǽ�������еĵ�һ����Ч��ֵ�±�
}
