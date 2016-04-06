/********************************************************************************************************************************************************
*�汾��CFDInversion_Laplace_TheoryModel_Parallel
*��д��:ϸ��Ǳ��
*********************************************************************************************************************************************************
*������������
***��m_AngularFrequencyIndex�滻��m_DampingConsts
***��m_DampingConsts_RelateToAngularFrequencyIndex�滻��m_AngularFrequency_Indexs_RelateToDampingConsts
*****Ҳ������ԭ����˥��ϵ��-->��Ƶ�ʵ������ṹת��Ϊ�˽�Ƶ�ʵ�����-->˥��ϵ���ṹ(��Ӧ��Ҳ�������������������Ϣ�ĺ���)
*
***��m_ObjectAssumedSource_RelateToAngularFrequencyIndex�滻��m_ObjectAssumedSource_RelateToDampingConsts
***��m_InitalAssumedSourec_RelateToAngularFrequencyIndex�滻��m_InitalAssumedSourec_RelateToDampingConsts
***��m_SourceEstimateMulti_RelateToAngularFrequencyIndex�滻��m_SourceEstimateMulti_RelateToDampingConsts
*****Ҳ������ԭ����˥��ϵ��-->��Ƶ�ʵ������ṹת��Ϊ�˽�Ƶ�ʵ�����-->˥��ϵ���ṹ(Ҳ����������������Ϣ����غ���)
*
***�޸���m_ObjectObservationData�����ݵĴ�Žṹ��ԭ�����ڵ�-->���յ�-->˥��ϵ��-->��Ƶ�ʵ�����ת�����ڵ�-->���յ�-->��Ƶ�ʵ�����-->˥��ϵ��
*****����������������Ϣ����غ���
*
***
*****��֮���ǰ�ԭ����˥��ϵ��-->��Ƶ�ʵ������ṹת�����˽�Ƶ�ʵ�����-->˥��ϵ���Ľṹ
***
*********************************************************************************************************************************************************/

/********************************************************************************************************************************************************
*�汾:CFDInversion_Laplace_TheoryModel
*��д��:ϸ��Ǳ��
*********************************************************************************************************************************************************
*��;:����Laplace���������ݵĲ��η���
*********************************************************************************************************************************************************
*
*�ٶ�ģ�͵���Դ������������Ϊ��������BIN�ļ����ɺ궨��InitalBinName_Model��ObjectBinName_Model���궨��ʼ�ٶ�ģ�ͺ�Ŀ���ٶ�ģ��BIN�ļ����ڵ�λ��
*����ر��������Դ���������������Դ���ٶ�ģ�͵�BIN�ļ���Ҳ��������Դ��������BIN�ļ�����ö������SurfaceData_Source��ȷ��
*********************�ֱ��ú�InitalSurfaceDataSource��ObjectSurfaceDataSource����ǳ�ʼ�ٶ�ģ�ͺ�Ŀ���ٶ�ģ�͵�����ر�������Դ
*
*********************************************************************************************************************************************************
*���õĽ����ŵ�Ŀ¼�ɺ�SaveFileDirectory����Ϊ".\\Result\\"
*********************************************************************************************************************************************************
*
*�궨���������õ��ܲ���ʱ��(TimeAll)����������(TimeStep)
*�궨���Ƿ�������ʱʹ�ø߽�����С����������SquareGride_HighOrderErr_Flag(��Ϊfalse���ã���Ϊtrue����ڹ��캯����ͨ���������������ж�������ȷ��)
*�궨��������������������ʹ�õĲ�ֽ״�NormalGride_Diff_Order(��ΪTwo_Order���������Ƿ�������ʱʹ�ø߽�����С����������������һ������)
*
*********************************************************************************************************************************************************
*
*�궨��������ʱ���ϡ��¡������ĸ���������ӵ�PML��Ĳ���(NumPML_Left��NumPML_Right��NumPML_Up��NumPML_Down)(����ָͨ�������ϵ����£����ǵߵ�ģ�ͺ������)
*�궨��������ʱ���ϡ��¡������ĸ���������ӵ�PML������˥��ϵ��(MaxDampFactor_Left��MaxDampFactor_Right��MaxDampFactor_Up��MaxDampFactor_Down)
*
*********************************************************************************************************************************************************
*
*�ṹ�壺Point_x_z_rhsIndex
*�ô�������ڵ㼰���յ���X�����Z�����������������ʱ���������н��յ����γɵļ���������һ�����յ������е�����ֵ
*ע�⣺���е�==���ز�����ֻ�ж�index_x��index_z�����ж�index_rhs
*
*m_Shot_Coords�д���ڵ��������Ϣ(�ڵߵ������PML���е�������Ϣ)���������е�index_rhs�еĹ��캯��������0ֵ
*m_Recv_Coords_RelateToShotCoords�д���ڵ�����Ӧ�Ľ��յ��������Ϣ(�ڵߵ������PML���е�������Ϣ)���������е�index_rhs�еĹ��캯��������0ֵ
*m_Recv_Coords_Set�д���ڵ�����Ӧ�Ľ��յ��������������ɵļ��ϣ�����index_rhsΪ�������е��±�����
*
*m_DampingConsts�д����ѡȡ��˥��ϵ����Ϣ
*m_AngularFrequency_Indexs_RelateToDampingConsts�д��˥��ϵ������Ӧ�����н�Ƶ�ʵ�����
*
*m_NumPointTime_Used��ʾ��ʹ�õ�����ʱ�̵���
*m_Rank_RelateToNumPointTime��ʾ����ʱ�̵�������Ӧ��2�Ľ���
*m_SinglePointFreq_Gap��ʾ������Ƶ���������Ƶ�ʼ��
*
*m_ObjectAssumedSource_RelateToDampingConsts������˥����������Ӧ��ÿ����Ƶ�ʵ��ϵĹ۲����ݵĹ���Դ(�ڴ����Լ������Դ)
*m_InitalAssumedSourec_RelateToDampingConsts������˥����������Ӧ��ÿ����Ƶ�ʵ��ϵĳ�ʼ���ݵĹ���Դ(�ڴ����Լ������Դ)
*m_SourceEstimateMulti_RelateToDampingConsts������˥����������Ӧ��ÿ����Ƶ�ʵ��ϵĳ�ʼ���ݵĹ���Դ��Դ���Ƴ��ӣ���ϳ�ʼ���ݵĹ���Դ�ɵ�Դ����
*
***ע�⣺���ǵ�Դ�����Ƕ��ڵ���˥��ϵ������Ӧ�ĵ�����Ƶ�ʵ��������ڵ���Եģ�Ҳ�������е��ڵ��Դ���ݶ���һ���ģ��ڴ�Ҳ��û����������********************
*
*
***���������ķ���
*
*********************************************************************************************************************************************************/


#ifndef CONFIGURE_H
#define CONFIGURE_H


//�궨����ѡ��ģ�͵�X��Z������������Ŀ(������)
#define GrideNumX 60		
#define GrideNumZ 60


//�궨����ѡ��ģ�͵�X��Z������������(��λΪ��)
#define GrideSpaceX 25.0f
#define GrideSpaceZ 25.0f


//��һ��ö���������������ر����Դ���ٶ�ģ��BIN�ļ�����������BIN�ļ�
enum SurfaceData_Source
{
	S_D_Same , //����ر�������Դ���ٶ�ģ��BIN�ļ�
	S_D_Other  //����ر�������Դ������BIN�ļ�
};

//�궨���ʼ�ٶ�ģ�ͼ�Ŀ���ٶ�ģ�͵�����ر����ݵ���Դ
#define InitalSurfaceDataSource S_D_Same
#define ObjectSurfaceDataSource S_D_Same


//�궨���ʼ�ٶ�ģ�ͺ�Ŀ���ٶ�ģ������BIN�ļ���������ٶ��ǰ��д�ţ�δ�������µߵ�

#define InitalBinName_Model "60x60Start.bin"
#define ObjectBinName_Model "60x60True.bin"



//�궨�嵱����ر����ݴ�����ٶ�ģ���ļ���ʱ����BIN�ļ���λ�ã�������ٶ��ǰ��д�ţ�δ�������µߵ�
#define InitalBinName_Surface "Marmousi737230_qifu.bin"
#define ObjectBinName_Surface "Marmousi737230_qifu.bin"


//��һ��ö������������ٶ�ģ���ǳ�ʼ�ٶ�ģ�ͻ���Ŀ���ٶ�ģ��
enum VelocityModel_UsedAs
{
	M_As_Inital , //���ٶ�ģ���ǳ�ʼ�ٶ�ģ��
	M_As_Object   //���ٶ�ģ����Ŀ���ٶ�ģ��
};


//�궨��Ҫ������������ڵ�Ŀ¼
#define SaveFileDirectory  ".\\Result\\"


//�궨���ܲ���ʱ�䡢��������
#define TimeAll 3.0f
#define TimeStep 0.004f


//�궨��������ʱ�Ƿ���ʹ�ø߽�����С����������
#define SquareGride_HighOrderErr_Flag true


//��һ��ö�������������������������������ʹ�õĲ�ֽ���
enum Differential_Order
{
	Two_Order = 1 , //���ײ�֣��ǻ�������е�5��
	Four_Order , //�Ľײ�֣���������е�9�㣬��ǻ�������е�9��
};

//�궨��������������������ʹ�õĲ�ֽ״�
#define NormalGride_Diff_Order Two_Order


//�궨����ӵĸ��������PML��Ĳ������������������ָ����ͨ����˵�����£����ǵߵ��ٶ�ģ�ͺ������
#define NumPML_Left  20
#define NumPML_Right 20
#define NumPML_Up    20
#define NumPML_Down  20

//�궨��PI
#define PI 3.1415926f

//�궨����ӵĸ��������PML������˥��ϵ��
#define MaxDampFactor_Left  90.0f
#define MaxDampFactor_Right 90.0f
#define MaxDampFactor_Up    90.0f
#define MaxDampFactor_Down  90.0f


//����һ���ṹ�壬�ô�������ڵ㼰���յ���X�����Z�����������������ʱ���������н��յ����γɵļ���������һ�����յ������е�����ֵ
struct Point_x_z_rhsIndex
{
	int index_x ; //X���������ֵ��Ҳ���ڼ���(�����0��ʼ����)
	int index_z ; //Z���������ֵ��Ҳ���ڼ���
	int index_rhs ; //����������еĽ��յ����γɵļ����У�ÿһ�����յ������ֵ(��0��ʼ)


public :
	Point_x_z_rhsIndex() : index_x(0) , index_z(0) , index_rhs(0){}
	Point_x_z_rhsIndex(const Point_x_z_rhsIndex & other){index_x = other.index_x ; index_z = other.index_z ; index_rhs = other.index_rhs ;}

	bool operator==(const Point_x_z_rhsIndex & other) const
	{
		if((index_x == other.index_x) && (index_z == other.index_z))
			return true ;
		else
			return false ;
	}


	const Point_x_z_rhsIndex & operator=(const Point_x_z_rhsIndex & other)
	{
		index_x = other.index_x ;
		index_z = other.index_z ;
		index_rhs = other.index_rhs ;

		return *this ;
	}
};


//�궨��һ���ṹ��
struct Array_Length
{
	float len ; //��ԭ�������ϵĲ���
	float value ; //��ʱ��Ŀ�꺯��ֵ

public :
	Array_Length(float one , float two):len(one) , value(two){}
};


#endif