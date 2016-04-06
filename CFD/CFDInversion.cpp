
#include "CFDInversion.h"


CFDInversion::CFDInversion()
{
	/***���ٶ�ģ�͵Ļ�����Ϣ���и�ֵ**************************************/

	m_GrideNumX = GrideNumX ;
	m_GrideNumZ = GrideNumZ ;

	m_GrideSpaceX = GrideSpaceX ;
	m_GrideSpaceZ = GrideSpaceZ ;

	m_InitalSurfaceDataSource = InitalSurfaceDataSource ; //ö�����ͣ���configue.h�ļ��е�����ر����Դ�ĺ������ж��� ����Դ���ٶ�ģ��bin�ļ�
	m_ObjectSurfaceDataSource = ObjectSurfaceDataSource ; //ö�����ͣ���configue.h�ļ��е�����ر����Դ�ĺ������ж���

	
	cout<<SetVelocityModel(M_As_Inital)<<endl ;//���ó�ʼ�ٶ�ģ��һ������ر���Ϣ
	cout<<SetVelocityModel(M_As_Object)<<endl ;//����Ŀ���ٶ�ģ��һ������ر���Ϣ
	cout<<"��ʼ�ٶ�ģ�ͺ�Ŀ���ٶ�ģ���Ƿ�ƥ�䣺"<<InitalModel_Matching_ObjectModel()<<endl ;


	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<CreateVelocityModelBin(M_As_Inital , "")<<endl ;
	cout<<CreateVelocityModelBin(M_As_Object , "")<<endl ;

	cout<<"�Ƿ���ƽ�ر�"<<m_SurfaceSmooth_Flag<<endl ;

	cout<<m_InitalMaxV<<endl ;//�����ʼ�ٶ�ģ�͵�����ٶ�ֵ
	cout<<m_InitalMinV<<endl ;
	cout<<m_ObjectMaxV<<endl ;
	cout<<m_ObjectMinV<<endl ;
	////////////////////////////////////////////////////////////////////////


	/***�������ݵĻ�����Ϣ****************************************************/

	m_TimeAll = TimeAll ;   //��configue.h�ļ��е��ܲ���ʱ�����ж���
	m_TimeStep = TimeStep ; //��configue.h�ļ��еĲ�����������ж���

	m_SquareGride_HighOrderErr_Flag = SquareGride_HighOrderErr_Flag && (m_GrideSpaceX == m_GrideSpaceZ) ; //��configure.h�ļ��е�����ʱ�Ƿ���ʹ�ø߽�����С����������ĺ� �����������ж���
	m_NormalGride_Diff_Order = NormalGride_Diff_Order ; //��configure.h�ļ��е�����ʱ����������ʹ�õĲ�ֽ״������ж���
	if((Two_Order == m_NormalGride_Diff_Order) && (true == m_SquareGride_HighOrderErr_Flag))
		m_NormalGride_Diff_Order = Four_Order ; //������ʱ����������ʹ�õĲ�ֽ״ν�������(��������ʱ�Ƿ���ʹ�ø߽�����С����������������)


	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<"�����������õĽ״Σ�"<<2*m_NormalGride_Diff_Order<<endl ;
	////////////////////////////////////////////////////////////////////////


	/***************************************************PML�������Ϣ**********************************************************************************/

	m_NumPML_Left = NumPML_Left ; //��configure.h�ļ��еĸ�����PML��Ĳ����ĺ���ж���
	m_NumPML_Right = NumPML_Right ;
	m_NumPML_Up = NumPML_Up ;
	m_NumPML_Down = NumPML_Down ;

	m_MaxDampFactor_Left = MaxDampFactor_Left ; //��configure.h�ļ��еĸ�����PML������˥��ϵ���ĺ���ж���
	m_MaxDampFactor_Right = MaxDampFactor_Right ;
	m_MaxDampFactor_Up = MaxDampFactor_Up ;
	m_MaxDampFactor_Down = MaxDampFactor_Down ;

	
	PrepareForForward() ; //������m_UsedTotalGrideNum_PreColumn_AddPML�����˸�ֵ,
	                      //���������ڴ���ڵߵ����ٶ�ģ�Ͳ������PML���ǰN��(��������N��)���ܵ��������Ŀ

	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<"���PML�����ܵ����������"<<m_UsedTotalGrideNum_PreColumn_AddPML[m_NumPML_Left + m_GrideNumX + m_NumPML_Right]<<endl ;
	////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<CreateVelocityModelBin_AddPML(M_As_Inital , "")<<endl ;
	cout<<CreateVelocityModelBin_AddPML(M_As_Object , "")<<endl ;
	////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<CreateDampFactorBin_AddPML()<<endl ;
	////////////////////////////////////////////////////////////////////////


	
	SetShotCoordsInformation() ; //�����ڵ���յ㼰���յ㼯�ϵ���Ϣ
	SetRecvCoordsInformation_RelateToShotCoords() ;
	SetRecvCoordsSetInformation() ; 

	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<m_Shot_Coords.size()<<"\t"<<m_Recv_Coords_RelateToShotCoords[0].size()<<"\t"<<m_Recv_Coords_Set.size()<<"\t"<<m_Recv_Coords_Set[m_Recv_Coords_Set.size()-1].index_rhs<<endl ;
	////////////////////////////////////////////////////////////////////////


	SetAngularFrequencyIndexInformation() ;//���ý�Ƶ�ʵ�������˥��ϵ��
	SetDampingConstsInformation_RelateToAngularFrequencyIndexs() ;

	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<m_AngularFrequencyIndex.size()<<"\t"<<m_DampingConsts_RelateToAngularFrequencyIndex[0].size()<<endl ;
	////////////////////////////////////////////////////////////////////////


	SetAssumeSource_SourceEstimateInformation() ;//����Դ����Ϣ������Դϵ������Ϣ


	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	cout<<m_ObjectAssumedSource_RelateToAngularFrequencyIndex.size()<<"\t"<<m_ObjectAssumedSource_RelateToAngularFrequencyIndex[0].size()<<endl ;
	cout<<m_InitalAssumedSourec_RelateToAngularFrequencyIndex.size()<<"\t"<<m_InitalAssumedSourec_RelateToAngularFrequencyIndex[0].size()<<endl ;
	cout<<m_SourceEstimateMulti_RelateToAngularFrequencyIndex.size()<<"\t"<<m_SourceEstimateMulti_RelateToAngularFrequencyIndex[0].size()<<endl ;
	cout<<m_NumPointTime_Used<<"\t"<<m_Rank_RelateToNumPointTime<<endl ;
	cout<<m_SinglePointFreq_Gap<<endl ;
	////////////////////////////////////////////////////////////////////////


	SetObjectObservationDataInformation() ;  //���ù۲����ݵ������ռ估��ʽ

	////////////////////////////////////////////////����Ĵ���ֻ�����ڲ���
	//cout<<m_ObjectObservationData.size()<<"\t"<<m_ObjectObservationData[0].size()<<"\t"<<m_ObjectObservationData[239][239].size()<<"\t" \
	//	<<m_ObjectObservationData[0][0][0].size()<<endl ;
	////////////////////////////////////////////////////////////////////////



	LaplaceFourier_Inversion() ; //���з���
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////�����ٶ�ģ�ͼ���Ӧ������ر����ݣ��жϳ�ʼ��Ŀ���ٶ�ģ���Ƿ�ƥ��///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////�ӿڣ���m_InitalVelocityModel��m_InitalNonzeroCoordinates��m_ObjectVelocityModel��m_ObjectNonzeroCoordinates���д���

bool CFDInversion::SetVelocityModel(VelocityModel_UsedAs ModelAs)
{
	if(M_As_Inital == ModelAs) //���ó�ʼ�ٶ�ģ��
	{
		if(true == SetVelocityData_Inital())//���ó�ʼ�ٶ�ģ��
			return SetSurfaceData_Inital() ;//��������ر���Ϣ
	}
	else//����Ŀ���ٶ�ģ��
	{
		if(true == SetVelocityData_Object())
			return SetSurfaceData_Object() ;
	}

	return false ;
}

/////�ӿڣ���m_SurfaceSmooth_Flag��m_InitalMaxV��m_InitalMinV��m_ObjectMaxV��m_ObjectMinV���д���

bool CFDInversion::InitalModel_Matching_ObjectModel()
{
	m_SurfaceSmooth_Flag = true ;

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		if(m_InitalNonzeroCoordinates[i] != m_ObjectNonzeroCoordinates[i])
			return false ;

		if((true == m_SurfaceSmooth_Flag) && (m_GrideNumZ-1 != m_InitalNonzeroCoordinates[i]))
			m_SurfaceSmooth_Flag = false ;
	}

	m_InitalMaxV = 0.0f ;
	m_InitalMinV = 100000000.0f ;
	m_ObjectMaxV = 0.0f ;
	m_ObjectMinV = 100000000.0f ;

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=0 ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			if(m_InitalMaxV < m_InitalVelocityModel[i][j])
				m_InitalMaxV = m_InitalVelocityModel[i][j] ;

			if(m_InitalMinV > m_InitalVelocityModel[i][j])
				m_InitalMinV = m_InitalVelocityModel[i][j] ;
		}

		for(int j=0 ; j<=m_ObjectNonzeroCoordinates[i] ; ++j)
		{
			if(m_ObjectMaxV < m_ObjectVelocityModel[i][j])
				m_ObjectMaxV = m_ObjectVelocityModel[i][j] ;

			if(m_ObjectMinV > m_ObjectVelocityModel[i][j])
				m_ObjectMinV = m_ObjectVelocityModel[i][j] ;
		}
	}

	return true ;
}


/////////ʵ��:�Ӻ�InitalBinName_Model�����BIN�ļ��л�þ����ģ���ٶ�ֵ////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetVelocityData_Inital()
{
	fstream Read(InitalBinName_Model , fstream::in | fstream::binary) ; 

	if(!Read)
	{
		cout<<"�򿪺�InitalBinName_Model������ļ�("<<InitalBinName_Model<<")(���ڶ�ȡ��ʼ�ٶ�ģ��)ʧ��!!!"<<endl  ;
		return false ;
	}


	Read.seekg(0 , fstream::end) ;
	fstream::pos_type end_l = Read.tellg() ;
	Read.seekg(0 , fstream::beg) ;
	fstream::pos_type beg_l = Read.tellg() ;

	__int64 count_File = end_l-beg_l ; //�ļ���С

	if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
	{
		cout<<"��InitalBinName_Model������ļ�("<<InitalBinName_Model<<"��С����������Ϊ��ȡ��ʼ�ٶ�ģ��ʹ��!!!"<<endl ;
		Read.close() ;
		return false ;
	}
		
	m_InitalVelocityModel.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ; //���·����ʼ�ٶ�ģ����ռ�õ��ڴ�ռ�
	m_InitalNonzeroCoordinates.assign(m_GrideNumX , 0) ; //���·����ʼ�ٶ�ģ������Ӧ��ÿ�е������Ϣ��ռ�õ��ڴ�ռ�


	bool flag = false ; //��������ڴ��еĶ�ȡ���Ƿ��Ѿ����ֹ�����ֵ����true����ֹ�����false��û�г��ֹ�
	float tempVel = 0.0f ; //�������ÿ�δ��ļ��ж�ȡ�����ٶ�ֵ

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		flag = false ;

		for(int j=0 ; j<m_GrideNumZ ; ++j)
		{
			Read.read((char*)(&tempVel) , sizeof(tempVel)) ;
			
			if(0.0f != tempVel) 
			{
				m_InitalVelocityModel[i][m_GrideNumZ-1-j] = tempVel ; //�ٶ�ģ�͸�ֵ���ߵ��ٶ�ģ�͵�ÿһ������

				if(false == flag)
				{
					flag = true ;
					m_InitalNonzeroCoordinates[i] = m_GrideNumZ-1-j ;//��������ر���Ϣ
				}
			}
		}
	}

	Read.close() ;

	cout<<"���������Ӻ�InitalBinName_Model������ļ�("<<InitalBinName_Model<<")�ж�ȡ��ʼ�ٶ�ģ�ͣ�" ;
	return true ;
}

/////////ʵ��:��ó�ʼ�ٶ�ģ�͵�����ر���Ϣ////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetSurfaceData_Inital()
{
	if(S_D_Same == m_InitalSurfaceDataSource)//�����ʼ�ٶ�ģ�͵����ݺ������������ͬһ��BIN�ļ������ڴ˲����κδ���
	{
		cout<<"������ر���Ϣ��"<<endl ;
		return true ;
	}
	else
	{
		fstream Read(InitalBinName_Surface , fstream::in | fstream::binary) ; 

		if(!Read)
		{
			cout<<"�򿪺�InitalBinName_Surface������ļ�("<<InitalBinName_Surface<<")(���ڶ�ȡ��ʼ�ٶ�ģ�͵�����ر�����)ʧ��!!!"<<endl  ;
			return false ;
		}

		Read.seekg(0 , fstream::end) ;
		fstream::pos_type end_l = Read.tellg() ;
		Read.seekg(0 , fstream::beg) ;
		fstream::pos_type beg_l = Read.tellg() ;

		__int64 count_File = end_l-beg_l ; //�ļ���С
		
		if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
		{
			cout<<"��InitalBinName_Surface������ļ�("<<InitalBinName_Surface<<"��С����������Ϊ��ȡ��ʼ�ٶ�ģ������ر�����ʹ��!!!"<<endl ;
			Read.close() ;
			return false ;
		}

		bool flag = false ; //������Ǵ����Ƿ��Ѽ��
		float tempVel = 0.0f ; //�������ÿ�δ��ļ��ж�ȡ�����ٶ�ֵ

		for(int i=0 ; i<m_GrideNumX ; ++i)
		{
			flag = false ;

			for(int j=0 ; j<m_GrideNumZ ; ++j)
			{
				Read.read((char*)(&tempVel) , sizeof(tempVel)) ;

				if((false == flag) && (0.0f != tempVel))
				{
					flag = true ;

					if(m_GrideNumZ-1-j > m_InitalNonzeroCoordinates[i]) //�����Ϣ�ĵر�߶ȸ����Ѿ���ȡ���ٶ�ģ���еĵر�߶�
						return false ;
					else
						m_InitalNonzeroCoordinates[i] = m_GrideNumZ-1-j ;//��������ر���Ϣ
				}
			}
		}

		Read.close() ;
		cout<<"���������Ӻ�InitalBinName_Surface������ļ�("<<InitalBinName_Surface<<")�ж�ȡ��ʼ�ٶ�ģ�͵������Ϣ��"<<endl ;
		return true ;
	}

}

/////////ʵ��:�Ӻ�ObjectBinName_Model�����BIN�ļ��л�þ����ģ���ٶ�ֵ////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetVelocityData_Object()
{
	fstream Read(ObjectBinName_Model , fstream::in | fstream::binary) ; 

	if(!Read)
	{
		cout<<"�򿪺�ObjectBinName_Model������ļ�("<<ObjectBinName_Model<<")(���ڶ�ȡĿ���ٶ�ģ��)ʧ��!!!"<<endl  ;
		return false ;
	}

	Read.seekg(0 , fstream::end) ;
	fstream::pos_type end_l = Read.tellg() ;
	Read.seekg(0 , fstream::beg) ;
	fstream::pos_type beg_l = Read.tellg() ;

	__int64 count_File = end_l-beg_l ; //�ļ���С

	if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
	{
		cout<<"��ObjectBinName_Model������ļ�("<<ObjectBinName_Model<<"��С����������Ϊ��ȡĿ���ٶ�ģ��ʹ��!!!"<<endl ;
		Read.close() ;
		return false ;
	}

	m_ObjectVelocityModel.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ;//���·���Ŀ���ٶ�ģ����ռ�õ��ڴ�ռ�
	m_ObjectNonzeroCoordinates.assign(m_GrideNumX , 0) ; //���·���Ŀ���ٶ�ģ������Ӧ��ÿ�е������Ϣ��ռ�õ��ڴ�ռ�

	bool flag = false ; //��������ڴ��еĶ�ȡ���Ƿ��Ѿ����ֹ�����ֵ����true����ֹ�����false��û�г��ֹ�
	float tempVel = 0.0f ; //�������ÿ�δ��ļ��ж�ȡ�����ٶ�ֵ

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		flag = false ;

		for(int j=0 ; j<m_GrideNumZ ; ++j)
		{
			Read.read((char*)(&tempVel) , sizeof(tempVel)) ;
			
			if(0.0f != tempVel) 
			{
				m_ObjectVelocityModel[i][m_GrideNumZ-1-j] = tempVel ; //�ߵ��ٶ�ģ�͵�ÿһ������

				if(false == flag)
				{
					flag = true ;
					m_ObjectNonzeroCoordinates[i] = m_GrideNumZ-1-j ;
				}
			}
		}
	}

	Read.close() ;

	cout<<"���������Ӻ�ObjectBinName_Model������ļ�("<<ObjectBinName_Model<<")�ж�ȡĿ���ٶ�ģ�ͣ�" ;
	return true ;
}

/////////ʵ��:���Ŀ���ٶ�ģ�͵�����ر���Ϣ////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetSurfaceData_Object() 
{
	if(S_D_Same == m_ObjectSurfaceDataSource)//���Ŀ���ٶ�ģ�͵����ݺ������������ͬһ��BIN�ļ������ڴ˲����κδ���
	{
		cout<<"������ر���Ϣ��"<<endl ;
		return true ;
	}
	else
	{
		fstream Read(ObjectBinName_Surface , fstream::in | fstream::binary) ; 

		if(!Read)
		{
			cout<<"�򿪺�ObjectBinName_Surface������ļ�("<<ObjectBinName_Surface<<")(���ڶ�ȡĿ���ٶ�ģ�͵�����ر�����)ʧ��!!!"<<endl  ;
			return false ;
		}

		Read.seekg(0 , fstream::end) ;
		fstream::pos_type end_l = Read.tellg() ;
		Read.seekg(0 , fstream::beg) ;
		fstream::pos_type beg_l = Read.tellg() ;

		__int64 count_File = end_l-beg_l ; //�ļ���С
		if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
		{
			cout<<"��ObjectBinName_Surface������ļ�("<<ObjectBinName_Surface<<"��С����������Ϊ��ȡĿ���ٶ�ģ������ر�����ʹ��!!!"<<endl ;
			Read.close() ;
			return false ;
		}

		bool flag = false ; //������Ǵ����Ƿ��Ѽ��
		float tempVel = 0.0f ; //�������ÿ�δ��ļ��ж�ȡ�����ٶ�ֵ

		for(int i=0 ; i<m_GrideNumX ; ++i)
		{
			flag = false ;

			for(int j=0 ; j<m_GrideNumZ ; ++j)
			{
				Read.read((char*)(&tempVel) , sizeof(tempVel)) ;

				if((false == flag) && (0.0f != tempVel))
				{
					flag = true ;

					if(m_GrideNumZ-1-j > m_ObjectNonzeroCoordinates[i]) //�����Ϣ�ĵر�߶ȸ����Ѿ���ȡ���ٶ�ģ���еĵͱ�߶�
						return false ;
					else
						m_ObjectNonzeroCoordinates[i] = m_GrideNumZ-1-j ;
				}
			}
		}

		Read.close() ;

		cout<<"���������Ӻ�ObjectBinName_Surface������ļ�("<<ObjectBinName_Surface<<")�ж�ȡĿ���ٶ�ģ�͵������Ϣ��"<<endl ;
		return true ;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////�����ٶ�ģ�͵�Bin�ļ�/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool CFDInversion::CreateVelocityModelBin(VelocityModel_UsedAs ModelAs , string FileName , int n) const
{
	char buffer[10] ;

	_itoa_s(n , buffer , sizeof(buffer) , 10) ; 

	if(M_As_Inital == ModelAs) 
		FileName = "Inital_" + FileName ;
	else
		FileName = "Object_" + FileName ;

	FileName = SaveFileDirectory + FileName ;
	FileName += "_" ;
	FileName += buffer ;
	FileName += ".bin" ;

	fstream Write(FileName.c_str() , fstream::out | fstream::binary) ;

	if(!Write)
	{
		cout<<"����д"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"�����ݵ�Bin�ļ�("<<FileName<<")ʧ��!!!"<<endl ;
		return false ;
	}

	float tempVel = 0.0f ; //�������ÿһ��Ҫд���ٶ�ֵ
	int tempRow = 0 ; //�������ĳһ�����ٶ�ģ�͵Ŀ�ʼ����ֵ���к�(��0��ʼ)

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		if(M_As_Inital == ModelAs)
			tempRow = m_GrideNumZ-1 -m_InitalNonzeroCoordinates[i] ;
		else
			tempRow = m_GrideNumZ-1 -m_ObjectNonzeroCoordinates[i] ;

		for(int j=0 ; j<m_GrideNumZ ; ++j)
		{
			if(j >= tempRow)
			{
				if(M_As_Inital == ModelAs)
					tempVel = m_InitalVelocityModel[i][m_GrideNumZ-1-j] ;
				else
					tempVel = m_ObjectVelocityModel[i][m_GrideNumZ-1-j] ;
			}
			else
				tempVel = 0.0f ;

			Write.write((char*)(&tempVel) , sizeof(tempVel)) ;
		}
	}

	Write.close() ;

	cout<<"��������д"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"�����ݵ�Bin�ļ�("<<FileName<<")��"<<endl ;
	return true ;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////�������ǰ�Ļ������ݵĳ�ʼ��//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////�ӿڣ���m_UsedTotalGrideNum_PreColumn_AddPML���д���

void CFDInversion::PrepareForForward()
{
	//������m_UsedTotalGrideNum_PreColumn_AddPML���д���
	//      ���ڴ���ڵߵ����ٶ�ģ�Ͳ������PML���ǰN��(��������N��)���ܵ��������Ŀ
	m_UsedTotalGrideNum_PreColumn_AddPML.assign(m_NumPML_Left + m_GrideNumX + m_NumPML_Right+1 , 0) ;

	int temp_Column = 0 ; //�����PML����У���δ���PML������е�ӳ����

	for(int i=1 ; i<m_NumPML_Left + m_GrideNumX + m_NumPML_Right + 1 ; ++i)
	{
		temp_Column = i-1 ; //�кż�1���������������±�ΪN�ĵط���ŵ��Ǵ��±�Ϊ0���±�ΪN-1�е����������

		if(temp_Column < m_NumPML_Left) //�����PML����
			temp_Column = 0 ; 
		else
			if(temp_Column < m_NumPML_Left + m_GrideNumX) //����������
				temp_Column -= m_NumPML_Left ;
			else
				temp_Column = m_GrideNumX - 1 ; //���Ҳ�PML����

		m_UsedTotalGrideNum_PreColumn_AddPML[i] = m_UsedTotalGrideNum_PreColumn_AddPML[i-1] + \
												m_NumPML_Up + m_InitalNonzeroCoordinates[temp_Column] +1 + m_NumPML_Down ;

	}

	m_NumUsedElementPoint_AddPML = m_UsedTotalGrideNum_PreColumn_AddPML[m_NumPML_Left + m_GrideNumX + m_NumPML_Right] ;
}

/////�ӿڣ��������е��ڵ��������Ϣ��Ҳ����m_Shot_Coords���д���

void CFDInversion::SetShotCoordsInformation()
{
	int num_shot = m_GrideNumX/2 +1 ;

	m_Shot_Coords.assign(num_shot , Point_x_z_rhsIndex()) ; //�ڴ˴�������240���ڵ�

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i)  //�Ȼ�����ٶ�ģ��δ�ߵ���Ҳδ���PML���ģ���е��ڵ�������Ϣ(�����Ǵ�0��ʼ����)
	{
		m_Shot_Coords[i].index_x = 2*i ;
		m_Shot_Coords[i].index_z = 0 ;	
	}

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //����Ϣת�����ٶ�ģ�͵ߵ����������PML���ģ���е��ڵ�������Ϣ(�����0��ʼ����)
	{
		m_Shot_Coords[i].index_z = m_InitalNonzeroCoordinates[m_Shot_Coords[i].index_x] - m_Shot_Coords[i].index_z + m_NumPML_Down ;
		m_Shot_Coords[i].index_x += m_NumPML_Left ;
	}

}

/////�ӿڣ������ڵ�����Ӧ�����н��յ����Ϣ(ÿ���ڵ�����Ӧ�Ľ��յ����һ��Vector��)��Ҳ����m_Recv_Coords_RelateToShotCoords���д���

void CFDInversion::SetRecvCoordsInformation_RelateToShotCoords()
{
	int num_recv = m_GrideNumX/2 +1 ;
	m_Recv_Coords_RelateToShotCoords.assign(m_Shot_Coords.size() , vector<Point_x_z_rhsIndex>(num_recv , Point_x_z_rhsIndex())) ; //ÿ���ڵ�����240�����յ�

	//�Ȼ�����ٶ�ģ��δ�ߵ���Ҳδ���PML���ģ���е������ÿ���ڵ�Ľ��յ��������Ϣ(�����Ǵ�0��ʼ����)
	for(int i=0 ; i<m_Recv_Coords_RelateToShotCoords.size() ; ++i) //�ڵ�����
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //�ڵ�����Ӧ�Ľ��յ������
		{
			m_Recv_Coords_RelateToShotCoords[i][j].index_x = 2*j ;
			m_Recv_Coords_RelateToShotCoords[i][j].index_z = 0 ; 
		}
	}

	//����Ϣת�����ٶ�ģ�͵ߵ����������PML���ģ���е������ÿ���ڵ�Ľ��յ��������Ϣ(�����0��ʼ����)
	for(int i=0 ; i<m_Recv_Coords_RelateToShotCoords.size() ; ++i) //�ڵ�����
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //�ڵ�����Ӧ�Ľ��յ������
		{
			m_Recv_Coords_RelateToShotCoords[i][j].index_z = m_InitalNonzeroCoordinates[m_Recv_Coords_RelateToShotCoords[i][j].index_x] - \
																m_Recv_Coords_RelateToShotCoords[i][j].index_z + m_NumPML_Down ;
			m_Recv_Coords_RelateToShotCoords[i][j].index_x += m_NumPML_Left ;
			
		}
	}
}

/////�ӿڣ����������ڵ����Ե����н��յ���������ɵļ��ϣ�Ҳ����m_Recv_Coords_Set���д���

void CFDInversion::SetRecvCoordsSetInformation() 
{
	m_Recv_Coords_Set.clear() ; //�������

	for(int i=0 ; i<m_Recv_Coords_RelateToShotCoords.size() ; ++i) //�ڵ�����
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //�ڵ�����Ӧ�Ľ��յ������
		{
			if(0 == count(m_Recv_Coords_Set.begin() , m_Recv_Coords_Set.end() , m_Recv_Coords_RelateToShotCoords[i][j])) //Ҳ��û�д�Ź��˽��յ�
			{
				m_Recv_Coords_Set.push_back(m_Recv_Coords_RelateToShotCoords[i][j]) ;
				m_Recv_Coords_Set[m_Recv_Coords_Set.size()-1].index_rhs = m_Recv_Coords_Set.size()-1 ;
			}

			//����Ź��˵���������
		}
	}
}

/////�ӿڣ�������ʹ�õ�����˥��ϵ����Ҳ����m_DampingConsts���д���

void CFDInversion::SetAngularFrequencyIndexInformation()
{
	int iiw[] ={7,10,14,19,30,39,50};  
		//{10.07,16.01,25.4,40.5,64.38,102.4};//{14.336,21.7,32.97,50.05,75.94,115.18,174.7};//{14.336,23.26,37.76,61.31,99.57};// {14.336,21.3,31.5,47.104,68.96,105.55};//{14.336,17.2,20.4,24.5,29.9,35.6,43.0,51.6,62.26,74.96,90.11,106.49};//{14.3,17.2,20.9,24.6,29.9,35.6,43.0,51.2,62.3,75.0,90.1};//{4.6,5.5,6.6,8.0,9.55,11.5,13.8,16.6,19.9,24.0,31.7,34.6,41.6,50.0}; //{8 , 12 , 16 , 23 , 32} ;//{8 , 10 , 12 , 15 , 18 , 21 , 26 , 31}  ; //   //ѡȡ��Ƶ�ʵ�
	
	m_AngularFrequencyIndex.assign(7, 0) ;  //�ڴ˴�����7����Ƶ�ʵ�����

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i)
	{
		m_AngularFrequencyIndex[i] = iiw[i] ;
	}
}

/////�ӿڣ�����˥��ϵ������Ӧ�����н�Ƶ�ʵ�����(ÿ��˥��ϵ������Ӧ�Ľ�Ƶ�ʵ���������һ��Vector��)��Ҳ����m_AngularFrequency_Indexs_RelateToDampingConsts���д���

void CFDInversion::SetDampingConstsInformation_RelateToAngularFrequencyIndexs()
{
	float iconstant[] = {14 , 12 , 10 , 8 , 6 , 4 , 2} ;
	m_DampingConsts_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<float>()) ; //��Ƶ�ʵ���������

	for(int i=0 ; i<m_DampingConsts_RelateToAngularFrequencyIndex.size() ; ++i)
	{
		m_DampingConsts_RelateToAngularFrequencyIndex[i].assign(1 , 0.0f) ;//ÿ����Ƶ�ʵ���������1��˥��ϵ��

		for(int j=0 ; j<m_DampingConsts_RelateToAngularFrequencyIndex[i].size() ; ++j)
		{
			m_DampingConsts_RelateToAngularFrequencyIndex[i][j] = 0.0f ; //iconstant[j] ;
		}
	}
}

/////�ӿڣ�������ʹ�õ���Ŀ�����ݵ�Դ���ݣ���ʼ���ݵ�Դ���ݣ��Գ�ʼ���ݵ�Դ���ݽ���������Դ���Ƴ���
/////Ҳ����m_ObjectAssumedSource_RelateToDampingConsts��m_InitalAssumedSourec_RelateToDampingConsts��m_SourceEstimateMulti_RelateToDampingConsts���д���
/////���Ҵ���m_NumPointTime_Used(��ʹ�õ�����ʱ�̵���)��m_Rank_RelateToNumPointTime(����ʱ�̵�������Ӧ��2�Ľ���)��m_SinglePointFreq_Gap(������Ƶ���������Ƶ�ʼ��)

void CFDInversion::SetAssumeSource_SourceEstimateInformation()
{
	m_ObjectAssumedSource_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<complex<float> >()) ; //��Ƶ�ʵ���������
	m_InitalAssumedSourec_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<complex<float> >()) ; //��Ƶ�ʵ���������
	m_SourceEstimateMulti_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<complex<float> >()) ; //��Ƶ�ʵ���������

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i)
	{
		m_ObjectAssumedSource_RelateToAngularFrequencyIndex[i].assign(m_DampingConsts_RelateToAngularFrequencyIndex[i].size() , complex<float>()) ; //ÿ����Ƶ�ʵ���������Ӧ��˥��ϵ���ĸ���
		m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i].assign(m_DampingConsts_RelateToAngularFrequencyIndex[i].size() , complex<float>()) ; //ÿ����Ƶ�ʵ���������Ӧ��˥��ϵ���ĸ���

		m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i].assign(m_DampingConsts_RelateToAngularFrequencyIndex[i].size() , complex<float>(1.0f , 0.0f)) ; //ÿ����Ƶ�ʵ���������Ӧ��˥��ϵ���ĸ���
	}

	int nt = (int)(m_TimeAll/m_TimeStep) + 1 ;  //ԭ�����ܵĲ�������

	int ntnew = 1 ; //�������õĵ���
	int r = 0 ;     //�������õĵ�����2�Ķ��ٽ���

	while(ntnew < nt)
	{
		ntnew<<=1 ;
		++r ;
	}

	int r_add = 0 ; //���������Ĳ���Ҫ��ӵĽ���
	r_add = 0 ;

	ntnew<<=r_add ;
	r += r_add ;

	m_NumPointTime_Used = ntnew ;
	m_Rank_RelateToNumPointTime = r ;

	m_SinglePointFreq_Gap = 1.0f/m_TimeStep/m_NumPointTime_Used ;

	complex<float> * source_Time = new complex<float>[m_NumPointTime_Used] ; 
	for(int i=0 ; i<m_NumPointTime_Used ; ++i) //�����Դ��λ����Ϣ
	{
		source_Time[i] = GetSourceDisplacement_InTimeDomain(i) ;
	}

	complex<float> * source_Time_Damped = new complex<float>[m_NumPointTime_Used] ; 
	complex<float> * source_Freq_Damped = new complex<float>[m_NumPointTime_Used] ; 

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i)  //��Ƶ�ʵ�����
	{
		for(int j=0 ; j<m_DampingConsts_RelateToAngularFrequencyIndex[i].size() ; ++j)
		{
			for(int k=0 ; k<m_NumPointTime_Used ; ++k)
			{
				source_Time_Damped[k] = source_Time[k]*exp(-m_DampingConsts_RelateToAngularFrequencyIndex[i][j]*m_TimeStep*k) ;
			}

			FFT(source_Time_Damped , source_Freq_Damped , m_Rank_RelateToNumPointTime) ;

			m_ObjectAssumedSource_RelateToAngularFrequencyIndex[i][j] = source_Freq_Damped[m_AngularFrequencyIndex[i]] ;
			m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i][j] = source_Freq_Damped[m_AngularFrequencyIndex[i]] ;
		}		

	}

	delete [] source_Freq_Damped ;
	delete [] source_Time_Damped ;
	delete [] source_Time ;
}

/////�ӿڣ�����Ŀ��۲����ݵ�Vector��Ϣ

void CFDInversion::SetObjectObservationDataInformation()
{
	m_ObjectObservationData.assign(m_Shot_Coords.size() , vector< vector< vector< complex<float> > > >()); //�����ڵ���Ŀ
	
	for(int i=0 ; i<m_ObjectObservationData.size() ; ++i)
	{
		m_ObjectObservationData[i].assign(m_Recv_Coords_RelateToShotCoords[i].size() , vector< vector< complex<float> > >()) ; //���ý��յ���Ŀ

		for(int j=0 ; j<m_ObjectObservationData[i].size() ; ++j)
		{
			m_ObjectObservationData[i][j].assign(m_AngularFrequencyIndex.size() , vector< complex<float> >()) ; //��Ƶ�ʵ�������Ŀ

			for(int k=0 ; k<m_ObjectObservationData[i][j].size() ; ++k)
			{
				m_ObjectObservationData[i][j][k].assign(m_DampingConsts_RelateToAngularFrequencyIndex[k].size() , complex<float>()) ;//˥��ϵ�����ڲ�����ֵΪ0.0f
			}
		}
	}
}

/////////ʵ�֣������Դ��ʱ��������ʱ�̵�λ��ֵ(δ˥��)/////////////////////////////////////////////////////////////////////////////////////////////////

complex<float> CFDInversion::GetSourceDisplacement_InTimeDomain(int i)
{
	////
	//mainFreq = 20.0f ;
	//t0 = 1/mainFreq ;
	////

	double mainFreq =15.0f ; //15.0 ; //8.0 ;
	double t0 = 1.5*sqrt(6.0)/PI/mainFreq ;
	double t = i*m_TimeStep ;

	

	double middle = PI*mainFreq*(t - t0) ;
	middle *= middle ;

	double ret_temp ;
	ret_temp = (1.0 - 2*middle)*exp(-middle) ;

	complex<float> ret ;
	ret = complex<float>((float)ret_temp , 0.0f) ;

	return ret ;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////���������PML�����ٶ�ģ�͵�Bin�ļ�/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////�ӿڣ�Ĭ�ϼ��裬Ŀ���ٶ�ģ�����ʼ�ٶ�ģ��ƥ��/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::CreateVelocityModelBin_AddPML(VelocityModel_UsedAs ModelAs , string FileName , int n) const
{
	char buffer[10] ;

	_itoa_s(n , buffer , sizeof(buffer) , 10) ; 

	if(M_As_Inital == ModelAs) 
		FileName = "Inital_AddPML_" + FileName ;
	else
		FileName = "Object_AddPML_" + FileName ;

	FileName = SaveFileDirectory + FileName ;
	FileName += buffer ;

	_itoa_s(m_NumPML_Left + m_GrideNumX + m_NumPML_Right , buffer , sizeof(buffer) , 10) ; 
	FileName += "_" ;
	FileName += buffer ;
	FileName +="x" ;
	_itoa_s(m_NumPML_Up + m_GrideNumZ + m_NumPML_Down , buffer , sizeof(buffer) , 10) ; 
	FileName += buffer ;

	
	FileName += ".bin" ;

	fstream Write(FileName.c_str() , fstream::out | fstream::binary) ;

	if(!Write)
	{
		cout<<"����д"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"�������PML������ݵ�Bin�ļ�("<<FileName<<")ʧ��!!!"<<endl ;
		return false ;
	}

	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;

	float tempVel = 0.0f ; //�������ÿһ��Ҫд���ٶ�ֵ

	for(int i=0 ; i<total_Column ; ++i)
	{
			for(int j=0 ; j<total_Row ; ++j)
			{
				tempVel = GetVelocity_PML_MappingToNormal(ModelAs , i , total_Row-1-j) ;

				if(tempVel < 0.0f)
				{
					cout<<"д"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"�������PML������ݵ�Bin�ļ�("<<FileName<<")ʧ��!!!"<<endl ;
					Write.close() ;
					return false ;
				}

				Write.write((char*)(&tempVel) , sizeof(tempVel)) ;
			}
	}

	Write.close() ;

	cout<<"��������д"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"�������PML������ݵ�Bin�ļ�("<<FileName<<")��"<<endl ;
	return true ;
}

/////////ʵ�֣�����ٶ�ģ�͵ߵ��������PML����ģ����������ӳ����ٶ�ֵ////////////////////////////////////////////////////////////////////////////////

float CFDInversion::GetVelocity_PML_MappingToNormal(VelocityModel_UsedAs ModelAs , int column , int row) const
{
	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;

	if((column < 0) || (row < 0) || (column > total_Column-1) || (row > total_Row-1)) //���������PML�����޶���һ����ľ��η�Χ
		return -1.0f ;

	int tempRow = 0 ; //ֻ�����Ӧ�����������е����������(Ҳ��ȥ����PML���еĵ���)

	if(column<m_NumPML_Left)//�����PML����
	{
		tempRow = m_InitalNonzeroCoordinates[0] ;
		column = 0 ;
	}
	else
		if(column<m_NumPML_Left + m_GrideNumX) //�����������
		{
			tempRow = m_InitalNonzeroCoordinates[column-m_NumPML_Left] ;
			column -= m_NumPML_Left ;
		}
		else
		{
			tempRow = m_InitalNonzeroCoordinates[m_GrideNumX-1] ;
			column = m_GrideNumX-1 ;
		}
			

	if(row > m_NumPML_Down + tempRow + m_NumPML_Up) //˵���������PML��������ر�֮�ϣ���û�г������PML����޶���һ����ľ��η�Χ
		return 0.0f ;


	row -= m_NumPML_Down ; //����m_NumPML_Down��ͨ��������˵��δ�ߵ�ģ��ʱ�������PML���������ڵߵ���ͱ��������

	if(row < 0) //˵���ڵߵ��ٶ�ģ�ͺ����PML����
		row = 0 ;
	else
		if(row > tempRow)
			row = tempRow ;


	if(M_As_Inital == ModelAs)
		return m_InitalVelocityModel[column][row] ;
	else
		return m_ObjectVelocityModel[column][row] ;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////���������PML����˥��ϵ����Bin�ļ�/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////�ӿڣ�Ĭ�ϼ��裬Ŀ���ٶ�ģ�����ʼ�ٶ�ģ��ƥ��/////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::CreateDampFactorBin_AddPML(int n) const
{
	bool flag = false ;
	flag = CreateDampFactorBin_AddPML_OneCase(n , true , true) ;
	if(false == flag)
		return false ;

	flag = CreateDampFactorBin_AddPML_OneCase(n , false , true) ;
	if(false == flag)
		return false ;

	flag = CreateDampFactorBin_AddPML_OneCase(n , true , false) ;
	if(false == flag)
		return false ;

	return CreateDampFactorBin_AddPML_OneCase(n , false , false) ;
}

/////////ʵ�֣���������Ӧ�ĵߵ����ٶ�ģ�Ͳ������PML��󣬵����򡢵��ֵ�����˥��ϵ����Bin�ļ�///////////////////////////////////////////////////////////

bool CFDInversion::CreateDampFactorBin_AddPML_OneCase(int num , bool flag_RowOrientation , bool flag_IntegralPoint) const
{
	string FileName("DampFactor_") ;

	char buffer[10] ;

	_itoa_s(num , buffer , sizeof(buffer) , 10) ; 

	FileName = SaveFileDirectory + FileName ;
	FileName += buffer ;

	if(true == flag_RowOrientation)
		FileName += "_Row_" ;
	else
		FileName += "_Column_" ;

	if(true == flag_IntegralPoint)
		FileName += "Intergral_" ;
	else
		FileName += "Half_" ;

	_itoa_s(m_NumPML_Left + m_GrideNumX + m_NumPML_Right , buffer , sizeof(buffer) , 10) ; 
	FileName += buffer ;
	FileName +="x" ;
	_itoa_s(m_NumPML_Up + m_GrideNumZ + m_NumPML_Down , buffer , sizeof(buffer) , 10) ; 
	FileName += buffer ;

	FileName += ".bin" ;

	fstream Write(FileName.c_str() , fstream::out | fstream::binary) ;

	if(!Write)
	{
		cout<<"����д�����PML���˥��ϵ���ļ���("<<FileName<<")ʧ��!!!"<<endl ;
		return false ;
	}

	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Down + m_GrideNumZ +  m_NumPML_Up ;

	float tempDampFactor = 0.0f ; //�������ÿһ��Ҫд��˥��ϵ��


	for(int i=0 ; i<total_Column ; ++i)
	{
		for(int j=0 ; j<total_Row ; ++j)
		{
			tempDampFactor = GetDampFactor_PML(i , total_Row-1-j ,  flag_RowOrientation , flag_IntegralPoint , false) ;
			
			if(tempDampFactor <= -2.0f)
			{
				cout<<"д���PML����˥��ϵ���ļ���("<<FileName<<")ʧ��!!!"<<endl ;
				Write.close() ;
				return false ;
			}

			if(tempDampFactor == -1.0f) //�ڴˣ���˸�ֵֻ��Ϊ��ʹ�õ���ͼ���ڹ۲����
				tempDampFactor = -10.0f ; 

			Write.write((char*)(&tempDampFactor) , sizeof(tempDampFactor)) ;
		}
	}
	
	Write.close() ;

	cout<<"��������д���PML����˥��ϵ���ļ���("<<FileName<<")��"<<endl ;
	return true ;
}

///////////////ʵ�ֵ�ʵ�֣��������Ӧ�ĵߵ����ٶ�ģ�Ͳ������PML�����������������ڲ�ͬ����������ϵ�˥��ϵ��//////////////////////////////////////

float CFDInversion::GetDampFactor_PML(int column , int row , bool flag_RowOrientation , bool flag_IntegralPoint , bool flag_File) const 
{
	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;

	if((column < 0) || (row < 0) || (column > total_Column-1) || (row > total_Row-1)) //���������PML�����޶���һ����ľ��η�Χ
		return -2.0f ;


	int tempRow = m_NumPML_Down + m_NumPML_Up ; //�����Ӧ�ĵߵ�ģ�����PML����������

	if(column<m_NumPML_Left)//�����PML����
		tempRow += m_InitalNonzeroCoordinates[0] ;
	else
		if(column<m_NumPML_Left + m_GrideNumX) //�����������
			tempRow += m_InitalNonzeroCoordinates[column-m_NumPML_Left] ;
		else
			tempRow += m_InitalNonzeroCoordinates[m_GrideNumX-1] ;

	if(row > tempRow) //�����˿������ķ�Χ����δ�����޶��Ĵ�ľ���
		return -1.0f ;

	if(true == flag_RowOrientation) //��һ���з���Ҳ��X����
	{
		if(column < m_NumPML_Left) //�����PML����
		{
			if(true == flag_IntegralPoint) //��һ�������˥��ϵ������
				return m_MaxDampFactor_Left*(1-cos(PI/2*(m_NumPML_Left - column)/m_NumPML_Left)) ;
			else
				return m_MaxDampFactor_Left*(1-cos(PI/2*(m_NumPML_Left - column -0.5)/m_NumPML_Left)) ;
		}


		column -= m_NumPML_Left ;
		if(column < m_GrideNumX) //����������
		{
			if((false == flag_IntegralPoint) && (m_GrideNumX-1 == column)) //����������������ҵ������Ұ��
			{
				if(0 == m_NumPML_Right)
				{
					if(false == flag_File)
						return -1.0f ;
					else
						return 0.0f ;
				}
				else
					return m_MaxDampFactor_Right*(1 - cos(PI/2*(0.5)/m_NumPML_Right)) ;
			}		
			else
				return 0.0f ;
		}

		column -= m_GrideNumX ;
		if(true == flag_IntegralPoint) //��һ�������˥��ϵ������
			return m_MaxDampFactor_Right*(1 - cos(PI/2*(column+1)/m_NumPML_Right)) ;
		else
			if((m_NumPML_Right-1 == column) && (false == flag_File))
				return -1.0f ;
			else
				return m_MaxDampFactor_Right*(1 - cos(PI/2*(column+1+0.5)/m_NumPML_Right)) ;

	}
	else //��һ���з���Ҳ��Z����
	{
		if(row < m_NumPML_Down) //�ڵߵ�ģ�Ͳ����PML��������PML����
		{
			if(true == flag_IntegralPoint)//��һ�������˥��ϵ������
				return m_MaxDampFactor_Down*(1 - cos(PI/2*(m_NumPML_Down - row)/m_NumPML_Down)) ;
			else
				return m_MaxDampFactor_Down*(1 - cos(PI/2*(m_NumPML_Down - row -0.5)/m_NumPML_Down)) ;
		}

		row -= m_NumPML_Down ;
		tempRow -= m_NumPML_Down + m_NumPML_Up ; //ֻ��Ǵ�����������������Ŀ
		if(row <= tempRow) //����������
		{
			if((false == flag_IntegralPoint) && (tempRow == row)) //����������������µ������°��
			{
				if(0 == m_NumPML_Up)
				{
					if(false == flag_File)
						return -1.0f ;
					else
						return 0.0f ;
				}
				else
					return m_MaxDampFactor_Up*(1 - cos(PI/2*(0.5)/m_NumPML_Up)) ;
			}	
			else
				return 0.0f ;
		}

		
		row -= tempRow ;
		if(true == flag_IntegralPoint) //��������
			return m_MaxDampFactor_Up*(1 - cos(PI/2*(row)/m_NumPML_Up)) ;
		else
			if((m_NumPML_Up == row) && (false == flag_File))
				return -1.0f ;
			else
				return m_MaxDampFactor_Up*(1 - cos(PI/2*(row +0.5)/m_NumPML_Up)) ;
	}

	return -3.0f ;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////���ݵľ��庯��////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CFDInversion::LaplaceFourier_Inversion() 
{

	GetObjectObservationData() ; //���Ŀ��ģ�͵Ĺ۲�����

	m_Gradient_Vector.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ; //���ݶȾ������ռ�
	m_PseudoHessianDiag_Vector.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ; //��α��������Խ�Ԫ����ռ�

	fWrite_Step.open("StepLength.txt" , fstream::out) ;

	vector<int> vecLapaceDepthBegin(7 , 0) ;
	vecLapaceDepthBegin[0] = 158 ; //208 ;
	vecLapaceDepthBegin[1] = 153 ; //203 ;
	vecLapaceDepthBegin[2] = 146 ; //194 ;
	vecLapaceDepthBegin[3] = 135 ; //181 ;
	vecLapaceDepthBegin[4] = 113 ; //153 ;
	vecLapaceDepthBegin[5] = 53 ; //71 ;
	vecLapaceDepthBegin[6] = 0 ; //0 ;

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i) //��Ƶ�ʵ�����
	{
		for(int j=0 ; j<m_DampingConsts_RelateToAngularFrequencyIndex[i].size() ; ++j) //��Ƶ�ʵ����Ӧ��˥��ϵ��
		{
			laplace_Depth_Begain = 0 ; //vecLapaceDepthBegin[j] ;


			MumpsInital() ; //��ʼ��

			complex<float> complex_freq = complex<float>(2*PI*m_AngularFrequencyIndex[i]*m_SinglePointFreq_Gap , -m_DampingConsts_RelateToAngularFrequencyIndex[i][j]) ;

			GetPseudoHessianDiagData(complex_freq , i , j , true) ; //��ú���������Ӧ��Դ����(false��ʾû�н���Դ����)

			for(int k=0 ; k<15 ; ++k) //����ÿһ��˥��ϵ���ͽ�Ƶ�ʶԣ�����10�ε���
			{
				m_StepLength_Value_List.assign(5 , Array_Length(0.0f , 0.0f)) ; //�������Ԫ�صĿռ䣬��ʼ��Ϊ0.0f,�󲽳�

				GetGradientData(complex_freq , i , j) ; //������ �����Ŀ�꺯��ֵ�����ݶ�

				MakeGradientPretreatment() ; //���ݶȽ���Ԥ����

				GetStepLength_Used(complex_freq , i , j) ;//�󲽳�

				if((m_StepLength_Value_List[4].value < 0) || ((fabs(m_StepLength_Value_List[3].value - m_StepLength_Value_List[0].value)<0.0001f*m_StepLength_Value_List[0].value))) 
					break ;//������Ľ����Ч

				StepChange_InitalVelocityModel(m_StepLength_Value_List[3].len) ;
				StepChange_GradientVector(m_StepLength_Value_List[3].len) ;


				Create_VelocityModel_GradientVector_HessianVector(i , j , k , 1) ;
				Create_VelocityModel_GradientVector_HessianVector(i , j , k , 2) ;
				Create_VelocityModel_GradientVector_HessianVector(i , j , k , 3) ;

			}

			MumpsEnd() ; //�����ռ�õĿռ估��Դ
		}
	}

	fWrite_Step.close() ;

}

/////////ʵ�֣����Ŀ���ٶ�ģ������Ӧ�Ĺ۲�����/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetObjectObservationData()
{
	//int num_shot = m_GrideNumX/2 ;

	////���µĴ������ڴ�һ��BIN�ļ��л�ù۲�����
	//vector< vector< vector<float> > > Test(num_shot , vector< vector<float> >(num_shot , vector<float>(2000 , 0.0f))) ;

	//fstream fileRead("MarmousiTimeRecord2000.bin" , fstream::in | fstream::binary) ;
	//
	//float temp = 0.0f ;

	//complex<float> * Record_Time = new complex<float>[m_NumPointTime_Used] ; 
	//complex<float> * Record_Freq = new complex<float>[m_NumPointTime_Used] ;

	//for(int i=0 ; i<m_NumPointTime_Used ; ++i)
	//{
	//	Record_Time[i] = 0.0f ;
	//}
	//

	//for(long long i=0 ; i<num_shot ; ++i) //�ڵ���
	//{
	//	cout<<"��"<<i<<"�ڹ۲�����"<<endl ;
	//	for(long long j=0 ; j<num_shot ; ++j) //���յ���
	//	{
	//		for(long long k=0 ; k<2000 ; ++k) //ʱ�����
	//		{
	//			
	//			fileRead.seekg( (((long long)2000)*369*i + 2000*j + k)*sizeof(float) , fstream::beg) ;
	//			fileRead.read((char*)(&temp) , sizeof(float)) ;
	//			Record_Time[k] = temp ;

	//			Test[i][j][k] = temp ;
	//		}

	//		if((i == num_shot-1) && (j == num_shot-1) )
	//		{
	//		
	//			cout<<"testSuccess"<<endl ;
	//	
	//		}
	//			
	//		FFT(Record_Time , Record_Freq , m_Rank_RelateToNumPointTime) ;

	//		for(int m=0 ; m<m_AngularFrequencyIndex.size() ; ++m) //��Ƶ�ʵ�����
	//		{
	//			m_ObjectObservationData[i][j][m][0] = \
	//				Record_Freq[ m_AngularFrequencyIndex[m] ] ;
	//		}
	//		if((i == num_shot-1) && (j == num_shot-1))
	//		{
	//				cout<<"testSuccess!!!"<<endl ;
	//		}
	//			
	//	}
	//}
	//
	//fileRead.close() ;

	//delete [] Record_Time ;
	//delete [] Record_Freq ;


	//fstream fileWrite("Test.bin" , fstream::out | fstream::binary) ;

	//for(int i=0 ; i<num_shot ; ++i)
	//{
	//	for(int j=0 ; j<num_shot ; ++j)
	//	{
	//		for(int k=0 ; k<2000 ; ++k)
	//		{
	//			temp = Test[i][j][k] ;
	//			fileWrite.write((char*)(&temp) , sizeof(float)) ;
	//		}
	//	}
	//}

	//fileWrite.close() ;

	//return ;


	MumpsInital() ; //��ʼ��

	int index = 0 ; //������¼�����±�

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i) //��Ƶ�ʵ�����
	{
		for(int j=0 ; j<m_DampingConsts_RelateToAngularFrequencyIndex[i].size() ; ++j) //��Ƶ�ʵ����Ӧ��˥��ϵ��
		{
			complex<float> complex_freq = complex<float>(2*PI*m_AngularFrequencyIndex[i]*m_SinglePointFreq_Gap , -m_DampingConsts_RelateToAngularFrequencyIndex[i][j]) ;

			Set_NRHS_Matrix() ; //�����ڵ�
			Set_Coefficient_Matrix(M_As_Object , complex_freq) ; //����ϵ������

			m_Mumps.job = 6;
			cmumps_c(&m_Mumps);

			if(0 != m_Mumps.info[1-1])
			{
				cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
			}

	#pragma omp parallel for private(index)
			for(int k=0 ; k<m_Shot_Coords.size() ; ++k) //�ڵ�
			{
				for(int m=0 ; m<m_Recv_Coords_RelateToShotCoords[k].size() ; ++m) //���յ�
				{
					index = k*m_NumUsedElementPoint_AddPML ; 
					index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_RelateToShotCoords[k][m].index_x , m_Recv_Coords_RelateToShotCoords[k][m].index_z , 0 , 0) -1 ;

					m_ObjectObservationData[k][m][i][j] = complex<float>(m_Mumps.rhs[index].r , m_Mumps.rhs[index].i)*m_ObjectAssumedSource_RelateToAngularFrequencyIndex[i][j] ;
				}
			}
		}
	}

	MumpsEnd() ; //�����ռ�õĿռ估��Դ
}



/////////ʵ�֣����α��������Խ�Ԫ������///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetPseudoHessianDiagData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume)
{
	Set_NRHS_Matrix_Hessian() ; //�������÷�����ĸ��������Դ�ʱ�ķ�������ұ߲��ֽ��и�ֵ

	Set_Coefficient_Matrix(M_As_Inital , complex_freq) ; //����ϵ������

	m_Mumps.job = 6;
	cmumps_c(&m_Mumps);

	if(0 != m_Mumps.info[1-1])
	{
		cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
	}

	SourceEstimate(i_frequ_index , j_dampconst_index , flag_sourceAssume) ; //����Դ����


	//���α��������ĶԽ�Ԫ
	
	float velocity = 0.0f ; //����ٶ�ֵ
	float b = 1.0f ; //0.6284f ; //���ĵ���ռ��Ȩ��
	complex<float> temp ; //������ʾ���е�һ��ϵ��
	complex<float> res ; //�����м���

	int index_s = 0 ; //�ڵ������
	int index_r = 0 ; //���յ������
	int index_base = 0 ; //�����Ļ�

	//�����BEGIN
	complex<float> compTempBase ;
	compTempBase = 2.0f*complex_freq*complex_freq*b ;
	compTempBase *= m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index]* \
			m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ;

	//�����END

#pragma omp parallel for private(velocity , temp , index_base , index_s , index_r , res)
	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			velocity = m_InitalVelocityModel[i][j] ; //��ô˵���ٶ�ֵ

			//temp = 2.0f*complex_freq*complex_freq*b/velocity/velocity/velocity ;
			//����Ϊ
			temp = compTempBase/velocity/velocity/velocity ;
			

			m_PseudoHessianDiag_Vector[i][j] = 0.0f ; //���ú����ĳ�ʼֵΪ0

			index_base = GetIndex_InColumnStoredModel_AddPML(i , j , m_NumPML_Left , m_NumPML_Down) -1 ;
			
			
			for(int k=0 ; k<m_Shot_Coords.size() ; ++k) //�ڵ�
			{
				for(int m=0 ; m<m_Recv_Coords_Set.size() ; ++m) //���յ�
				{
					index_s = k*m_NumUsedElementPoint_AddPML ;
					index_s += index_base ;
					index_r = (m+m_Shot_Coords.size())*m_NumUsedElementPoint_AddPML ;
					index_r += index_base ;


					//res = complex<float>(m_Mumps.rhs[index_s].r , m_Mumps.rhs[index_s].i)* \
					//	m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index]* \
					//	m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ; //�ڵ����ݵĽ��

					//����Ϊ
					res = complex<float>(m_Mumps.rhs[index_s].r , m_Mumps.rhs[index_s].i) ;

					res *= temp ;
					/*res *= complex<float>(m_Mumps.rhs[index_r].r , m_Mumps.rhs[index_r].i) ;*/
					/*if(index_base!=GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_Set[m].index_x,m_Recv_Coords_Set[m].index_z,m_NumPML_Left,m_NumPML_Left)-1)
						res=0;*/

					m_PseudoHessianDiag_Vector[i][j] += (conj(res)*res).real() ;
				}
			}


		}
	}


}

///////////////ʵ�ֵ�ʵ�֣����ض���˥�����ֽ���Դ����///////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::SourceEstimate(int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume)
{
	if(false == flag_sourceAssume) //������
	{
		m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] = 1.0f ;
		return ;
	}

	complex<float> numerator(0.0f , 0.0f) ; //�����������
	complex<float> denominator(0.0f , 0.0f) ; //���������ĸ

	complex<float> temp(0.0f , 0.0f) ;

	int index = 0 ; //������¼�����±�

	for(int k=0 ;  k<m_Shot_Coords.size() ; ++k) //�ڵ�
	{
		for(int m=0 ; m<m_Recv_Coords_RelateToShotCoords[k].size() ; ++m) //���յ�
		{
			index = k*m_NumUsedElementPoint_AddPML ; 
			index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_RelateToShotCoords[k][m].index_x , m_Recv_Coords_RelateToShotCoords[k][m].index_z , 0 , 0) -1 ;
			
			temp = complex<float>(m_Mumps.rhs[index].r , m_Mumps.rhs[index].i)*m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ;
			numerator += conj(temp)*m_ObjectObservationData[k][m][i_frequ_index][j_dampconst_index] ;
			denominator += conj(temp)*temp ;
		}
	}

	m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] = numerator/denominator ;
}



/////////ʵ�֣�����ݶȾ���/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetGradientData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index)
{
	Set_NRHS_Matrix() ; //�����ڵ�
	Set_Coefficient_Matrix(M_As_Inital , complex_freq) ; //����ϵ������

	m_Mumps.job = 6;
	cmumps_c(&m_Mumps);

	if(0 != m_Mumps.info[1-1])
	{
		cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
	}

	vector<complex<float> > forwardData(m_NumUsedElementPoint_AddPML*m_Mumps.nrhs , complex<float>(0.0f , 0.0f)) ; //����������ݵĽ��

	for(int i=0 ; i<forwardData.size() ; ++i) //��ֵ
	{
		forwardData[i] = complex<float>(m_Mumps.rhs[i].r , m_Mumps.rhs[i].i)* \
			m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index]* \
			m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ;
	}

	memset(m_Mumps.rhs , 0 , sizeof(CMUMPS_COMPLEX)*m_Mumps.nrhs*m_NumUsedElementPoint_AddPML) ; //��NRHS�����ݽ���ȫ����Ϊ0

	int index = 0 ; //������¼�����±�
	complex<float> temp ; 

	m_StepLength_Value_List[0].len = 0.0f ;
	m_StepLength_Value_List[0].value = 0.0f ; //Ϊ��Ŀ�꺯����׼��

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //�ڵ�
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //���յ�
		{
			index = i*m_NumUsedElementPoint_AddPML ; 
			index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_RelateToShotCoords[i][j].index_x , m_Recv_Coords_RelateToShotCoords[i][j].index_z , 0 , 0) -1 ;

			temp = conj(forwardData[index] - m_ObjectObservationData[i][j][i_frequ_index][j_dampconst_index]) ; //�����в�
			
			m_Mumps.rhs[index].r = temp.real() ;
			m_Mumps.rhs[index].i = temp.imag() ;

			m_StepLength_Value_List[0].value += (conj(temp)*temp).real() ;
		}
	}

	cout<<"��0��������"<<m_StepLength_Value_List[0].len<<"\t\t\t"<<m_StepLength_Value_List[0].value<<endl ;

	m_Mumps.job = 3; //��3(performs the solution)��ԭ���ǣ���ʱ=1����Ӧ��performs the analysis�Ѿ����ˣ�=2����Ӧ��performs the factorizationҲ�Ѿ����ˣ�
						//����ʹ��=6��������JOB=1,2��3��It must be preceded by a call to MUMPS with JOB=-1 on the same instance
						//�ʿ��Լ��ټ�����
	cmumps_c(&m_Mumps);


	int index_base = 0 ; //�����Ļ�
	float velocity = 0.0f ; //����ٶ�ֵ
	float b = 1.0f ; //0.6284f ; //���ĵ���ռ��Ȩ��
	complex<float> res ;

#pragma omp parallel for private(velocity , temp , index_base , index , res)
	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			velocity = m_InitalVelocityModel[i][j] ; //��ô˵���ٶ�ֵ
			temp = 2.0f*complex_freq*complex_freq*b/velocity/velocity/velocity ;

			index_base = GetIndex_InColumnStoredModel_AddPML(i , j , m_NumPML_Left , m_NumPML_Down) -1 ;
			
			m_Gradient_Vector[i][j] = 0.0f ;

			for(int k=0 ; k<m_Shot_Coords.size() ; ++k) //�ڵ�
			{	
				index = k*m_NumUsedElementPoint_AddPML ;
				index += index_base ;

				res = forwardData[index]*temp*complex<float>(m_Mumps.rhs[index].r , m_Mumps.rhs[index].i) ;

				m_Gradient_Vector[i][j] += res.real() ;
			}

		}
	}


}

/////////ʵ�֣���α��������Խ�Ԫ�����õ��ݶȾ������Ԥ����/////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::MakeGradientPretreatment()
{
	float hessianElement_max = 0 ;

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			if(fabs(m_PseudoHessianDiag_Vector[i][j]) > hessianElement_max)
				hessianElement_max = m_PseudoHessianDiag_Vector[i][j] ;
		}
	}

	float damp = 5e-4f * hessianElement_max ;

#pragma omp parallel for
	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			m_Gradient_Vector[i][j] /= damp + m_PseudoHessianDiag_Vector[i][j] ;
		}
	}
}



/////////ʵ�֣������ѵ���������///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetStepLength_Used(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index)
{
	cout<<"���Ӧ��Ŀ�꺯��ֵ��"<<m_StepLength_Value_List[0].value<<endl ;
	float stepLength_Max = StepLength_MAX_Valid(0) ; 
	cout<<"��󲽳�Ϊ��"<<stepLength_Max<<endl ;

	fWrite_Step<<"Ƶ�ʵ�����"<<i_frequ_index<<endl ;
	fWrite_Step<<"���Ӧ��Ŀ�꺯��ֵ��"<<m_StepLength_Value_List[0].value<<endl ;
	fWrite_Step<<"��󲽳�Ϊ��"<<stepLength_Max<<endl<<endl  ;


	float length ; //����
	float objectiveValue ; //��Ӧ��Ŀ�꺯��ֵ

	///////////////////////////////////////////�ƽ�ָ�������

	//float goldenRatio ;
	//float goldenRatioRemainder ;

	//float leftSelectPoint ;
	//float rightSelectPoint ;

	//goldenRatio = (sqrt(5.0)-1)/2.0 ;
	//goldenRatioRemainder = 1 - goldenRatio ;

	//m_StepLength_Value_List[1] = m_StepLength_Value_List[0] ;
	//
	////////////////////////////////
	//float loadFactor = 1.0f ;
	//loadFactor = m_StepLength_Value_List[1].len + goldenRatio*(stepLength_Max - m_StepLength_Value_List[1].len) ;
	//loadFactor /= stepLength_Max ;

	////����
	//loadFactor *= goldenRatio*goldenRatio*goldenRatio ; 
	//
	//int tempCount = 1 ;

	////float coeMin = goldenRatio*goldenRatio*goldenRatio*goldenRatio*goldenRatio ;
	////float coeMax = goldenRatio*goldenRatio*goldenRatio*goldenRatio ;

	////float rakeRatio = (coeMax - coeMin)/2 ;
	////float increment = (3*coeMin - coeMax)/2 ;
	//
	//float coeMin ;
	//float coeMax ;
	//float rakeRatio ;
	//float increment ;

	//int NumList_min[] = {5 , 3 , 2 , 1} ;
	//int NumList_max[] = {3 , 2 , 1 , 1} ;
	//int NumListLength = sizeof(NumList_min)/sizeof(int) ;

	//srand((unsigned)time(NULL)) ;

	//
	//while(1)
	//{
	//	leftSelectPoint = m_StepLength_Value_List[1].len + goldenRatioRemainder*(loadFactor*stepLength_Max - m_StepLength_Value_List[1].len) ;
	//	StepChange_InitalVelocityModel(leftSelectPoint) ; //�����ٶ�ģ��
	//	objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	//	StepChange_InitalVelocityModel(-leftSelectPoint)  ; //����Ϊԭ�����ٶ�ģ��

	//	cout<<"��"<<tempCount++<<"���������������ߵ�ѡ���"<<endl ;
	//	fWrite_Step<<"��"<<tempCount-1<<"���������������ߵ�ѡ���"<<endl ;
	//	fWrite_Step<<"\t���������"<<leftSelectPoint ;
	//	fWrite_Step<<"\t"<<objectiveValue<<endl ;

	//	if(objectiveValue < m_StepLength_Value_List[0].value)
	//		break ;

	//	{
	//		coeMin = 1.0f ;
	//		coeMax = 1.0f ;

	//		int num_max = 1 ;
	//		int num_min = 1 ;
	//		if(tempCount-2 < NumListLength)
	//		{
	//			num_max = NumList_max[tempCount-2] ;
	//			num_min = NumList_min[tempCount-2] ;
	//		}

	//		if(num_max == num_min)
	//			++num_min ;

	//		for(int i=0 ; i < num_max ; i++)
	//		{
	//			coeMax *= goldenRatio ;
	//		}
	//		for(int i=0 ; i < num_min ; i++)
	//		{
	//			coeMin *= goldenRatio ;
	//		}

	//		


	//		rakeRatio = (coeMax - coeMin)/2 ;
	//		increment = (3*coeMin - coeMax)/2 ;
	//	}


	//	loadFactor *= goldenRatio*(rakeRatio*(2.0*rand() + RAND_MAX)/RAND_MAX + increment) ;
	//}

	//m_StepLength_Value_List[3].len = leftSelectPoint ;
	//m_StepLength_Value_List[3].value = objectiveValue ;

	//
	//m_StepLength_Value_List[2].len = loadFactor*stepLength_Max ;

	//m_StepLength_Value_List[4].len = m_StepLength_Value_List[1].len + goldenRatio*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ; 
	//StepChange_InitalVelocityModel(m_StepLength_Value_List[4].len) ; //�����ٶ�ģ��
	//objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	//StepChange_InitalVelocityModel(-m_StepLength_Value_List[4].len)  ; //����Ϊԭ�����ٶ�ģ��
	//m_StepLength_Value_List[4].value = objectiveValue ;


	//fWrite_Step<<"�ұ�������"<<m_StepLength_Value_List[4].len ;
	//fWrite_Step<<"\t"<<objectiveValue<<endl ;
	//
	//tempCount++ ;

	//if(m_StepLength_Value_List[4].len - m_StepLength_Value_List[3].len > 0.001*stepLength_Max)
	//{
	//	
	//	StepChange_InitalVelocityModel(m_StepLength_Value_List[2].len) ; //�����ٶ�ģ��
	//	objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	//	StepChange_InitalVelocityModel(-m_StepLength_Value_List[2].len)  ; //����Ϊԭ�����ٶ�ģ��
	//	m_StepLength_Value_List[2].value = objectiveValue ;


	//	fWrite_Step<<"�ұ߽��"<<m_StepLength_Value_List[2].len ;
	//	fWrite_Step<<"\t"<<objectiveValue<<endl ;


	//	tempCount++ ;
	//}
	//else
	//{
	//	cout<<"���ٽ��лƽ�ָ����"<<endl ;
	//	fWrite_Step<<"���ٽ��лƽ�ָ����"<<endl ;
	//}
	//	

	//

	///////////////////////
	////rightSelectPoint = m_StepLength_Value_List[0].len + goldenRatio*(stepLength_Max - m_StepLength_Value_List[1].len) ;

	////m_StepLength_Value_List[2].len = rightSelectPoint ;
	////StepChange_InitalVelocityModel(m_StepLength_Value_List[2].len) ; //�����ٶ�ģ��
	////objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	////StepChange_InitalVelocityModel(-m_StepLength_Value_List[2].len)  ; //����Ϊԭ�����ٶ�ģ��
	////m_StepLength_Value_List[2].value = objectiveValue ;

	////leftSelectPoint = m_StepLength_Value_List[1].len + goldenRatioRemainder*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;
	////rightSelectPoint = m_StepLength_Value_List[1].len + goldenRatio*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;
	////StepChange_InitalVelocityModel(leftSelectPoint) ; //�����ٶ�ģ��
	////objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	////StepChange_InitalVelocityModel(-leftSelectPoint)  ; //����Ϊԭ�����ٶ�ģ��

	////m_StepLength_Value_List[3].len = leftSelectPoint ;
	////m_StepLength_Value_List[3].value = objectiveValue ;

	////StepChange_InitalVelocityModel(rightSelectPoint) ; //�����ٶ�ģ��
	////objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	////StepChange_InitalVelocityModel(-rightSelectPoint)  ; //����Ϊԭ�����ٶ�ģ��

	////m_StepLength_Value_List[4].len = rightSelectPoint ;
	////m_StepLength_Value_List[4].value = objectiveValue ;

	////int tempCount = 4 ;

	//while(1)
	//{
	//	if(m_StepLength_Value_List[4].len - m_StepLength_Value_List[3].len < 0.001*stepLength_Max)
	//		break ;

	//	//Сֵ�����
	//	if(m_StepLength_Value_List[3].value < m_StepLength_Value_List[4].value)
	//	{
	//		m_StepLength_Value_List[2] = m_StepLength_Value_List[4] ;
	//		m_StepLength_Value_List[4] = m_StepLength_Value_List[3] ;
	//		leftSelectPoint = m_StepLength_Value_List[1].len + goldenRatioRemainder*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;

	//		StepChange_InitalVelocityModel(leftSelectPoint) ; //�����ٶ�ģ��
	//		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	//		StepChange_InitalVelocityModel(-leftSelectPoint)  ; //����Ϊԭ�����ٶ�ģ��

	//		m_StepLength_Value_List[3].len = leftSelectPoint ;
	//		m_StepLength_Value_List[3].value = objectiveValue ;

	//		fWrite_Step<<"������������"<<endl ;
	//		fWrite_Step<<"\t��������"<<leftSelectPoint ;
	//		fWrite_Step<<"\t"<<objectiveValue<<endl ;

	//	}
	//	else
	//	{
	//		m_StepLength_Value_List[1] = m_StepLength_Value_List[3] ;
	//		m_StepLength_Value_List[3] = m_StepLength_Value_List[4] ;
	//		rightSelectPoint = m_StepLength_Value_List[1].len + goldenRatio*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;

	//		StepChange_InitalVelocityModel(rightSelectPoint) ; //�����ٶ�ģ��
	//		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ
	//		StepChange_InitalVelocityModel(-rightSelectPoint)  ; //����Ϊԭ�����ٶ�ģ��

	//		m_StepLength_Value_List[4].len = rightSelectPoint ;
	//		m_StepLength_Value_List[4].value = objectiveValue ;

	//		fWrite_Step<<"������������"<<endl ;
	//		fWrite_Step<<"\t��������"<<leftSelectPoint ;
	//		fWrite_Step<<"\t"<<objectiveValue<<endl ;
	//	}

	//	cout<<"��"<<tempCount++<<"�ε�������"<<endl ;
	//	fWrite_Step<<"��"<<tempCount-1<<"�ε�������"<<endl ;
	//}

	//if(m_StepLength_Value_List[3].value > m_StepLength_Value_List[4].value)
	//{
	//	m_StepLength_Value_List[3] = m_StepLength_Value_List[4] ;
	//}

	//if(m_StepLength_Value_List[3].value > m_StepLength_Value_List[0].value)
	//	m_StepLength_Value_List[4].value = -1.0f ;
	//else
	//	m_StepLength_Value_List[4].value = 1.0f ;

	//cout<<"����Ĳ�����"<<m_StepLength_Value_List[3].len<<endl ;
	//cout<<"���Ӧ��Ŀ�꺯��ֵ��"<<m_StepLength_Value_List[3].value<<endl ;
	//fWrite_Step<<"����Ĳ�����"<<m_StepLength_Value_List[3].len<<endl ;
	//fWrite_Step<<"���Ӧ��Ŀ�꺯��ֵ��"<<m_StepLength_Value_List[3].value<<endl ;
	//fWrite_Step<<"\n\n"<<endl ;
	//return ;

	////////////////////////////////////////������ǻƽ�ָ�����
	

	

	float test_len = 0.009f ; //0.03f ; //0.009f ; //0.03f ; //���Բ���

	for(int i=0 ; i<5+1 ; ++i)
	{
		if(5 == i) //Ҳ���޷��ҵ���Ӧ�Ĳ���
		{
			m_StepLength_Value_List[4].value = -1.0f ;
			return ;
		}
			

		length = test_len*stepLength_Max ;

		StepChange_InitalVelocityModel(length) ; //�����ٶ�ģ��

		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ

		StepChange_InitalVelocityModel(-length)  ; //����Ϊԭ�����ٶ�ģ��

		if(objectiveValue < m_StepLength_Value_List[0].value)
		{
			m_StepLength_Value_List[1].len = length ;
			m_StepLength_Value_List[1].value = objectiveValue ;
			cout<<"��һ����Ч������"<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;

			fWrite_Step<<"��һ����Ч������"<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;

			break ;

		}
		else
			test_len /= 2.0f ;

		cout<<"��һ�����Բ�����"<<length<<"\t\t\t"<<objectiveValue<<endl ;
		fWrite_Step<<"��һ�����Բ�����"<<length<<"\t\t\t"<<objectiveValue<<endl<<endl ;
	}

	test_len *= 2.0f ; 

	while(true)
	{
		length = test_len*stepLength_Max ;

		if(test_len > 0.99)
		{
			m_StepLength_Value_List[4].value = -1.0f ;
			return ;
		}

		StepChange_InitalVelocityModel(length) ; //�����ٶ�ģ��

		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ

		StepChange_InitalVelocityModel(-length)  ; //����Ϊԭ�����ٶ�ģ��

		if(objectiveValue < m_StepLength_Value_List[1].value)
		{
			m_StepLength_Value_List[1].len = length ;
			m_StepLength_Value_List[1].value = objectiveValue ;

			test_len *= 2.0f ;

			cout<<"������һ����Ч������"<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;
			fWrite_Step<<"������һ����Ч������"<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;
		}
		else
		{
			m_StepLength_Value_List[2].len = length ;
			m_StepLength_Value_List[2].value = objectiveValue ;
			cout<<"�ڶ�����Ч������"<<m_StepLength_Value_List[2].len<<"\t\t\t"<<m_StepLength_Value_List[2].value<<endl ;
			fWrite_Step<<"�ڶ�����Ч������"<<m_StepLength_Value_List[2].len<<"\t\t\t"<<m_StepLength_Value_List[2].value<<endl<<endl ;
			break ;
		}
	}


	float alpha[3],cost[3];
	cout<<"��󲽳�Ϊ��"<<stepLength_Max<<endl ;
	for(int i=0 ; i<3 ; ++i)
	{
		alpha[i] = m_StepLength_Value_List[i].len ;
		cost[i] = m_StepLength_Value_List[i].value ;
		cout<<alpha[i]<<"------"<<cost[i]<<endl ;
		fWrite_Step<<alpha[i]<<"------"<<cost[i]<<endl ;
	}

	//float denominator = (cost[1] - cost[0])*(alpha[2] - alpha[0]) - \
	//					(cost[2] - cost[0])*(alpha[1] - alpha[0]) ;

	//float numerator = (alpha[1]*alpha[1] - alpha[0]*alpha[0])*(cost[2] - cost[0]) - \
	//					(alpha[2]*alpha[2] - alpha[0]*alpha[0])*(cost[1] - cost[0]) ;

	//float  fstep = -numerator/denominator/2 ;

	//float alphamin = fstep ;

	float alphamin = ParaFit(alpha,cost,3) ; 

	m_StepLength_Value_List[3].len = alphamin ;

	StepChange_InitalVelocityModel(alphamin) ; //�����ٶ�ģ��

	objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //��ô�ʱ��Ŀ�꺯��ֵ

	StepChange_InitalVelocityModel(-alphamin)  ; //����Ϊԭ�����ٶ�ģ��

	m_StepLength_Value_List[3].value = objectiveValue ;

	cout<<"�����߷�����Ĳ�����"<<m_StepLength_Value_List[3].len<<"\t\t\t"<<m_StepLength_Value_List[3].value<<endl ;
	fWrite_Step<<"�����߷�����Ĳ�����"<<m_StepLength_Value_List[3].len<<"\t\t\t"<<m_StepLength_Value_List[3].value<<endl<<endl ;

	if((m_StepLength_Value_List[3].len > stepLength_Max) || (objectiveValue > m_StepLength_Value_List[0].value))
		m_StepLength_Value_List[4].value = -1.0f ;
	else
		m_StepLength_Value_List[4].value = 1.0f ;
}

///////////////ʵ�ֵ�ʵ�֣�������������󲽳�/////////////////////////////////////////////////////////////////////////////////////////////////////////

float CFDInversion::StepLength_MAX_Valid(float min_velocity) const
{
	float ret = -1.0f ;
	float temp = 0.0f ;

	if(min_velocity < 0)
		min_velocity = 0 ;

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			if(m_Gradient_Vector[i][j] > 1e-20) //Ҳ��>0
			{
				temp = (m_InitalVelocityModel[i][j]-min_velocity) / m_Gradient_Vector[i][j] ;

				if(ret < 0.0f)
					ret = temp ;
				if(temp < ret)
					ret = temp ;
			}
		}
	}

	return ret ;
}

///////////////ʵ�ֵ�ʵ�֣��Գ�ʼģ�Ͱ���ѡ�õĲ�����������/////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::StepChange_InitalVelocityModel(float length)
{
#pragma omp parallel for
	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			m_InitalVelocityModel[i][j] -= length*m_Gradient_Vector[i][j] ;
		}
	}
}

///////////////ʵ�ֵ�ʵ�֣���ó�ʼ�ٶ�ģ����Ŀ���ٶ�ģ��֮���Ŀ�꺯��ֵ//��///////////////////////////////////////////////////////////////////////////

float CFDInversion::GetObjectiveFunction_Value(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index)
{
	Set_NRHS_Matrix() ; //�����ڵ�
	Set_Coefficient_Matrix(M_As_Inital , complex_freq) ; //����ϵ������

	m_Mumps.job = 6;
	cmumps_c(&m_Mumps);

	if(0 != m_Mumps.info[1-1])
	{
		cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
	}

	complex<float> temp ;
	int index = 0 ;

	float ret = 0.0f ;

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //�ڵ�
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //���յ�
		{
			index = i*m_NumUsedElementPoint_AddPML ; 
			index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_RelateToShotCoords[i][j].index_x , m_Recv_Coords_RelateToShotCoords[i][j].index_z , 0 , 0) -1 ;
			
			temp = complex<float>(m_Mumps.rhs[index].r , m_Mumps.rhs[index].i)* \
			m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index]* \
			m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ;

			temp -= m_ObjectObservationData[i][j][i_frequ_index][j_dampconst_index] ;

			ret += (conj(temp)*temp).real() ;
		}
	}

	return ret ;
}



/////////ʵ�֣�����α�����Խ�Ԫ����Ԥ��������ݶȾ��󣬳��Բ�����ȷ�����յĳ�ʼ�ٶ�������///////////////////////////////////////////////////////////////

void CFDInversion::StepChange_GradientVector(float length)
{
#pragma omp parallel for
	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			m_Gradient_Vector[i][j] *= length ;
		}
	}
}

/////////ʵ�֣���������ĳ�ʼ�ٶ�ģ�͡��������õ�α��������������ʹ�õ����ճ�ʼ�ٶ�ģ�����������浽BIN�ļ���//////////////////////////////////////////

bool CFDInversion::Create_VelocityModel_GradientVector_HessianVector(int i_dampconst_index , int j_frequ_index , int iter , int flag) const 
{
	string FileName = SaveFileDirectory ;

	char buffer[10] ;

	if(1 == flag)
	{
		FileName += "Correct_VelocityModel_" ;
	}
	else
	{
		if(2 == flag)
		{
			FileName += "Correct_GradientVector_" ;
		}
		else
		{
			FileName += "Correct_HessianVector_" ;
		}
	}

	_itoa_s(i_dampconst_index+1 , buffer , sizeof(buffer) , 10) ; //��nת��Ϊʮ��������ŵ�buffer��

	FileName += buffer ;
	FileName += "_" ;

	_itoa_s(j_frequ_index+1 , buffer , sizeof(buffer) , 10) ; //��nת��Ϊʮ��������ŵ�buffer��

	FileName += buffer ;
	FileName += "_" ;

	_itoa_s(iter+1 , buffer , sizeof(buffer) , 10) ; //��nת��Ϊʮ��������ŵ�buffer��

	FileName += buffer ;
	FileName += "_" ;

	FileName += ".bin" ;

	fstream Write(FileName.c_str() , fstream::out | fstream::binary) ;

	if(!Write)
	{
		cout<<"����Bin�ļ�("<<FileName<<")ʧ��!!!"<<endl ;
		return false ;
	}

	float tempValue = 0.0f ; //�������ÿһ��Ҫд���ٶ�ֵ
	int tempRow = 0 ; //�������ĳһ�����ٶ�ģ�͵Ŀ�ʼ����ֵ���к�(��0��ʼ)

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		tempRow = m_GrideNumZ-1 -m_InitalNonzeroCoordinates[i] ;

		for(int j=0 ; j<m_GrideNumZ ; ++j)
		{
			if(j >= tempRow)
			{
				if(1 == flag)
					tempValue = m_InitalVelocityModel[i][m_GrideNumZ-1-j] ;
				else
					if(2 == flag)
						tempValue = m_Gradient_Vector[i][m_GrideNumZ-1-j] ;
					else
						tempValue = m_PseudoHessianDiag_Vector[i][m_GrideNumZ-1-j] ;
			}
			else
				tempValue = 0.0f ;

			Write.write((char*)(&tempValue) , sizeof(tempValue)) ;
		}
	}

	Write.close() ;

	cout<<"��������д���ݵ�Bin�ļ�("<<FileName<<")��"<<endl ;
	return true ;
	
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////�ⷽ�̵���ؽӿڼ�ʵ��////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//�ӿ�
void CFDInversion::MumpsInital()
{
	m_Mumps.job = -1 ; //Initalizes an instance of the package
	m_Mumps.par = 1 ;  //Host is involved in factorization/solve phases
	m_Mumps.sym = 0 ;  //A is unsymmetric

	cmumps_c(&m_Mumps) ;

	int number_Element_Normal = 0 ; //�������δ���PML��ʱ����������Ŀ

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		number_Element_Normal += m_InitalNonzeroCoordinates[i] +1 ;
	}

	int number_NozeroMatrixCoeff = 0 ; //ϵ�������еķ���Ԫ����Ŀ
	
	int multi = 0 ; //������־һ�������õ����������Ŀ

	if(true == m_SquareGride_HighOrderErr_Flag) //�û������
	{
		multi = 9 ;
		number_NozeroMatrixCoeff = m_NumUsedElementPoint_AddPML*multi ;

		cout<<"ʹ�û�������Ľײ��"<<endl ;
	}
	else
	{
		if(Two_Order == m_NormalGride_Diff_Order)
		{
			multi = 5 ;
			number_NozeroMatrixCoeff = m_NumUsedElementPoint_AddPML*multi ;

			cout<<"ʹ����ͨ������ײ��"<<endl ;
		}
		else
		{
			multi = 9 ;
			number_NozeroMatrixCoeff = m_NumUsedElementPoint_AddPML*5 + number_Element_Normal*4 ;
			cout<<"ʹ����ͨ�����Ľײ��"<<endl ;
		}
	}

	m_Mumps.irn = new int[number_NozeroMatrixCoeff]() ; //Ϊ��¼��������������ռ�
	m_Mumps.jcn = new int[number_NozeroMatrixCoeff]() ; //Ϊ��¼��������������ռ�
	m_Mumps.a   = new CMUMPS_COMPLEX[number_NozeroMatrixCoeff] ; //Ϊ��¼����������������ͬ�±��е��С���������ָ���ľ����е�ֵ���������ռ�

	m_Mumps.n = m_NumUsedElementPoint_AddPML ;  //ָ��һ���������еķ��̵ĸ���
	m_Mumps.nrhs = m_Shot_Coords.size() ; //��ͬϵ���ķ�����ĸ���
	m_Mumps.lrhs = m_NumUsedElementPoint_AddPML ; //ָ��ÿһ���ұ���ĳ���
	m_Mumps.rhs = new CMUMPS_COMPLEX[m_NumUsedElementPoint_AddPML*m_Shot_Coords.size()] ; //����ͬϵ���ķ�������ұ߳��������ռ�
	

	m_Mumps.icntl[20-1] = 0 ; //The right-hand side must be given in dense form in the structure componet RHS
	m_Mumps.icntl[21-1] = 0 ; //The solution vector will be assembled and stored in the structure component RHS
	m_Mumps.icntl[7-1] = 0 ; //Approximate Mininum Degree(AMD) is used
	m_Mumps.icntl[8-1] = 7 ; //Simultaneous row and column iterative scaling based on ... ;
								//computed during the numerical factorization phase
	m_Mumps.icntl[14-1] = 30 ; //It corresponds to the percentage increase in the estimated working space

	m_Mumps.icntl[1-1] = -1 ; //������ʾ��Ϣ
	m_Mumps.icntl[2-1] = -1 ; 
	m_Mumps.icntl[3-1] = -1 ;
	m_Mumps.icntl[4-1] = 0 ;
}

//�ӿ�
void CFDInversion::MumpsEnd()
{
	m_Mumps.job = -2 ; //Destroys an instance of the package

	cmumps_c(&m_Mumps) ;
	delete [] m_Mumps.irn ;
	delete [] m_Mumps.jcn ;
	delete [] m_Mumps.a ;
	delete [] m_Mumps.rhs ;
}

//�ӿ�
void CFDInversion::Set_NRHS_Matrix()
{
	if(m_Mumps.nrhs != m_Shot_Coords.size()) //Ҳ���ڵ���Ŀ��ƥ��ʱ
	{
		delete [] m_Mumps.rhs ;
		m_Mumps.nrhs = m_Shot_Coords.size() ; //���������ڵ���Ŀ
		m_Mumps.rhs = new CMUMPS_COMPLEX[m_NumUsedElementPoint_AddPML*m_Shot_Coords.size()]() ; //���·���ռ�
	}

	memset(m_Mumps.rhs , 0 , sizeof(CMUMPS_COMPLEX)*m_Mumps.nrhs*m_NumUsedElementPoint_AddPML) ; //��NRHS�����ݽ���ȫ����Ϊ0

	int index = 0 ; //����ָʾ����NRHS�е��±�����

#pragma omp parallel for private(index)
	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //����ÿһ���ڵ�
	{
		index = i*m_NumUsedElementPoint_AddPML ;
		index += GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0)-1 ; //���һ��������0��ʼ������

		if(GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0) <= -1)
			cout<<"ERROR!!!"<<endl ;
		m_Mumps.rhs[index].r = 1.0f ;
		m_Mumps.rhs[index].i = 0.0f ;
	}
}

//�ӿ�
void CFDInversion::Set_NRHS_Matrix_Hessian()
{
	if(m_Mumps.nrhs != m_Shot_Coords.size() + m_Recv_Coords_Set.size()) //Ҳ���ڵ���Ŀ��ƥ��ʱ
	{
		delete [] m_Mumps.rhs ;
		m_Mumps.nrhs = m_Shot_Coords.size()+ m_Recv_Coords_Set.size() ; //���������ڵ���Ŀ
		m_Mumps.rhs = new CMUMPS_COMPLEX[m_NumUsedElementPoint_AddPML*(m_Shot_Coords.size()+ m_Recv_Coords_Set.size())]() ; //���·���ռ�
	}

	memset(m_Mumps.rhs , 0 , sizeof(CMUMPS_COMPLEX)*m_Mumps.nrhs*m_NumUsedElementPoint_AddPML) ; //��NRHS�����ݽ���ȫ����Ϊ0

	int index = 0 ; //����ָʾ����NRHS�е��±�����

#pragma omp parallel for private(index)
	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //����ÿһ���ڵ�
	{
		index = i*m_NumUsedElementPoint_AddPML ;
		index += GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0)-1 ; //���һ��������0��ʼ������

		if(GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0) <= -1)
			cout<<"ERROR!!!"<<endl ;
		m_Mumps.rhs[index].r = 1.0f ;
		m_Mumps.rhs[index].i = 0.0f ;
	}

#pragma omp parallel for private(index)
	for(int i=m_Shot_Coords.size() ; i<m_Shot_Coords.size()+m_Recv_Coords_Set.size() ; ++i) //����ÿһ�����յ�
	{
		index = i*m_NumUsedElementPoint_AddPML ;
		index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_x , m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_z , 0 , 0)-1 ; //���һ��������0��ʼ������

		if(GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_x , m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_z , 0 , 0) <= -1)
			cout<<"ERROR!!!"<<endl ;
		m_Mumps.rhs[index].r = 1.0f ;
		m_Mumps.rhs[index].i = 0.0f ;
	}
}


//�ӿ�
void CFDInversion::Set_Coefficient_Matrix(VelocityModel_UsedAs ModelAs , complex<float> complex_frequency)
{
	int count_has_get = 0 ; //��������Ѿ���õ�ϵ���ĸ���

	/*****��������Ľײ��****************************************************************************************************************************/
	if(true ==m_SquareGride_HighOrderErr_Flag)
	{
		bool flag_velocity_oneOrder = true ; //Ϊtrueʱ��ʾ��ʹ���ٶȵ�һ�׽��ƣ�����ʹ���ٶȵ�һ�׽���

		bool flag_press_oneOrder = true ; //Ϊtrueʱ��ʾ��ʹ��ѹǿ��һ�׽��ƣ�����ʹ��ѹǿ��һ�׽���

		bool flag_TransDamp_approximate = true ; //Ϊtrueʱ��ʾ��ʹ��ԭʼ���������˥��ϵ��ֱ�ӽ��ƣ�����ʹ��ԭʼ��ͬ����ļ�Ȩ��ֵ������

		float a = 0.5461f ; //����ת�����ռ�ı���
		float b = 0.6284f ; //���ĸ����ռ�ı���
		float c = (1-b)/4 ; //���ĸ����ϡ��¡������ĸ������ռ�ı���

		float damp_x = 0.0f ; //���ĵ���X�����˥��ϵ��

		float damp_x_b1 = 0.0f ; //���ĵ��ұߵ��ڵ�˥��ϵ��
		float damp_x_b_1 = 0.0f ; //���ĵ������˥��ϵ��

		float damp_z = 0.0f ; //���ĵ���Z�����˥��ϵ��

		float damp_z_b1 = 0.0f ; //���ĵ����ϰ���˥��ϵ��
		float damp_z_b_1 = 0.0f ; //���ĵ����°���˥��ϵ��


		float damp_tx = 0.0f ; //��ת��������ĵ���X�����˥��ϵ��

		float damp_tx_b1 = 0.0f ; //��ת��������ĵ��Ұ���˥��ϵ��
		float damp_tx_b_1 = 0.0f ; //��ת��������ĵ������˥��ϵ��

		float damp_tz = 0.0f ; //��ת��������ĵ���Z�����˥��ϵ��

		float damp_tz_b1 = 0.0f ; //��ת��������ĵ��ϰ���˥��ϵ��
		float damp_tz_b_1 = 0.0f ; //��ת��������ĵ��°���˥��ϵ��


		float temp1 = a/m_GrideSpaceX/m_GrideSpaceX ;
		float temp2 = (1-a)/m_GrideSpaceX/m_GrideSpaceX/2 ;
		complex<float> temp3_3 = b*complex_frequency*complex_frequency ;
		complex<float> temp4_4 = c*complex_frequency*complex_frequency ;

		float velocity = 0.0f ;//������Ŵ����ĵ��ϵ��ٶ�ֵ

		complex<float> temp3 ;
		complex<float> temp4 ;

		complex<float> temp5 ;
		complex<float> temp6 ;
		complex<float> temp7 ;
		complex<float> temp8 ;
		complex<float> temp9 ;
		complex<float> temp10 ;
		complex<float> temp11 ;
		complex<float> temp12 ;


		complex<float> temp_coefficient_element(0.0f , 0.0f) ; //���������ʱ��Ԫ��ϵ��
		complex<float> Unit_Imaginary_Trans = complex<float>(0.0f , 1.0f)/complex_frequency ; //���嵥λ�������븴Ƶ�ʵ���


		for(int i=0 ; i<m_NumPML_Left + m_GrideNumX + m_NumPML_Right ; ++i)
		{
			int tempColumn = 0 ; 
			if(i<m_NumPML_Left)
				tempColumn = 0 ;
			else
				if(i<m_NumPML_Left + m_GrideNumX)
					tempColumn =i - m_NumPML_Left ;
				else
					tempColumn = m_GrideNumX-1 ;

			int totalRow = m_NumPML_Down + m_InitalNonzeroCoordinates[tempColumn] + m_NumPML_Up ; 

			for(int j=0 ; j<=totalRow ; ++j)
			{
				velocity = GetVelocity_PML_MappingToNormal(ModelAs , i , j) ;
				if(velocity <= 0.0f)
				cout<<"ERROR!!!"<<endl ;

				temp3 = temp3_3/velocity/velocity ;
				temp4 = temp4_4/velocity/velocity ;

				damp_x = GetDampFactor_PML(i , j , true , true) ;

				if(-1.0f < GetDampFactor_PML(i , j , true , false)) //�Ұ�����������
					damp_x_b1 = GetDampFactor_PML(i , j , true , false) ;
				else
					damp_x_b1 = GetDampFactor_PML(i-1 , j , true , false) ;


				if(-1.0f < GetDampFactor_PML(i-1 , j , true , false)) //�������������
					damp_x_b_1 = GetDampFactor_PML(i-1 , j , true , false) ;
				else
					damp_x_b_1 = GetDampFactor_PML(i , j , true , false) ;


				damp_z = GetDampFactor_PML(i , j , false , true) ;

				if(-1.0f < GetDampFactor_PML(i , j , false , false)) //�ϰ�����������
					damp_z_b1 = GetDampFactor_PML(i , j , false , false) ;
				else
					damp_z_b1 = GetDampFactor_PML(i , j-1 , false , false) ;


				if(-1.0f < GetDampFactor_PML(i , j-1 , false , false)) //�°�����������
					damp_z_b_1 = GetDampFactor_PML(i , j-1 , false , false) ;
				else
					damp_z_b_1 = GetDampFactor_PML(i , j , false , false) ;


				//������ת���˥��ϵ����������

				if(true == flag_TransDamp_approximate)
				{
					damp_tx = damp_x ;
					damp_tx_b1 = damp_x_b1 ;
					damp_tx_b_1 = damp_x_b_1 ;

					damp_tz = damp_z ;
					damp_tz_b1 = damp_z_b1 ;
					damp_tz_b_1 = damp_z_b_1 ;
				}
				else
				{
					damp_tx = (damp_x + damp_z)/2.0f ;
					damp_tz = damp_tx ;

					damp_tx_b1 = (damp_x_b1 + damp_z_b1)/2.0f ;
					damp_tx_b_1 = (damp_x_b_1 + damp_z_b_1)/2.0f ;

					damp_tz_b1 = (damp_x_b_1 + damp_z_b1)/2.0f ;
					damp_tz_b_1 = (damp_x_b1 + damp_z_b_1)/2.0f ;
				}


				temp5 = temp1/(1.0f - Unit_Imaginary_Trans*damp_x)/(1.0f - Unit_Imaginary_Trans*damp_x_b1) ;
				temp6 = temp1/(1.0f - Unit_Imaginary_Trans*damp_x)/(1.0f - Unit_Imaginary_Trans*damp_x_b_1) ;

				temp7 = temp1/(1.0f - Unit_Imaginary_Trans*damp_z)/(1.0f - Unit_Imaginary_Trans*damp_z_b1) ;
				temp8 = temp1/(1.0f - Unit_Imaginary_Trans*damp_z)/(1.0f - Unit_Imaginary_Trans*damp_z_b_1) ;

				temp9 = temp2/(1.0f - Unit_Imaginary_Trans*damp_tx)/(1.0f - Unit_Imaginary_Trans*damp_tx_b1) ;
				temp10 = temp2/(1.0f - Unit_Imaginary_Trans*damp_tx)/(1.0f - Unit_Imaginary_Trans*damp_tx_b_1) ;

				temp11 = temp2/(1.0f - Unit_Imaginary_Trans*damp_tz)/(1.0f - Unit_Imaginary_Trans*damp_tz_b1) ;
				temp12 = temp2/(1.0f - Unit_Imaginary_Trans*damp_tz)/(1.0f - Unit_Imaginary_Trans*damp_tz_b_1) ;


				//���ĵ�
				temp_coefficient_element = temp3 - temp5 - temp6 - temp7 - temp8 - temp9 - temp10 - temp11 - temp12 ;

				m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
				m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
				m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
				m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
				++count_has_get ;

				//�ұߵ�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 1 , 0)) //�ұߵ����
				{
					if(true == flag_velocity_oneOrder) //ʹ���ٶȵ�һ�׽���
					{
						velocity = GetVelocity_PML_MappingToNormal(ModelAs , i+1 , j) ;
						if(velocity <= 0.0f)
							cout<<"ERROR!!!"<<endl ;
						temp4 = temp4_4/velocity/velocity ;
					}
					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp4 + temp5 + temp9 - temp11/2.0f - temp12/2.0f ;
					else
						temp_coefficient_element = temp4 + temp5 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 1 , 0) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

				//��ߵ�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , -1 , 0)) //��ߵ����
				{
					if(true == flag_velocity_oneOrder) //ʹ���ٶȵ�һ�׽���
					{
						velocity = GetVelocity_PML_MappingToNormal(ModelAs , i-1 , j) ;
						if(velocity <= 0.0f)
							cout<<"ERROR!!!"<<endl ;
						temp4 = temp4_4/velocity/velocity ;
					}

					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp4 + temp6 + temp10 - temp11/2.0f - temp12/2.0f ;
					else
						temp_coefficient_element = temp4 + temp6 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , -1 , 0) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

				//�ϱߵ�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 1)) //�ϱߵ����
				{
					if(true == flag_velocity_oneOrder) //ʹ���ٶȵ�һ�׽���
					{
						velocity = GetVelocity_PML_MappingToNormal(ModelAs , i , j+1) ;
						if(velocity <= 0.0f)
							cout<<"ERROR!!!"<<endl ;
						temp4 = temp4_4/velocity/velocity ;
					}

					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp4 + temp7 - temp9/2.0f - temp10/2.0f + temp11 ;
					else
						temp_coefficient_element = temp4 + temp7 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 1) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

				//�±�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 0 , -1)) //�±ߵ����
				{
					if(true == flag_velocity_oneOrder) //ʹ���ٶȵ�һ�׽���
					{
						velocity = GetVelocity_PML_MappingToNormal(ModelAs , i , j-1) ;
						if(velocity <= 0.0f)
							cout<<"ERROR!!!"<<endl ;
						temp4 = temp4_4/velocity/velocity ;
					}

					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp4 + temp8 - temp9/2.0f - temp10/2.0f + temp12 ;
					else
						temp_coefficient_element = temp4 + temp8 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , -1) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

				//���ϱߵ�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 1 , 1)) //���ϱߵ����
				{
					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp9/2.0f + temp11/2.0f ;
					else
						temp_coefficient_element = temp9 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 1 , 1) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

				//���±ߵ�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , -1 , -1)) //���±ߵ����
				{
					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp10/2.0f + temp12/2.0f ;
					else
						temp_coefficient_element = temp10 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , -1 , -1) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

				//���ϱߵ�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , -1 , 1)) //���ϱߵ����
				{
					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp10/2.0f + temp11/2.0f ;
					else
						temp_coefficient_element = temp11 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , -1 , 1) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

				//���±ߵ�
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 1 , -1)) //���±ߵ����
				{
					if(true == flag_press_oneOrder)
						temp_coefficient_element = temp9/2.0f + temp12/2.0f ;
					else
						temp_coefficient_element = temp12 ;

					m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
					m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 1 , -1) ;
					m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
					m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
					++count_has_get ;
				}

			}
		}
	}

	/*****�ǻ��������ײ��***************************************************************************************************************************/

	if((false == m_SquareGride_HighOrderErr_Flag) && (Two_Order == m_NormalGride_Diff_Order))
	{
		cout<<"�ǻ��������ײ�֣�δ����ERROR!!!"<<endl ;
	}

	/*****�ǻ�������Ľײ��***************************************************************************************************************************/

	if((false == m_SquareGride_HighOrderErr_Flag) && (Four_Order == m_NormalGride_Diff_Order))
	{
		cout<<"�ǻ�������Ľײ�֣�δ����ERROR!!!"<<endl ;
	}

	m_Mumps.nz = count_has_get ; //�������õķ����Ԫ����Ŀ
}

/////////ʵ�֣����ذ����ŷŵߵ������PML�����ٶ�ģ��������һ������������е�λ��(��1��ʼ)/////////////////////////////////////////////////////////////
int CFDInversion::GetIndex_InColumnStoredModel_AddPML(int column , int row , int offset_c , int offset_r) const 
{
	column += offset_c ;
	row += offset_r ;


	{
	    int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
		int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;
	
		if((column < 0) || (row < 0) || (column > total_Column-1) || (row > total_Row-1)) //���������PML�����޶���һ����ľ��η�Χ
			return -2 ;
     }


	{ //�ж�λ�����������Ƿ���磬������������ر��³��Ȳ�һ
		int tempRow = m_NumPML_Down + m_NumPML_Up ; //�����Ӧ�ĵߵ�ģ�����PML����������    

		if(column<m_NumPML_Left)//�����PML����
			tempRow += m_InitalNonzeroCoordinates[0] ;
		else
			if(column<m_NumPML_Left + m_GrideNumX) //�����������
				tempRow += m_InitalNonzeroCoordinates[column-m_NumPML_Left] ;
			else
				tempRow += m_InitalNonzeroCoordinates[m_GrideNumX-1] ;

		if(row > tempRow) //�����˿������ķ�Χ����δ�����޶��Ĵ�ľ���
			return -1 ;
	}

	

	return m_UsedTotalGrideNum_PreColumn_AddPML[column] + row +1 ; //��1��ԭ������Ҫ����һ����1��ʼ������
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////������־//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////