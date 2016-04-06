#ifndef CFDINVERSION_H
#define CFDINVERSION_H

#include<iostream>
using std::cout ;
using std::endl ;
#include<vector>
using std::vector ;
#include<fstream>
using std::fstream ;
#include<string>
using std::string ;
#include<algorithm>
using std::count ;
#include<complex>
using std::complex ;

#include<omp.h>

#include<cstdlib>
using std::rand ;
#include<ctime>
using std::time ;

#include "mumps_c_types.h"
#include "cmumps_c.h"

#include "Configure.h"


class CFDInversion
{
	int laplace_Depth_Begain ; //��������ڽ���Laplace��ȼ���ʱ
	//��ת�ٶ�ģ�ͺ󣬿�ʼ����Ĳ���

public :
	/**********************************************************�ٶ�ģ�͵ĳ�����Ϣ**********************************************************************/

	bool m_SurfaceSmooth_Flag ; //��������Ƿ�������ر�(��true��Ϊƽ�ر���Ϊfalse��Ϊ����ر�)

	int m_GrideNumX ; //�ٶ�ģ����X������������(�ڴ˰�����ر����Ϊ������ر�)
	int m_GrideNumZ ; //�ٶ�ģ����Z������������

	float m_GrideSpaceX ; //�ٶ�ģ����X�����������ļ��
	float m_GrideSpaceZ ; //�ٶ�ģ����Z�����������ļ��

private :
	/**********************************************************�ٶ�ģ�͵���ϸ��Ϣ**********************************************************************/

	vector< vector<float> > m_InitalVelocityModel ; //������ų�ʼ���ٶ�ģ�ͣ����ڲ����д�ţ������������·�ת
	vector< vector<float> > m_ObjectVelocityModel ; //�������Ŀ���ٶ�ģ��(Ҳ���������ݻ�ù۲����ݵ��ٶ�ģ��)

	vector< vector<float> > m_PseudoHessianDiag_Vector ; //�������α��������ĶԽ�Ԫ
	vector< vector<float> > m_Gradient_Vector ;  //��������ݶȷ���

	vector<int> m_InitalNonzeroCoordinates ; //������ų�ʼ�ٶ�ģ���У�ÿһ�е����һ�������ٶ�ֵ������(�����0��ʼ����)(�ٶ�ģ�������·�ת���ģ��)
	vector<int> m_ObjectNonzeroCoordinates ; //�������Ŀ���ٶ�ģ���У�ÿһ�е����һ�������ٶ�ֵ������


	float m_InitalMaxV ; //��ʼ�ٶ�ģ���е�����ٶ�ֵ
	float m_InitalMinV ; //��ʼ�ٶ�ģ���е���С�ٶ�ֵ

	float m_ObjectMaxV ; //Ŀ���ٶ�ģ���е�����ٶ�ֵ
	float m_ObjectMinV ; //Ŀ���ٶ�ģ���е���С�ٶ�ֵ

	SurfaceData_Source m_InitalSurfaceDataSource ; //��һ��ö�����ͱ�����õĳ�ʼ�ٶ�ģ�͵�����ر����������Գ�ʼ�ٶ�ģ��BIN�ļ���������BIN�ļ�
	SurfaceData_Source m_ObjectSurfaceDataSource ; //��һ��ö�����ͱ�����õ�Ŀ���ٶ�ģ�͵�����ر�����������Ŀ���ٶ�ģ��BIN�ļ���������BIN�ļ�


public :
	/**********************************************************�������ݵĻ�����Ϣ***********************************************************************/

	float m_TimeAll ; //�ܲ���ʱ��
	float m_TimeStep ; //��������

	int m_NumPointTime_Used ; //��������ʵ��ʹ�õ����ݵ�ʱ�����
	int m_Rank_RelateToNumPointTime ; //������ʵ��ʹ�õ����ݵ�ʱ��������Ӧ����2�Ķ��ٽ���
	float m_SinglePointFreq_Gap ; //����ʱ�䵽Ƶ��ʱ������������֮���Ƶ�ʼ��


	bool m_SquareGride_HighOrderErr_Flag ; //�������������ʱ�Ƿ���ʹ�ø߽�����С����������
	Differential_Order m_NormalGride_Diff_Order ; //��һ��ö����������������ݹ���������������ʹ�õĲ�ֽ״�


private :
	/**********************************************PML�������Ϣ����PML���е����˥��ϵ��**************************************************************/

	int m_NumPML_Left ; //��ģ�������ӵ�PML��Ĳ���
	int m_NumPML_Right ; //��ģ���Ҳ���ӵ�PML��Ĳ���
	int m_NumPML_Up ; //��ģ���ϲ���ӵ�PML��Ĳ���
	int m_NumPML_Down ; //��ģ���²���ӵ�PML��Ĳ���

	float m_MaxDampFactor_Left ; //ģ�������ӵ�PML������˥��ϵ��
	float m_MaxDampFactor_Right ; //ģ���Ҳ���ӵ�PML������˥��ϵ��
	float m_MaxDampFactor_Up ; //ģ���ϲ���ӵ�PML������˥��ϵ��
	float m_MaxDampFactor_Down ; //ģ���²���ӵ�PML������˥��ϵ��

	vector<int> m_UsedTotalGrideNum_PreColumn_AddPML ; //���ڴ���ڵߵ����ٶ�ģ�Ͳ������PML���ǰN��(��������N��)���ܵ��������Ŀ
														//�������Ĵ�СΪm_GrideNumX + m_NumPML_Left + m_NumPML_Right+1
														//�±�ΪN�ĵط��洢���ǵߵ��ٶ�ģ�����PML����±��0��N-1���е������������
														//ע�⣺vector<int>�е�int������ģ�ͽϴ�ʱ��������Ի�Ϊ long long���ͻ�__int64����
														//ע�⣺�ڱ����ϲ���sizeof(int) == sizeof(long) == 4

	int m_NumUsedElementPoint_AddPML ; //������������PML�����ܵ��������Ŀ

public :
	/**********************************************************�������ݵ���ϸ��Ϣ***********************************************************************/

	vector<Point_x_z_rhsIndex> m_Shot_Coords ; //��������ڵ��������Ϣ�������õ�Ϊδ�ߵ������PML���ģ���е����꣬��������ת�����ߵ������PML����ģ���е�����

	vector< vector<Point_x_z_rhsIndex> > m_Recv_Coords_RelateToShotCoords ; //��������ڵ�����Ӧ�Ľ��յ��������Ϣ�������õ�Ϊδ�ߵ������PML���ģ���е����꣬��������ת�����ߵ������PML����ģ���е�����

	vector<Point_x_z_rhsIndex> m_Recv_Coords_Set ; //��������ڵ�����Ӧ�Ľ��յ��������������ɵļ��ϣ��������е�index_rhs���и�ֵ��ʹ֮��־���ڼ����е�����(��0��ʼ)

	vector<int>  m_AngularFrequencyIndex ; //���������ʹ�õ����н�Ƶ�ʵ�����

	vector< vector<float> > m_DampingConsts_RelateToAngularFrequencyIndex ; //������Ž�Ƶ�ʵ���������Ӧ��˥����������Ϣ

	vector< vector<complex<float> > > m_ObjectAssumedSource_RelateToAngularFrequencyIndex ; //�����Ž�Ƶ�ʵ���������Ӧ��ÿ��˥��ϵ���Ĺ۲����ݵĹ���Դ(�ڴ����Լ������Դ)
	vector< vector<complex<float> > > m_InitalAssumedSourec_RelateToAngularFrequencyIndex ; //�����Ž�Ƶ�ʵ���������Ӧ��ÿ��˥��ϵ���ϵĳ�ʼ���ݵĹ���Դ(�ڴ����Լ������Դ)
	vector< vector<complex<float> > > m_SourceEstimateMulti_RelateToAngularFrequencyIndex ; //�����Ž�Ƶ�ʵ���������Ӧ��ÿ��˥��ϵ���ϵĳ�ʼ���ݵĹ���Դ��Դ���Ƴ��ӣ���ϳ�ʼ���ݵĹ���Դ�ɵ�Դ����

	vector< vector< vector< vector< complex<float> > > > > m_ObjectObservationData ; //������Ź۲����ݣ�˥�����Ƶ��������
																					//�ṹΪ���ڵ�->���յ�->˥������->��Ƶ�ʵ�����(�ڲ������Ӧ�ľ���ֵ)

	vector<Array_Length> m_StepLength_Value_List ; //��������󲽳�������������ݣ�����λ������õĲ���(��СΪ���Ԫ��)�ֱ�Ϊ���Ľ��������־λ


	//�ӿ�
public :
	/**********************************************************���ڽⷽ�̵���Ϣ************************************************************************/
	CMUMPS_STRUC_C m_Mumps ; //һ��mumps����


public :
	/*****���캯��*************************************************************************************************************************************/

	CFDInversion() ; //���캯��������һЩ��������������

	//�ӿ�
public :
	/*****�����ٶ�ģ�͵��ٶ�ֵ����Ӧ������ر����ݡ��жϳ�ʼģ�ͺ�Ŀ��ģ���Ƿ�ƥ��*********************************************************************/

	bool SetVelocityModel(VelocityModel_UsedAs) ; //�Գ�ʼ�ٶ�ģ�͡�Ŀ���ٶ�ģ�ͽ��и�ֵ������������Ӧ������ر���Ϣ
												  //�������������ǶԳ�ʼ�ٶ�ģ�ͻ���Ŀ���ٶ�ģ�ͽ��д���
												  //�ǵ�Ҫ���Է���ֵ����Ȼ�п��ܶ��ٶ�ģ�͵ĸ�ֵ���ɹ�

	bool InitalModel_Matching_ObjectModel() ; //�ж������õĳ�ʼ�ٶ�ģ�ͺ�Ŀ���ٶ�ģ���Ƿ�ƥ��
											  //�������true����ɽ�������ķ��ݣ���m_InitalNonzeroCoordinates��m_ObjectNonzeroCoordinates�е�������ͬ
											  //�������false�����ܽ�������ķ��ݣ�m_SurfaceSmooth_Flag��ʱʧȥ����Ƿ�Ϊ����ر������
											  //�������޸�����Ӧ�ĳ�ʼ��Ŀ���ٶ�ģ���е�������С�ٶ�ֵ(ǰ���Ƿ���trueʱ��Ϣ��Ч)

private :
	bool SetVelocityData_Inital() ; //�Ѻ�InitalBinName_Model�����BIN�ļ��е����ݶ���m_InitalVelocityModel�У�����m_InitalNonzeroCoordinates���е�һ�θ�ֵ
	bool SetSurfaceData_Inital() ;  //��m_InitalNonzeroCoordinates���еڶ��δ���Ӷ�ȷ����׼ȷ�������Ϣ

	bool SetVelocityData_Object() ; //�Ѻ�ObjectBinName_Model�����BIN�ļ��е����ݶ���m_ObjectVelocityModel�У�����m_ObjectNonzeroCoordinates���е�һ�θ�ֵ
	bool SetSurfaceData_Object() ; //��m_ObjectNonzeroCoordinates���еڶ��δ���Ӷ�ȷ����׼ȷ�������Ϣ


	//�ӿ�
public :
	/*****�����ٶ�ģ�͵�BIN�ļ�************************************************************************************************************************/

	bool CreateVelocityModelBin(VelocityModel_UsedAs , string , int =0) const ; //���ļ���ŵ��ɺ�SaveFileDirectory�����Ŀ¼�£��ļ���ΪVelocityModel_UsedFor + string + n + .bin
																				//����VelocityModel_UsedAsָ���Ǳ���m_InitalVelocityModel����m_ObjectVelocityModel�е��ٶ�����


public :
	/*****�������ǰ�Ļ������ݵĳ�ʼ��*****************************************************************************************************************/

	void PrepareForForward() ; //������m_UsedTotalGrideNum_PreColumn_AddPML�����˸�ֵ

	void SetShotCoordsInformation() ; //���������ڵ����Ϣ

	void SetRecvCoordsInformation_RelateToShotCoords() ; //���������ڵ�����Ӧ�Ľ��յ����Ϣ��ÿ���ڵ�����Ӧ�Ľ��յ����һ��Vector��

	void SetRecvCoordsSetInformation() ; //���������ڵ�����Ӧ�����н��յ��������Ϣ���γɵļ��ϣ��������е�index_rhs���и�ֵ��ʹ֮��־���ڼ����е�����(��0��ʼ)

	void SetAngularFrequencyIndexInformation() ; //������ʹ�õ����н�Ƶ�ʵ�������Ϣ

	void SetDampingConstsInformation_RelateToAngularFrequencyIndexs() ; //�������н�Ƶ�ʵ���������Ӧ��˥��������Ϣ��ÿ����Ƶ�ʵ���������Ӧ��˥��ϵ������һ��Vector��

	void SetAssumeSource_SourceEstimateInformation() ; //������ʹ�õ���Ŀ�����ݵ�Դ���ݣ���ʼ���ݵ�Դ���ݣ��Գ�ʼ���ݵ�Դ���ݽ���������Դ���Ƴ���

	void SetObjectObservationDataInformation() ; //����Ŀ��۲����ݵ�Vector��Ϣ


	//ʵ��
private :
	complex<float> GetSourceDisplacement_InTimeDomain(int i) ; //�����Դ��ʱ��������ʱ�̵�λ��ֵ(δ˥��)


	//�ӿ�
public :
	/*****���������PML�����ٶ�ģ�͵�BIN�ļ�*********************************************************************************************************/

	bool CreateVelocityModelBin_AddPML(VelocityModel_UsedAs , string , int =0) const ;//���ļ���ŵ��ɺ�SaveFileDirectory�����Ŀ¼�£��ļ���ΪVelocityModel_UsedFor + "AddPML" + string + n +"_ģ�ʹ�С_"+ .bin
																				//����VelocityModel_UsedAsָ���Ǳ���m_InitalVelocityModel����m_ObjectVelocityModel���PML�����ٶ�����
																		//Ĭ�ϼ��裬Ŀ���ٶ�ģ�����ʼ�ٶ�ģ��ƥ��
	
private :
	float GetVelocity_PML_MappingToNormal(VelocityModel_UsedAs , int column , int row) const ; //�������Ӧ�ĵߵ��ٶ�ģ�Ͳ������PML���ӳ�䵽ֻ�ߵ��ٶ�ģ�͵��ٶ�ֵ
																				//����VelocityModel_UsedAsָ���ǻ��m_InitalVelocityModel����m_ObjectVelocityModel���PML�����ٶ�����
																				//����ֵ��Ϊ-1������Ϊ����Ĳ�������
																				//����ֵ��Ϊ0����˵������Ĳ����պ���(�����PML���)����ر����棬�����޶��Ĵ���ο���

	//�ӿ�
public :
	/*****���������PML�����ٶ�ģ�͵�BIN�ļ�*********************************************************************************************************/

	bool CreateDampFactorBin_AddPML(int =0) const ;//���ļ���ŵ��ɺ�SaveFileDirectory�����Ŀ¼�£��ļ���Ϊstring + n +"_ģ�ʹ�С_"+ .bin
															//Ĭ�ϼ��裬Ŀ���ٶ�ģ�����ʼ�ٶ�ģ��ƥ��

private :
	bool CreateDampFactorBin_AddPML_OneCase(int num , bool flag_RowOrientation , bool flag_IntegralPoint) const ; 
																		//��������Ӧ�ĵߵ����ٶ�ģ�Ͳ������PML��󣬵����򡢵��ֵ�����˥��ϵ����Bin�ļ�
																		//���ļ���ŵ��ɺ�SaveFileDirectory�����Ŀ¼�£��ļ���Ϊ"DampFactor_" + num + "����͵�����" + ģ�ʹ�С +.bin 
																		//Ĭ�ϼ��裬Ŀ���ٶ�ģ�����ʼ�ٶ�ģ��ƥ��


	float GetDampFactor_PML(int column , int row , bool flag_RowOrientation , bool flag_IntegralPoint , bool flag_File = false) const ;
															//�������Ӧ�ĵߵ����ٶ�ģ�Ͳ������PML�����������������ڲ�ͬ����������ϵ�˥��ϵ��
															//��������Լ��������ֵͨ�������һ������������(���������ָ��תģ�ͺ������)
															//�������壺��ת����к����к�����
															//flag_RowOrientatonΪ�Ƿ�ΪX����flag_IntegralPointΪ�Ƿ�Ϊ����
												//flag_FileΪ�Ƿ������������ļ����������ڰ�����Ĺ����ڵߵ������PML����ٶ�ģ�͵������к��������ڷֱ����º����������ʱ����������δ����ĵ�
										//����ֵΪ��-3.0f����ʾ�˳����������⣬Ӧ��������
										//����ֵΪ��-2.0f����ʾ�������������㳬�������PML������޶��Ĵ�ľ��ο�
										//����ֵΪ��-1.0f����ʾ�������������㳬���˿������ķ�Χ(�ڼ�����ߵ������PML��������������)����δ�����޶��Ĵ�ľ���

	//�ӿ�
public :
	/*****���ݵľ��庯��*******************************************************************************************************************************/

	void LaplaceFourier_Inversion() ; 

	//ʵ��
private :
	void GetObjectObservationData() ;   //���Ŀ���ٶ�ģ������Ӧ�Ĺ۲�����


	void GetPseudoHessianDiagData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume) ; //���α��������Խ�Ԫ������
			//ʵ�ֵ�ʵ��
	void SourceEstimate(int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume) ; //���ض���˥�����ֽ���Դ����


	void GetGradientData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index) ; //����ݶȾ���

	void MakeGradientPretreatment() ;  //��α��������Խ�Ԫ�����õ��ݶȾ������Ԥ����

	fstream fWrite_Step ;

	void GetStepLength_Used(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index) ; //�����ѵ���������
		//ʵ�ֵ�ʵ��
	float StepLength_MAX_Valid(float min_velocity) const ; //������������󲽳�
		//ʵ�ֵ�ʵ��
	void StepChange_InitalVelocityModel(float length) ; //�Գ�ʼģ�Ͱ���ѡ�õĲ�����������
		//ʵ�ֵ�ʵ��
	float GetObjectiveFunction_Value(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index) ;
													//��ó�ʼ�ٶ�ģ����Ŀ���ٶ�ģ��֮���Ŀ�꺯��ֵ


	void StepChange_GradientVector(float length) ; //����α�����Խ�Ԫ����Ԥ��������ݶȾ��󣬳��Բ�����ȷ�����յĳ�ʼ�ٶ�������

	bool Create_VelocityModel_GradientVector_HessianVector(int i_dampconst_index , int j_frequ_index , int iter , int flag) const ; 
													//��������ĳ�ʼ�ٶ�ģ�͡��������õ�α��������������ʹ�õ����ճ�ʼ�ٶ�ģ�����������浽BIN�ļ���
	


public :
	/**********************************************************���ڽⷽ��*****************************************************************************/

	void MumpsInital() ; //���MUMPS�Ļ�����ʼ��

	void MumpsEnd() ;     //��MUMPS������Դ�ȷ��������

	void Set_NRHS_Matrix() ; //������ͬϵ����������ұ߲��֣���������ʵ�ڵ�����ʱ������

	void Set_NRHS_Matrix_Hessian() ; //������ͬϵ����������ұ߲��֣�ֻ��Ϊ�����ʱʹ��

	void Set_Coefficient_Matrix(VelocityModel_UsedAs , complex<float> complex_frequency) ;//���ó�ʼ�ٶȡ�Ŀ���ٶ�ģ�͵ߵ������PML�������Ӧ��ͬϵ��������ķ���ϵ��
																				//����VelocityModel_UsedAsָ���ǻ��m_InitalVelocityModel����m_ObjectVelocityModel���PML�����ٶ�����

private :
	int GetIndex_InColumnStoredModel_AddPML(int column , int row , int offset_c , int offset_r) const ;
											//���ذ����ŷŵߵ������PML�����ٶ�ģ��������һ������������е�λ��(��1��ʼ)
											//����ֵΪ��-2����ʾ�������������㳬�������PML������޶��Ĵ�ľ��ο�
											//����ֵΪ��-1����ʾ�������������㳬���˿������ķ�Χ(�ڼ�����ߵ������PML��������������)����δ�����޶��Ĵ�ľ���

	//ʵ��
private :
	//FFT�任
	void FFT(complex<float> * TD, complex<float> * FD, int r)
	{
		// ����Ҷ�任����
		long count;

		// ѭ������
		int		i,j,k;

		// �м����
		int		bfsize,p;

		// �Ƕ�
		float angle;
		//WΪҪ�õ�����ת����
		//X1,X2�ֱ�Ϊ���������������������

		complex<float> *W,*X1,*X2,*X;

		// ���㸶��Ҷ�任����
		count = 1 << r;

		// ������������洢��
		W  = new complex<float>[count / 2]; 
		X1 = new complex<float>[count];
		X2 = new complex<float>[count];

		// �����Ȩϵ��
		for(i = 0; i < count / 2; i++)
		{
			angle = -i * PI * 2 / count;
			//		W[i].real()=cos(angle);
			//		W[i].image=sin(angle);	
			W[i]=complex<float>(cos(angle),sin(angle));
		}

		// ��ʱ���д��X1
		memcpy(X1, TD, sizeof(complex<float>) * count);

		// ���õ����㷨���п��ٸ���Ҷ�任
		for(k = 0; k < r; k++)  //KΪ��ǰ��������,ҲΪ��ǰ���е������
		{
			for(j = 0; j < 1 << k; j++) //j Ϊ��ǰ���еĵ������
			{
				bfsize = 1 << (r-k);    //bfsize/2Ϊ����,��ÿ�������е����������
				for(i = 0; i < bfsize / 2; i++)
				{
					p = j * bfsize;

					//������µ�������
					X2[i + p] = X1[i + p] + X1[i + p + bfsize / 2];
					X2[i + p + bfsize / 2] = (X1[i + p] - X1[i + p + bfsize / 2]) * W[i * (1<<k)];						

				}
			}
			//ǰ������������
			X  = X1;
			X1 = X2;
			X2 = X;
		}

		// ���㵹����,��������
		for(j = 0; j < count; j++)
		{
			p = 0;
			for(i = 0; i < r; i++)
			{
				if (j&(1<<i))
				{
					p+=1<<(r-i-1);
				}
			}
			FD[j]=X1[p];
		}

		// �ͷ��ڴ�
		delete []W;
		delete []X1;
		delete []X2;
	}

	void IFFT(complex<float> * FD, complex<float> * TD, int r)
	{
		// ����Ҷ�任����
		long count;

		// ѭ������
		int		i;

		complex<float> *X;

		// ���㸶��Ҷ�任����
		count = 1 << r;

		// ������������洢��
		X = new complex<float>[count];

		// ��Ƶ���д��X
		memcpy(X, FD, sizeof(complex<float>) * count);

		// ����
		for(i = 0; i < count; i++)
		{
			//		X[i].real=.real;
			//		X[i].image=-X[i].image;
			X[i]=conj(X[i]);
		}

		// ���ÿ��ٸ���Ҷ�任
		FFT(X, TD, r);

		// ��ʱ���Ĺ���
		for(i = 0; i < count; i++)
		{
			TD[i]=conj(TD[i]/(float)count);	
			//TD[i]=conj(TD[i]);	
		}
		// �ͷ��ڴ�
		delete []X;
	}


private :
	//�󲽳�
	float ParaFit(float x[], float y[], int n)                             //��С��������
	{
		float dc=(x[n-1]-x[0])/100;
		float *xc = new float[100];
		float *yc = new float[100];
		float xcmin,ycmin=1e20f;	 
		for (int i=0;i<100;i++)
		{
			xc[i]=x[0]+i*dc;
			yc[i]=PolyInt(x,y,n,xc[i]);
			if (yc[i] < ycmin)
			{
				ycmin = yc[i];
				xcmin = xc[i];
			}
		}	
		delete []xc;
		delete []yc;
		return xcmin;
	}

	float PolyInt(float xa[],float ya[],int n,float x)                             //�����߷���ȡ����
	{
		float *c = new float[n];
		float *d = new float[n];
		int i,j,ns=0;
		float ho,hp,w,den,dy;

		float dif = fabs(x-xa[0]);	
		for (i=0;i<n;i++)//diftָ�˵㵽���������ڽ��ĵ�ľ��룬nsָ��Ӧ�ĵ㣬c,d�ж����ya��ֵ
		{
			float dift = fabs(x-xa[i]);
			if (dift < dif)
			{
				dif  = dift ;
				ns = i;
			}
			c[i] = ya[i];
			d[i] = ya[i];
		}
		float y = ya[ns--];

		for (j=0;j<n-1;j++)
		{
			for (i=0;i<n-j-1;i++)
			{
				ho = xa[i] - x;
				hp = xa[i+j+1] - x;
				w = c[i+1] - d[i];
				den = ho - hp;
				if(den == 0) cout<<"Error in routine PolyInt"<<endl ;
				den = w/den;
				d[i] = hp*den;
				c[i] = ho*den;
			}
			y += (dy=(2*ns < (n-j-1) ? c[ns+1] : d[ns--]));
		}
		delete []c;
		delete []d;
		return y;
	}


};










#endif