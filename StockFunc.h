#ifndef __STOCKFUNC_H_INCLUDE
#define __STOCKFUNC_H_INCLUDE

/*
///////////////////////////////////////////////////////////////////////////
��������C���Խӿڡ���չ������ýӿڹ淶V1.0
1.��չ����������ʵ��ϵͳ��ʽ��������ʵ�ֵ������㷨.
2.��չ������windows 32λ��̬���ӿ�ʵ��,����ʹ��Microsoft Visual C++���.
3.����ʱ�ڹ�ʽ�༭������д"��̬������@��������"(������)����,�������溯������дΪ"STOCKFUNC@MYMACLOSE"(5)
4.��̬���ӿ����ƺͺ������ƿ����Լ�����.
5.ʹ��ʱ���뽫��̬���ļ���������*.stk�ĵ���ͬĿ¼��ʹ��.

�ӽ�������2.34��󣬽ӿ�֧����K��ģʽ���ã�����RUNMODEϵͳ���������ڸ�֪���õĹ�ʽϵͳ���ӿڵ�����ģʽ��
�������Ĺ�ʽϵͳ����ģʽ������ģʽ����K��ģʽ������ģʽģʽ������ʽϵͳ����ʱֻ�����һ�α��ӿڣ����ݺ�
�������в������ݵ��ӿڣ�����K��ģʽ���ڽ���ʱÿ��K�߶����ñ��ӿڣ����ݺͷ���Ҳ���ǵ�ֵ���ݡ�
��ͳ�Ľӿ������������ģʽ�����еģ���Ȼ����K��ģʽ�½�����Ҳ���Ե������д�ͳ�Ľӿڣ���������Ч�ʺܵͲ����ǲ�
�ȶ��ģ���������Ҫ����K��ģʽ��ʹ�ýӿڣ���ʹ�ô���ר��ģʽ��
*/

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

////////////////////////////////////////////////////
//��������
////////////////////////////////////////////////////
enum DATA_TYPE
{
	TICK_DATA=2,				//�ֱʳɽ�
	MIN1_DATA,					//1������
	MIN5_DATA,					//5������					
	MIN15_DATA,					//15������
	MIN30_DATA,					//30������
	MIN60_DATA,					//60������
	DAY_DATA,					//����
	WEEK_DATA,					//����
	MONTH_DATA,					//����
	YEAR_DATA,					//����
	MULTIDAY_DATA,				//������
	MULTIHOUR_DATA,				//��Сʱ��
	MULTISEC_DATA,				//������
	MULTIMIN_DATA,				//�������
	QUARTER_DATA,				//������
	SEMIYEAR_DATA,				//������
	SOLARTERM_DATA,				//������
	MIN3_DATA,					//3������
	MIN10_DATA,					//10������
};

////////////////////////////////////////////////////
//��������
////////////////////////////////////////////////////

typedef struct tagSTKDATA	
{
	DWORD	m_time;			//ʱ��,UCT (time_t)
	float	m_fOpen;		//����
	float	m_fHigh;		//���
	float	m_fLow;			//���
	float	m_fClose;		//����
	float	m_fVolume;		//�ɽ���(��)
	float	m_fAmount;		//�ɽ���(Ԫ)/�ֲ�(δƽ�ֺ�Լ�����ڻ���Ч)
	WORD	m_wAdvance;		//���Ǽ���(��������Ч)
	WORD	m_wDecline;		//�µ�����(��������Ч)
} STKDATA;

////////////////////////////////////////////////////
//��չ����,���������ֱʳɽ����ݵ�������
////////////////////////////////////////////////////

typedef union tagSTKDATAEx
{
	struct
	{
		float m_fBuyPrice[3];		//��1--��3��
		float m_fBuyVol[3];			//��1--��3��
		float m_fSellPrice[3];		//��1--��3��	
		float m_fSellVol[3];		//��1--��3��
		DWORD m_dwToken;			//�ɽ�����
	};
	float m_fDataEx[13];
} STKDATAEx;

//�ֱʳɽ�����
//ע��:�ֱ����ݵ�VOLUME��AMOUNT�ǵ��������ݽṹ
typedef struct{
	WORD m_fBuyVolume;
	WORD m_fSellVolume;	
	
	unsigned m_bOrder : 1;			//�ɽ����� 1���� 0����
	unsigned m_time : 31;			// UCT(time_t)
	
	float	m_fBuyPrice;								// �����1
	float	m_fSellPrice;							// ������1
	
	float	m_fNewPrice;
	float	m_fOI;				//open interest
	float	m_fVolume;
	float	m_fAmount;
}SUBSECTION_REPORT;

////////////////////////////////////////////////////
//��Ȩ����
////////////////////////////////////////////////////

typedef struct tagSPLITDATA	
{
	DWORD	m_time;			//ʱ��,UCT(time_t)
	float	m_fHg;			//���
	float	m_fPg;			//���
	float	m_fPgj;			//��ɼ�
	float	m_fHl;			//����
} SPLITDATA;


////////////////////////////////////////////////////
/*��������˳��(m_pfFinData����)

	���	����

	0	�ܹɱ�(���),
	1	���ҹ�,
	2	�����˷��˹�,
	3	���˹�,
	4	B��,
	5	H��,
	6	��ͨA��,
	7	ְ����,
	8	A2ת���,
	9	���ʲ�(ǧԪ),
	10	�����ʲ�,
	11	�̶��ʲ�,
	12	�����ʲ�,
	13	����Ͷ��,
	14	������ծ,
	15	���ڸ�ծ,
	16	�ʱ�������,
	17	ÿ�ɹ�����,
	18	�ɶ�Ȩ��,
	19	��Ӫ����,
	20	��Ӫ����,
	21	��������,
	22	Ӫҵ����,
	23	Ͷ������,
	24	��������,
	25	Ӫҵ����֧,
	26	�����������,
	27	�����ܶ�,
	28	˰������,
	29	������,
	30	δ��������,
	31	ÿ��δ����,
	32	ÿ������,
	33	ÿ�ɾ��ʲ�,
	34	����ÿ�ɾ���,
	35	�ɶ�Ȩ���,
	36	����������
	37  ��Ӫ�ֽ�����
	38  ��Ӫ�ֽ�����
	39  ��Ӫ�ֽ�����
	40  Ͷ���ֽ�����
	41  Ͷ���ֽ�����
	42  Ͷ���ֽ�����
	43  �����ֽ�����
	44  �����ֽ�����
	45  �����ֽ�����
	46  �ֽ𼰵ȼ���
	47  Ӧ���ʿ���ת��
	48  �����ת��
	49  �ɶ�����
	50  ���м�
	51  �ٶ�����
	52  ��Ӫҵ��������
	53  ˰������������
	54  ���ʲ�������
	55  ���ʲ�������
*/

////////////////////////////////////////////////////
//���ò�����ṹ
////////////////////////////////////////////////////
typedef struct tagCALCPARAM
{
	union 
	{
		float*	m_pfParam;				//���в�����ָ��һ������������	
		float	m_fParam;				//��ֵ����	
	};
	int	m_nParamStart;			//���в�����Ч��ʼλ��
}CALCPARAM;

////////////////////////////////////////////////////
//���ýӿ���Ϣ���ݽṹ
////////////////////////////////////////////////////
typedef struct tagCALCINFO
{
	const DWORD			m_dwSize;				//�ṹ��С
	const DWORD			m_dwVersion;			//��������汾(V2.10 : 0x210)
	const DWORD			m_dwSerial;				//����������к�
	const char*			m_strStkLabel;			//��Ʊ����
	const BOOL			m_bIndex;				//����

	const int			m_nNumData;				//��������(pData,pDataEx,pResultBuf��������)
	const STKDATA*		m_pData;				//��������,ע��:��m_nNumData==0ʱ����Ϊ NULL
	const STKDATAEx*	m_pDataEx;				//��չ����,�ֱʳɽ�������,ע��:����Ϊ NULL

	const int			m_nParam1Start;			//����1��Ч��ʼλ��
	const float*		m_pfParam1;				//���ò���1	
	const float*		m_pfParam2;				//���ò���2
	const float*		m_pfParam3;				//���ò���3
	const float*		m_pfParam4;				//���ò���4

	float*				m_pResultBuf;			//���������,��������ģʽ�������飬������K��ģʽ���ò������ص�������
	const DWORD			m_dataType;				//��������
	const float*		m_pfFinData;			//��������

//����Ϊ��չ�ṹ

	DWORD			    m_dwBarpos;				//��Ϊ��K��ģʽ���ã��˲���Ϊ��K�ߵ��ô��ݹ����Ļ���0��ŵ�K�����

	const int			m_nNumParam;			// ���ò�������
	const CALCPARAM*	m_pCalcParam;			// ���ò�������

	//2.55������,���շֱ�����
	SUBSECTION_REPORT * m_pSubData;		//���շֱʳɽ�����(��DATA_TYPE != TICK_DATAʱ��Ч)
	DWORD		m_dwSubDataCount;		//���շֱʳɽ�����(��DATA_TYPE != TICK_DATAʱ��Ч)
	DWORD		m_dwReservedEx[2];		// ����

	char*				m_strStkName;			//��Ʊ����

	SPLITDATA*			m_pSplitData;			//��Ȩ����
	int					m_nNumSplitData;		//��Ȩ����

} CALCINFO;

/* 
ע1: 
	1.�������ò�����m_pfParam1--m_pfParam4����,��ΪNULL���ʾ�ò�����Ч.
	2.��һ��������Чʱ,���������в�������Ч.
		��:m_pfParam2ΪNULL,��m_pfParam3,m_pfParam4ҲΪNULL.
	3.����1�����ǳ�������������������,�������ֻ��Ϊ��������.
	4.��m_nParam1Start<0, �����1Ϊ��������,��������*m_pfParam1;
	5.��m_nParam1Start>=0,�����1Ϊ����������,m_pfParam1ָ��һ������������,
		�����СΪm_nNumData,������Ч��ΧΪ m_nParam1Start �� m_nNumData-1.
		��ʱ����m_pData[x] �� m_pfParam1[x]��һ�µ�

ע2: 
	1.����չ�ṹʹ���ò����ڼ����Ͽ�����������Ŀ�ģ���ÿ��������Ϊ��ֵ���С�
	2.CALCPARAM�ṹ���ڴ��������Ϣ��ʵ�����ݣ�m_pCalcParam�����СΪm_nNumParam��������Ч��ΧΪ 0 �� m_nNumParam-1.
	3.��������˳��m_pCalcParam[0]Ϊ��һ�����������ݣ�m_pCalcParam[1]Ϊ�ڶ�������������...��Ϊ�˱��ּ��ݣ�ԭm_nParam1Start��m_pfParam1��5��������Ȼ�и�ֵ��
	4.ȡ��m_pCalcParam[i].m_pfParam�������СΪm_nNumData,������Ч��ΧΪm_pCalcParam[i].m_nParamStart �� m_nNumData-1. ��m_pCalcParam[i].m_nParamStart<0, �������������Ч���ݡ�
	5.���ڿ��Ե��ö�����У�������еļ���������ڹ�ʽ�н��У�Ȼ����Ϊ���õĲ������ɡ�

*/


////////////////////////////////////////////////////
/* �������

__declspec(dllexport) int xxxxxxxx(CALCINFO* pData);

1.����������ȫ����д.
2.����������������ʽ����,����ʵ�ʺ����������xxxxxxxx;
	����C++����������� extern "C" {   } ������.
3.������������pData->m_pResultBuf����.
4.��������-1��ʾ�����ȫ��������Ч,��������ģʽ���ص�һ����Чֵλ��,��:
	m_pResultBuf[����ֵ] -- m_pResultBuf[m_nNumData-1]��Ϊ��Чֵ.��K��ģʽ����ֵ���ڵ���0����ʾ�ɹ�

*/

//DLL��ʽ������ģʽ��ϵͳϵͳ���ø�DLLʱ����֪��ʽϵͳ��DLL��ʽ�����ں���ģʽ��
//����ֵ:	  0��DLL��������ģʽ 1��DLL����������ģʽ
__declspec(dllexport) int WINAPI RUNMODE();


//ʾ������,ʹ��ʱ��ʵ�������滻
__declspec(dllexport) int WINAPI MYMACLOSE(CALCINFO* pData);
__declspec(dllexport) int WINAPI MYMAVAR(CALCINFO* pData);
__declspec(dllexport) int WINAPI MYBBI(CALCINFO* pData);

__declspec(dllexport) int WINAPI CHZHSHCH(CALCINFO* pData);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__STOCKFUNC_H_INCLUDE
