#ifndef __FOXFUNC_H_INCLUDE
#define __FOXFUNC_H_INCLUDE

/*
///////////////////////////////////////////////////////////////////////////
�ɺ�����ʦ��C���Խӿڡ���չ������ýӿڹ淶V3.0
1.���淶�����ڷɺ�����ʦV3.x��ʽϵͳ.
2.��չ����������ʵ��ϵͳ��ʽ��������ʵ�ֵ������㷨.
3.��չ������windows 32λ��̬���ӿ�ʵ��,����ʹ��Microsoft Visual C++���.
4.����ʱ�ڹ�ʽ�༭������д"��̬������@��������"(������)����,�������溯������дΪ"FOXFUNC@MYMACLOSE"(5)
5.��̬���ӿ����ƺͺ������ƿ����Լ�����.
6.ʹ��ʱ���뽫��̬���ļ������ڷɺ�����ʦ��װĿ¼�µ�FmlDLL��Ŀ¼��ʹ��.
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
	MULTIMIN_DATA,				//�������
	MULTISEC_DATA				//������
};

////////////////////////////////////////////////////
//��������
////////////////////////////////////////////////////

typedef struct tagSTKDATA	
{
	time_t	m_time;			//ʱ��,UCT
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

////////////////////////////////////////////////////
//��Ȩ����
////////////////////////////////////////////////////

typedef struct tagSPLITDATA	
{
	time_t	m_time;			//ʱ��,UCT
	float	m_fHg;			//���
	float	m_fPg;			//���
	float	m_fPgj;			//��ɼ�
	float	m_fHl;			//����
} SPLITDATA;


////////////////////////////////////////////////////
/*��������˳��(m_pfFinData����)

	���	����
	0	��������
	1	�ܹɱ�(���),
	2	���ҹ�,
	3	�����˷��˹�,
	4	���˹�,
	5	B��,
	6	H��,
	7	��ͨA��,
	8	ְ����,
	9	A2ת���,
	10	���ʲ�(ǧԪ),
	11	�����ʲ�,
	12	�̶��ʲ�,
	13	�����ʲ�,
	14	����Ͷ��,
	15	������ծ,
	16	���ڸ�ծ,
	17	�ʱ�������,
	18	ÿ�ɹ�����,
	19	�ɶ�Ȩ��,
	20	��Ӫ����,
	21	��Ӫ����,
	22	��������,
	23	Ӫҵ����,
	24	Ͷ������,
	25	��������,
	26	Ӫҵ����֧,
	27	�����������,
	28	�����ܶ�,
	29	˰������,
	30	������,
	31	δ��������,
	32	ÿ��δ����,
	33	ÿ������,
	34	ÿ�ɾ��ʲ�,
	35	����ÿ�ɾ���,
	36	�ɶ�Ȩ���,
	37	����������
	38  ��Ӫ�ֽ�����
	39  ��Ӫ�ֽ�����
	40  ��Ӫ�ֽ�����
	41  Ͷ���ֽ�����
	42  Ͷ���ֽ�����
	43  Ͷ���ֽ�����
	44  �����ֽ�����
	45  �����ֽ�����
	46  �����ֽ�����
	47  �ֽ𼰵ȼ���
	48  Ӧ���ʿ���ת��
	49  �����ת��
	50  �ɶ�����
	51  ���м�
	52  �ٶ�����
	53  ��Ӫҵ��������
	54  ˰������������
	55  ���ʲ�������
	56  ���ʲ�������
*/

////////////////////////////////////////////////////
//���ò�����ṹ
////////////////////////////////////////////////////
typedef struct tagCALCPARAM
{
	union 
	{
		const float*	m_pfParam;				//���в�����ָ��һ������������	
		const float		m_fParam;				//��ֵ����	
	};
	const int			m_nParamStart;			//���в�����Ч��ʼλ��
}CALCPARAM;


////////////////////////////////////////////////////
//���ýӿ���Ϣ���ݽṹ
////////////////////////////////////////////////////
typedef struct tagCALCINFO
{
	const DWORD			m_dwSize;				//�ṹ��С
	const DWORD			m_dwVersion;			//��������汾(V3.00 : 0x300)
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

	float*				m_pResultBuf;			//���������
	const DWORD			m_dataType;				//��������
	const float*		m_pfFinData;			//��������

//����������Ҽ��ݣ�����������ṹ������

//����Ϊ�ɺ�����ʦ��չ

	const DWORD			m_dwReserved;			// ����

	const int			m_nNumParam;			// ���ò�������
	const CALCPARAM*	m_pCalcParam;			// ���ò�������

	const DWORD			m_dwReservedEx[4];		// ����

	char*				m_strStkName;			//��Ʊ����

	SPLITDATA*			m_pSplitData;			//��Ȩ����
	int					m_nNumSplitData;		//��Ȩ����

} CALCINFO;

/* 
ע1: (������Ҽ���)
	1.�������ò�����m_pfParam1--m_pfParam4����,��ΪNULL���ʾ�ò�����Ч.
	2.��һ��������Чʱ,���������в�������Ч.
		��:m_pfParam2ΪNULL,��m_pfParam3,m_pfParam4ҲΪNULL.
	3.����1�����ǳ�������������������,�������ֻ��Ϊ��������.
	4.��m_nParam1Start<0, �����1Ϊ��������,��������*m_pfParam1;
	5.��m_nParam1Start>=0,�����1Ϊ����������,m_pfParam1ָ��һ������������,
		�����СΪm_nNumData,������Ч��ΧΪ m_nParam1Start �� m_nNumData-1.
		��ʱ����m_pData[x] �� m_pfParam1[x]��һ�µ�

ע2: (�ɺ�����ʦ��չ)
	1.����չ�ṹʹ���ò����ڼ����Ͽ�����������Ŀ�ģ���ÿ����������Ϊ��ֵ�����У��ɹ�ʽ��ʵ�ʵĵ��ò���������
	2.CALCPARAM�ṹ���ڴ��������Ϣ��ʵ�����ݣ�m_pCalcParam�����СΪm_nNumParam��������Ч��ΧΪ 0 �� m_nNumParam-1.
	3.��������˳��m_pCalcParam[0]Ϊ��һ�����������ݣ�m_pCalcParam[1]Ϊ�ڶ�������������...��Ϊ�˱��ּ��ݣ�ԭm_nParam1Start��m_pfParam1��5��������Ȼ�и�ֵ��
	4.�� iλ�õĲ���Ϊ��ֵ��ȡ��m_pCalcParam[i].m_fParam.
	5.�� iλ�õĲ���Ϊ���У�ȡ��m_pCalcParam[i].m_pfParam�������СΪm_nNumData,������Ч��ΧΪm_pCalcParam[i].m_nParamStart �� m_nNumData-1. ��m_pCalcParam[i].m_nParamStart<0, �������������Ч���ݡ�
	6.���ڿ��Ե��ö�����У�������еļ���������ڹ�ʽ�н��У�Ȼ����Ϊ���õĲ������ɡ�
	7.������չ���Է����ҵ�DLL��Ȼ���Ե��á����ݡ�

*/


////////////////////////////////////////////////////
/* �������

__declspec(dllexport) int xxxxxxxx(CALCINFO* pData);

1.����������ȫ����д.
2.����������������ʽ����,����ʵ�ʺ����������xxxxxxxx;
	����C++����������� extern "C" {   } ������.
3.������������pData->m_pResultBuf����.
4.��������-1��ʾ�����ȫ��������Ч,���򷵻ص�һ����Чֵλ��,��:
	m_pResultBuf[����ֵ] -- m_pResultBuf[m_nNumData-1]��Ϊ��Чֵ.
5.�������Ƴ��Ȳ��ܳ���15�ֽ�,��̬���ӿ��ļ������ܳ���9�ֽ�(��������չ��),��̬�����Ʋ��ܽ�SYSTEM,EXPLORER

*/

//ʾ������,ʹ��ʱ��ʵ�������滻
__declspec(dllexport) int WINAPI MYMACLOSE(CALCINFO* pData);
__declspec(dllexport) int WINAPI MYMAVAR(CALCINFO* pData);
__declspec(dllexport) int WINAPI MYBBI(CALCINFO* pData);
__declspec(dllexport) int WINAPI CHZHSHCH(CALCINFO* pData);
#ifdef __cplusplus
}
#endif //__cplusplus


#endif //__FOXFUNC_H_INCLUDE
