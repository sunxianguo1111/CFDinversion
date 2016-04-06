
#include "CFDInversion.h"


CFDInversion::CFDInversion()
{
	/***对速度模型的基本信息进行赋值**************************************/

	m_GrideNumX = GrideNumX ;
	m_GrideNumZ = GrideNumZ ;

	m_GrideSpaceX = GrideSpaceX ;
	m_GrideSpaceZ = GrideSpaceZ ;

	m_InitalSurfaceDataSource = InitalSurfaceDataSource ; //枚举类型，用configue.h文件中的起伏地表的来源的宏来进行定义 都来源于速度模型bin文件
	m_ObjectSurfaceDataSource = ObjectSurfaceDataSource ; //枚举类型，用configue.h文件中的起伏地表的来源的宏来进行定义

	
	cout<<SetVelocityModel(M_As_Inital)<<endl ;//设置初始速度模型一起起伏地表信息
	cout<<SetVelocityModel(M_As_Object)<<endl ;//设置目标速度模型一起起伏地表信息
	cout<<"初始速度模型和目标速度模型是否匹配："<<InitalModel_Matching_ObjectModel()<<endl ;


	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<CreateVelocityModelBin(M_As_Inital , "")<<endl ;
	cout<<CreateVelocityModelBin(M_As_Object , "")<<endl ;

	cout<<"是否是平地表："<<m_SurfaceSmooth_Flag<<endl ;

	cout<<m_InitalMaxV<<endl ;//输出初始速度模型的最大速度值
	cout<<m_InitalMinV<<endl ;
	cout<<m_ObjectMaxV<<endl ;
	cout<<m_ObjectMinV<<endl ;
	////////////////////////////////////////////////////////////////////////


	/***用于正演的基本信息****************************************************/

	m_TimeAll = TimeAll ;   //用configue.h文件中的总采样时间宏进行定义
	m_TimeStep = TimeStep ; //用configue.h文件中的采样步长宏进行定义

	m_SquareGride_HighOrderErr_Flag = SquareGride_HighOrderErr_Flag && (m_GrideSpaceX == m_GrideSpaceZ) ; //用configure.h文件中的正演时是否是使用高阶无穷小的正方网格的宏 和网格间隔进行定义
	m_NormalGride_Diff_Order = NormalGride_Diff_Order ; //用configure.h文件中的正演时正常网格所使用的差分阶次来进行定义
	if((Two_Order == m_NormalGride_Diff_Order) && (true == m_SquareGride_HighOrderErr_Flag))
		m_NormalGride_Diff_Order = Four_Order ; //对正演时正常网格所使用的差分阶次进行修正(利用正演时是否是使用高阶无穷小的正方网格来修正)


	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<"正常网格所用的阶次："<<2*m_NormalGride_Diff_Order<<endl ;
	////////////////////////////////////////////////////////////////////////


	/***************************************************PML层相关信息**********************************************************************************/

	m_NumPML_Left = NumPML_Left ; //用configure.h文件中的各方向PML层的层数的宏进行定义
	m_NumPML_Right = NumPML_Right ;
	m_NumPML_Up = NumPML_Up ;
	m_NumPML_Down = NumPML_Down ;

	m_MaxDampFactor_Left = MaxDampFactor_Left ; //用configure.h文件中的各方向PML层的最大衰减系数的宏进行定义
	m_MaxDampFactor_Right = MaxDampFactor_Right ;
	m_MaxDampFactor_Up = MaxDampFactor_Up ;
	m_MaxDampFactor_Down = MaxDampFactor_Down ;

	
	PrepareForForward() ; //对容器m_UsedTotalGrideNum_PreColumn_AddPML进行了赋值,
	                      //此容器用于存放在颠倒了速度模型并添加了PML层后前N列(不包括第N列)的总的网格点数目

	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<"添加PML层后的总的网格点数："<<m_UsedTotalGrideNum_PreColumn_AddPML[m_NumPML_Left + m_GrideNumX + m_NumPML_Right]<<endl ;
	////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<CreateVelocityModelBin_AddPML(M_As_Inital , "")<<endl ;
	cout<<CreateVelocityModelBin_AddPML(M_As_Object , "")<<endl ;
	////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<CreateDampFactorBin_AddPML()<<endl ;
	////////////////////////////////////////////////////////////////////////


	
	SetShotCoordsInformation() ; //设置炮点接收点及接收点集合的信息
	SetRecvCoordsInformation_RelateToShotCoords() ;
	SetRecvCoordsSetInformation() ; 

	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<m_Shot_Coords.size()<<"\t"<<m_Recv_Coords_RelateToShotCoords[0].size()<<"\t"<<m_Recv_Coords_Set.size()<<"\t"<<m_Recv_Coords_Set[m_Recv_Coords_Set.size()-1].index_rhs<<endl ;
	////////////////////////////////////////////////////////////////////////


	SetAngularFrequencyIndexInformation() ;//设置角频率点索引及衰减系数
	SetDampingConstsInformation_RelateToAngularFrequencyIndexs() ;

	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<m_AngularFrequencyIndex.size()<<"\t"<<m_DampingConsts_RelateToAngularFrequencyIndex[0].size()<<endl ;
	////////////////////////////////////////////////////////////////////////


	SetAssumeSource_SourceEstimateInformation() ;//设置源的信息，估计源系数的信息


	////////////////////////////////////////////////下面的代码只是用于测试
	cout<<m_ObjectAssumedSource_RelateToAngularFrequencyIndex.size()<<"\t"<<m_ObjectAssumedSource_RelateToAngularFrequencyIndex[0].size()<<endl ;
	cout<<m_InitalAssumedSourec_RelateToAngularFrequencyIndex.size()<<"\t"<<m_InitalAssumedSourec_RelateToAngularFrequencyIndex[0].size()<<endl ;
	cout<<m_SourceEstimateMulti_RelateToAngularFrequencyIndex.size()<<"\t"<<m_SourceEstimateMulti_RelateToAngularFrequencyIndex[0].size()<<endl ;
	cout<<m_NumPointTime_Used<<"\t"<<m_Rank_RelateToNumPointTime<<endl ;
	cout<<m_SinglePointFreq_Gap<<endl ;
	////////////////////////////////////////////////////////////////////////


	SetObjectObservationDataInformation() ;  //设置观测数据的容器空间及格式

	////////////////////////////////////////////////下面的代码只是用于测试
	//cout<<m_ObjectObservationData.size()<<"\t"<<m_ObjectObservationData[0].size()<<"\t"<<m_ObjectObservationData[239][239].size()<<"\t" \
	//	<<m_ObjectObservationData[0][0][0].size()<<endl ;
	////////////////////////////////////////////////////////////////////////



	LaplaceFourier_Inversion() ; //进行反演
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////设置速度模型及相应的起伏地表数据，判断初始、目标速度模型是否匹配///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////接口，对m_InitalVelocityModel、m_InitalNonzeroCoordinates及m_ObjectVelocityModel、m_ObjectNonzeroCoordinates进行处理

bool CFDInversion::SetVelocityModel(VelocityModel_UsedAs ModelAs)
{
	if(M_As_Inital == ModelAs) //设置初始速度模型
	{
		if(true == SetVelocityData_Inital())//设置初始速度模型
			return SetSurfaceData_Inital() ;//设置起伏地表信息
	}
	else//设置目标速度模型
	{
		if(true == SetVelocityData_Object())
			return SetSurfaceData_Object() ;
	}

	return false ;
}

/////接口，对m_SurfaceSmooth_Flag、m_InitalMaxV、m_InitalMinV、m_ObjectMaxV、m_ObjectMinV进行处理

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


/////////实现:从宏InitalBinName_Model定义的BIN文件中获得具体的模型速度值////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetVelocityData_Inital()
{
	fstream Read(InitalBinName_Model , fstream::in | fstream::binary) ; 

	if(!Read)
	{
		cout<<"打开宏InitalBinName_Model定义的文件("<<InitalBinName_Model<<")(用于读取初始速度模型)失败!!!"<<endl  ;
		return false ;
	}


	Read.seekg(0 , fstream::end) ;
	fstream::pos_type end_l = Read.tellg() ;
	Read.seekg(0 , fstream::beg) ;
	fstream::pos_type beg_l = Read.tellg() ;

	__int64 count_File = end_l-beg_l ; //文件大小

	if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
	{
		cout<<"宏InitalBinName_Model定义的文件("<<InitalBinName_Model<<"大小不能用来作为读取初始速度模型使用!!!"<<endl ;
		Read.close() ;
		return false ;
	}
		
	m_InitalVelocityModel.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ; //重新分配初始速度模型所占用的内存空间
	m_InitalNonzeroCoordinates.assign(m_GrideNumX , 0) ; //重新分配初始速度模型所对应的每列的起伏信息所占用的内存空间


	bool flag = false ; //用来标记在此列的读取中是否已经出现过非零值，若true则出现过，若false则没有出现过
	float tempVel = 0.0f ; //用来存放每次从文件中读取到的速度值

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		flag = false ;

		for(int j=0 ; j<m_GrideNumZ ; ++j)
		{
			Read.read((char*)(&tempVel) , sizeof(tempVel)) ;
			
			if(0.0f != tempVel) 
			{
				m_InitalVelocityModel[i][m_GrideNumZ-1-j] = tempVel ; //速度模型赋值，颠倒速度模型的每一列数据

				if(false == flag)
				{
					flag = true ;
					m_InitalNonzeroCoordinates[i] = m_GrideNumZ-1-j ;//设置起伏地表信息
				}
			}
		}
	}

	Read.close() ;

	cout<<"可以正常从宏InitalBinName_Model定义的文件("<<InitalBinName_Model<<")中读取初始速度模型，" ;
	return true ;
}

/////////实现:获得初始速度模型的起伏地表信息////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetSurfaceData_Inital()
{
	if(S_D_Same == m_InitalSurfaceDataSource)//如果初始速度模型的数据和起伏数据来自同一个BIN文件，则在此不做任何处理
	{
		cout<<"及起伏地表信息。"<<endl ;
		return true ;
	}
	else
	{
		fstream Read(InitalBinName_Surface , fstream::in | fstream::binary) ; 

		if(!Read)
		{
			cout<<"打开宏InitalBinName_Surface定义的文件("<<InitalBinName_Surface<<")(用于读取初始速度模型的起伏地表数据)失败!!!"<<endl  ;
			return false ;
		}

		Read.seekg(0 , fstream::end) ;
		fstream::pos_type end_l = Read.tellg() ;
		Read.seekg(0 , fstream::beg) ;
		fstream::pos_type beg_l = Read.tellg() ;

		__int64 count_File = end_l-beg_l ; //文件大小
		
		if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
		{
			cout<<"宏InitalBinName_Surface定义的文件("<<InitalBinName_Surface<<"大小不能用来作为读取初始速度模型起伏地表数据使用!!!"<<endl ;
			Read.close() ;
			return false ;
		}

		bool flag = false ; //用来标记此列是否已检查
		float tempVel = 0.0f ; //用来存放每次从文件中读取到的速度值

		for(int i=0 ; i<m_GrideNumX ; ++i)
		{
			flag = false ;

			for(int j=0 ; j<m_GrideNumZ ; ++j)
			{
				Read.read((char*)(&tempVel) , sizeof(tempVel)) ;

				if((false == flag) && (0.0f != tempVel))
				{
					flag = true ;

					if(m_GrideNumZ-1-j > m_InitalNonzeroCoordinates[i]) //起伏信息的地表高度高于已经读取的速度模型中的地表高度
						return false ;
					else
						m_InitalNonzeroCoordinates[i] = m_GrideNumZ-1-j ;//设置起伏地表信息
				}
			}
		}

		Read.close() ;
		cout<<"可以正常从宏InitalBinName_Surface定义的文件("<<InitalBinName_Surface<<")中读取初始速度模型的起伏信息。"<<endl ;
		return true ;
	}

}

/////////实现:从宏ObjectBinName_Model定义的BIN文件中获得具体的模型速度值////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetVelocityData_Object()
{
	fstream Read(ObjectBinName_Model , fstream::in | fstream::binary) ; 

	if(!Read)
	{
		cout<<"打开宏ObjectBinName_Model定义的文件("<<ObjectBinName_Model<<")(用于读取目标速度模型)失败!!!"<<endl  ;
		return false ;
	}

	Read.seekg(0 , fstream::end) ;
	fstream::pos_type end_l = Read.tellg() ;
	Read.seekg(0 , fstream::beg) ;
	fstream::pos_type beg_l = Read.tellg() ;

	__int64 count_File = end_l-beg_l ; //文件大小

	if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
	{
		cout<<"宏ObjectBinName_Model定义的文件("<<ObjectBinName_Model<<"大小不能用来作为读取目标速度模型使用!!!"<<endl ;
		Read.close() ;
		return false ;
	}

	m_ObjectVelocityModel.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ;//重新分配目标速度模型所占用的内存空间
	m_ObjectNonzeroCoordinates.assign(m_GrideNumX , 0) ; //重新分配目标速度模型所对应的每列的起伏信息所占用的内存空间

	bool flag = false ; //用来标记在此列的读取中是否已经出现过非零值，若true则出现过，若false则没有出现过
	float tempVel = 0.0f ; //用来存放每次从文件中读取到的速度值

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		flag = false ;

		for(int j=0 ; j<m_GrideNumZ ; ++j)
		{
			Read.read((char*)(&tempVel) , sizeof(tempVel)) ;
			
			if(0.0f != tempVel) 
			{
				m_ObjectVelocityModel[i][m_GrideNumZ-1-j] = tempVel ; //颠倒速度模型的每一列数据

				if(false == flag)
				{
					flag = true ;
					m_ObjectNonzeroCoordinates[i] = m_GrideNumZ-1-j ;
				}
			}
		}
	}

	Read.close() ;

	cout<<"可以正常从宏ObjectBinName_Model定义的文件("<<ObjectBinName_Model<<")中读取目标速度模型，" ;
	return true ;
}

/////////实现:获得目标速度模型的起伏地表信息////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFDInversion::SetSurfaceData_Object() 
{
	if(S_D_Same == m_ObjectSurfaceDataSource)//如果目标速度模型的数据和起伏数据来自同一个BIN文件，则在此不做任何处理
	{
		cout<<"及起伏地表信息。"<<endl ;
		return true ;
	}
	else
	{
		fstream Read(ObjectBinName_Surface , fstream::in | fstream::binary) ; 

		if(!Read)
		{
			cout<<"打开宏ObjectBinName_Surface定义的文件("<<ObjectBinName_Surface<<")(用于读取目标速度模型的起伏地表数据)失败!!!"<<endl  ;
			return false ;
		}

		Read.seekg(0 , fstream::end) ;
		fstream::pos_type end_l = Read.tellg() ;
		Read.seekg(0 , fstream::beg) ;
		fstream::pos_type beg_l = Read.tellg() ;

		__int64 count_File = end_l-beg_l ; //文件大小
		if(count_File != m_GrideNumX*m_GrideNumZ*sizeof(float))
		{
			cout<<"宏ObjectBinName_Surface定义的文件("<<ObjectBinName_Surface<<"大小不能用来作为读取目标速度模型起伏地表数据使用!!!"<<endl ;
			Read.close() ;
			return false ;
		}

		bool flag = false ; //用来标记此列是否已检查
		float tempVel = 0.0f ; //用来存放每次从文件中读取到的速度值

		for(int i=0 ; i<m_GrideNumX ; ++i)
		{
			flag = false ;

			for(int j=0 ; j<m_GrideNumZ ; ++j)
			{
				Read.read((char*)(&tempVel) , sizeof(tempVel)) ;

				if((false == flag) && (0.0f != tempVel))
				{
					flag = true ;

					if(m_GrideNumZ-1-j > m_ObjectNonzeroCoordinates[i]) //起伏信息的地表高度高于已经读取的速度模型中的低表高度
						return false ;
					else
						m_ObjectNonzeroCoordinates[i] = m_GrideNumZ-1-j ;
				}
			}
		}

		Read.close() ;

		cout<<"可以正常从宏ObjectBinName_Surface定义的文件("<<ObjectBinName_Surface<<")中读取目标速度模型的起伏信息。"<<endl ;
		return true ;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////生成速度模型的Bin文件/////////////////////////////////////////////////////////////////////////
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
		cout<<"创建写"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"中数据到Bin文件("<<FileName<<")失败!!!"<<endl ;
		return false ;
	}

	float tempVel = 0.0f ; //用来存放每一个要写的速度值
	int tempRow = 0 ; //用来存放某一列中速度模型的开始非零值的行号(从0开始)

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

	cout<<"可以正常写"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"中数据到Bin文件("<<FileName<<")。"<<endl ;
	return true ;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////完成正演前的基本数据的初始化//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////接口，对m_UsedTotalGrideNum_PreColumn_AddPML进行处理

void CFDInversion::PrepareForForward()
{
	//对容器m_UsedTotalGrideNum_PreColumn_AddPML进行处理
	//      用于存放在颠倒了速度模型并添加了PML层后前N列(不包括第N列)的总的网格点数目
	m_UsedTotalGrideNum_PreColumn_AddPML.assign(m_NumPML_Left + m_GrideNumX + m_NumPML_Right+1 , 0) ;

	int temp_Column = 0 ; //添加了PML层的列，在未添加PML层的列中的映射列

	for(int i=1 ; i<m_NumPML_Left + m_GrideNumX + m_NumPML_Right + 1 ; ++i)
	{
		temp_Column = i-1 ; //列号减1，这是由于容器下标为N的地方存放的是从下标为0到下标为N-1中的所有网格点

		if(temp_Column < m_NumPML_Left) //在左侧PML层中
			temp_Column = 0 ; 
		else
			if(temp_Column < m_NumPML_Left + m_GrideNumX) //在正常区域
				temp_Column -= m_NumPML_Left ;
			else
				temp_Column = m_GrideNumX - 1 ; //在右侧PML层中

		m_UsedTotalGrideNum_PreColumn_AddPML[i] = m_UsedTotalGrideNum_PreColumn_AddPML[i-1] + \
												m_NumPML_Up + m_InitalNonzeroCoordinates[temp_Column] +1 + m_NumPML_Down ;

	}

	m_NumUsedElementPoint_AddPML = m_UsedTotalGrideNum_PreColumn_AddPML[m_NumPML_Left + m_GrideNumX + m_NumPML_Right] ;
}

/////接口，设置所有的炮点的坐标信息，也即对m_Shot_Coords进行处理

void CFDInversion::SetShotCoordsInformation()
{
	int num_shot = m_GrideNumX/2 +1 ;

	m_Shot_Coords.assign(num_shot , Point_x_z_rhsIndex()) ; //在此处设置了240个炮点

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i)  //先获得在速度模型未颠倒，也未添加PML层的模型中的炮点坐标信息(坐标是从0开始计算)
	{
		m_Shot_Coords[i].index_x = 2*i ;
		m_Shot_Coords[i].index_z = 0 ;	
	}

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //将信息转换到速度模型颠倒，并添加了PML层的模型中的炮点坐标信息(坐标从0开始计算)
	{
		m_Shot_Coords[i].index_z = m_InitalNonzeroCoordinates[m_Shot_Coords[i].index_x] - m_Shot_Coords[i].index_z + m_NumPML_Down ;
		m_Shot_Coords[i].index_x += m_NumPML_Left ;
	}

}

/////接口，设置炮点所对应的所有接收点的信息(每个炮点所对应的接收点放在一个Vector中)，也即对m_Recv_Coords_RelateToShotCoords进行处理

void CFDInversion::SetRecvCoordsInformation_RelateToShotCoords()
{
	int num_recv = m_GrideNumX/2 +1 ;
	m_Recv_Coords_RelateToShotCoords.assign(m_Shot_Coords.size() , vector<Point_x_z_rhsIndex>(num_recv , Point_x_z_rhsIndex())) ; //每个炮点设置240个接收点

	//先获得在速度模型未颠倒，也未添加PML层的模型中的相对于每个炮点的接收点的坐标信息(坐标是从0开始计算)
	for(int i=0 ; i<m_Recv_Coords_RelateToShotCoords.size() ; ++i) //炮点索引
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //炮点所对应的接收点的索引
		{
			m_Recv_Coords_RelateToShotCoords[i][j].index_x = 2*j ;
			m_Recv_Coords_RelateToShotCoords[i][j].index_z = 0 ; 
		}
	}

	//将信息转换到速度模型颠倒，并添加了PML层的模型中的相对于每个炮点的接收点的坐标信息(坐标从0开始计算)
	for(int i=0 ; i<m_Recv_Coords_RelateToShotCoords.size() ; ++i) //炮点索引
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //炮点所对应的接收点的索引
		{
			m_Recv_Coords_RelateToShotCoords[i][j].index_z = m_InitalNonzeroCoordinates[m_Recv_Coords_RelateToShotCoords[i][j].index_x] - \
																m_Recv_Coords_RelateToShotCoords[i][j].index_z + m_NumPML_Down ;
			m_Recv_Coords_RelateToShotCoords[i][j].index_x += m_NumPML_Left ;
			
		}
	}
}

/////接口，设置所有炮点所对的所有接收点坐标所组成的集合，也即对m_Recv_Coords_Set进行处理

void CFDInversion::SetRecvCoordsSetInformation() 
{
	m_Recv_Coords_Set.clear() ; //清空内容

	for(int i=0 ; i<m_Recv_Coords_RelateToShotCoords.size() ; ++i) //炮点索引
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //炮点所对应的接收点的索引
		{
			if(0 == count(m_Recv_Coords_Set.begin() , m_Recv_Coords_Set.end() , m_Recv_Coords_RelateToShotCoords[i][j])) //也即没有存放过此接收点
			{
				m_Recv_Coords_Set.push_back(m_Recv_Coords_RelateToShotCoords[i][j]) ;
				m_Recv_Coords_Set[m_Recv_Coords_Set.size()-1].index_rhs = m_Recv_Coords_Set.size()-1 ;
			}

			//若存放过此点则不做处理
		}
	}
}

/////接口，设置所使用的所有衰减系数，也即对m_DampingConsts进行处理

void CFDInversion::SetAngularFrequencyIndexInformation()
{
	int iiw[] ={7,10,14,19,30,39,50};  
		//{10.07,16.01,25.4,40.5,64.38,102.4};//{14.336,21.7,32.97,50.05,75.94,115.18,174.7};//{14.336,23.26,37.76,61.31,99.57};// {14.336,21.3,31.5,47.104,68.96,105.55};//{14.336,17.2,20.4,24.5,29.9,35.6,43.0,51.6,62.26,74.96,90.11,106.49};//{14.3,17.2,20.9,24.6,29.9,35.6,43.0,51.2,62.3,75.0,90.1};//{4.6,5.5,6.6,8.0,9.55,11.5,13.8,16.6,19.9,24.0,31.7,34.6,41.6,50.0}; //{8 , 12 , 16 , 23 , 32} ;//{8 , 10 , 12 , 15 , 18 , 21 , 26 , 31}  ; //   //选取的频率点
	
	m_AngularFrequencyIndex.assign(7, 0) ;  //在此处设置7个角频率点索引

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i)
	{
		m_AngularFrequencyIndex[i] = iiw[i] ;
	}
}

/////接口，设置衰减系数所对应的所有角频率点索引(每个衰减系数所对应的角频率点索引放在一个Vector中)，也即对m_AngularFrequency_Indexs_RelateToDampingConsts进行处理

void CFDInversion::SetDampingConstsInformation_RelateToAngularFrequencyIndexs()
{
	float iconstant[] = {14 , 12 , 10 , 8 , 6 , 4 , 2} ;
	m_DampingConsts_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<float>()) ; //角频率点索引个数

	for(int i=0 ; i<m_DampingConsts_RelateToAngularFrequencyIndex.size() ; ++i)
	{
		m_DampingConsts_RelateToAngularFrequencyIndex[i].assign(1 , 0.0f) ;//每个角频率点索引设置1个衰减系数

		for(int j=0 ; j<m_DampingConsts_RelateToAngularFrequencyIndex[i].size() ; ++j)
		{
			m_DampingConsts_RelateToAngularFrequencyIndex[i][j] = 0.0f ; //iconstant[j] ;
		}
	}
}

/////接口，设置所使用到的目标数据的源数据，初始数据的源数据，对初始数据的源数据进行修正的源估计乘子
/////也即对m_ObjectAssumedSource_RelateToDampingConsts、m_InitalAssumedSourec_RelateToDampingConsts、m_SourceEstimateMulti_RelateToDampingConsts进行处理
/////并且处理m_NumPointTime_Used(所使用的正演时刻点数)、m_Rank_RelateToNumPointTime(正演时刻点数所对应的2的阶数)、m_SinglePointFreq_Gap(两个角频率索引点的频率间隔)

void CFDInversion::SetAssumeSource_SourceEstimateInformation()
{
	m_ObjectAssumedSource_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<complex<float> >()) ; //角频率点索引数个
	m_InitalAssumedSourec_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<complex<float> >()) ; //角频率点索引数个
	m_SourceEstimateMulti_RelateToAngularFrequencyIndex.assign(m_AngularFrequencyIndex.size() , vector<complex<float> >()) ; //角频率点索引数个

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i)
	{
		m_ObjectAssumedSource_RelateToAngularFrequencyIndex[i].assign(m_DampingConsts_RelateToAngularFrequencyIndex[i].size() , complex<float>()) ; //每个角频率点索引所对应的衰减系数的个数
		m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i].assign(m_DampingConsts_RelateToAngularFrequencyIndex[i].size() , complex<float>()) ; //每个角频率点索引所对应的衰减系数的个数

		m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i].assign(m_DampingConsts_RelateToAngularFrequencyIndex[i].size() , complex<float>(1.0f , 0.0f)) ; //每个角频率点索引所对应的衰减系数的个数
	}

	int nt = (int)(m_TimeAll/m_TimeStep) + 1 ;  //原来的总的采样点数

	int ntnew = 1 ; //最后的所用的点数
	int r = 0 ;     //最后的所用的点数是2的多少阶数

	while(ntnew < nt)
	{
		ntnew<<=1 ;
		++r ;
	}

	int r_add = 0 ; //相对于上面的部分要添加的阶数
	r_add = 0 ;

	ntnew<<=r_add ;
	r += r_add ;

	m_NumPointTime_Used = ntnew ;
	m_Rank_RelateToNumPointTime = r ;

	m_SinglePointFreq_Gap = 1.0f/m_TimeStep/m_NumPointTime_Used ;

	complex<float> * source_Time = new complex<float>[m_NumPointTime_Used] ; 
	for(int i=0 ; i<m_NumPointTime_Used ; ++i) //获得震源的位移信息
	{
		source_Time[i] = GetSourceDisplacement_InTimeDomain(i) ;
	}

	complex<float> * source_Time_Damped = new complex<float>[m_NumPointTime_Used] ; 
	complex<float> * source_Freq_Damped = new complex<float>[m_NumPointTime_Used] ; 

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i)  //角频率点索引
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

/////接口，设置目标观测数据的Vector信息

void CFDInversion::SetObjectObservationDataInformation()
{
	m_ObjectObservationData.assign(m_Shot_Coords.size() , vector< vector< vector< complex<float> > > >()); //设置炮点数目
	
	for(int i=0 ; i<m_ObjectObservationData.size() ; ++i)
	{
		m_ObjectObservationData[i].assign(m_Recv_Coords_RelateToShotCoords[i].size() , vector< vector< complex<float> > >()) ; //设置接收点数目

		for(int j=0 ; j<m_ObjectObservationData[i].size() ; ++j)
		{
			m_ObjectObservationData[i][j].assign(m_AngularFrequencyIndex.size() , vector< complex<float> >()) ; //角频率点索引数目

			for(int k=0 ; k<m_ObjectObservationData[i][j].size() ; ++k)
			{
				m_ObjectObservationData[i][j][k].assign(m_DampingConsts_RelateToAngularFrequencyIndex[k].size() , complex<float>()) ;//衰减系数及内部具体值为0.0f
			}
		}
	}
}

/////////实现，获得震源在时域中任意时刻的位移值(未衰减)/////////////////////////////////////////////////////////////////////////////////////////////////

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
///////////////////////////////////////////////////////生成添加了PML层后的速度模型的Bin文件/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////接口，默认假设，目标速度模型与初始速度模型匹配/////////////////////////////////////////////////////////////////////////////////////////////////////

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
		cout<<"创建写"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"中添加了PML层后数据到Bin文件("<<FileName<<")失败!!!"<<endl ;
		return false ;
	}

	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;

	float tempVel = 0.0f ; //用来存放每一个要写的速度值

	for(int i=0 ; i<total_Column ; ++i)
	{
			for(int j=0 ; j<total_Row ; ++j)
			{
				tempVel = GetVelocity_PML_MappingToNormal(ModelAs , i , total_Row-1-j) ;

				if(tempVel < 0.0f)
				{
					cout<<"写"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"中添加了PML层后数据到Bin文件("<<FileName<<")失败!!!"<<endl ;
					Write.close() ;
					return false ;
				}

				Write.write((char*)(&tempVel) , sizeof(tempVel)) ;
			}
	}

	Write.close() ;

	cout<<"可以正常写"<<((M_As_Inital == ModelAs)?"m_InitalVelocityModel":"m_ObjectVelocityModel")<<"中添加了PML层后数据到Bin文件("<<FileName<<")。"<<endl ;
	return true ;
}

/////////实现，获得速度模型颠倒并添加了PML层后的模型中索引所映射的速度值////////////////////////////////////////////////////////////////////////////////

float CFDInversion::GetVelocity_PML_MappingToNormal(VelocityModel_UsedAs ModelAs , int column , int row) const
{
	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;

	if((column < 0) || (row < 0) || (column > total_Column-1) || (row > total_Row-1)) //超出了添加PML层所限定的一个大的矩形范围
		return -1.0f ;

	int tempRow = 0 ; //只存放相应的正常网格中的列网格点数(也即去除了PML层中的点数)

	if(column<m_NumPML_Left)//在左侧PML层中
	{
		tempRow = m_InitalNonzeroCoordinates[0] ;
		column = 0 ;
	}
	else
		if(column<m_NumPML_Left + m_GrideNumX) //在正常网格点
		{
			tempRow = m_InitalNonzeroCoordinates[column-m_NumPML_Left] ;
			column -= m_NumPML_Left ;
		}
		else
		{
			tempRow = m_InitalNonzeroCoordinates[m_GrideNumX-1] ;
			column = m_GrideNumX-1 ;
		}
			

	if(row > m_NumPML_Down + tempRow + m_NumPML_Up) //说明在添加了PML层后的起伏地表之上，且没有超出添加PML层后限定的一个大的矩形范围
		return 0.0f ;


	row -= m_NumPML_Down ; //由于m_NumPML_Down是通常我们所说的未颠倒模型时的上面的PML层数，故在颠倒后就变成了上面

	if(row < 0) //说明在颠倒速度模型后的上PML层中
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
///////////////////////////////////////////////////////生成添加了PML层后的衰减系数的Bin文件/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////接口，默认假设，目标速度模型与初始速度模型匹配/////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////实现，生成在相应的颠倒了速度模型并添加了PML层后，单方向、单种点类型衰减系数的Bin文件///////////////////////////////////////////////////////////

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
		cout<<"创建写添加了PML层的衰减系数文件：("<<FileName<<")失败!!!"<<endl ;
		return false ;
	}

	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Down + m_GrideNumZ +  m_NumPML_Up ;

	float tempDampFactor = 0.0f ; //用来存放每一个要写的衰减系数


	for(int i=0 ; i<total_Column ; ++i)
	{
		for(int j=0 ; j<total_Row ; ++j)
		{
			tempDampFactor = GetDampFactor_PML(i , total_Row-1-j ,  flag_RowOrientation , flag_IntegralPoint , false) ;
			
			if(tempDampFactor <= -2.0f)
			{
				cout<<"写添加PML层后的衰减系数文件：("<<FileName<<")失败!!!"<<endl ;
				Write.close() ;
				return false ;
			}

			if(tempDampFactor == -1.0f) //在此，如此赋值只是为了使得到的图便于观察而已
				tempDampFactor = -10.0f ; 

			Write.write((char*)(&tempDampFactor) , sizeof(tempDampFactor)) ;
		}
	}
	
	Write.close() ;

	cout<<"可以正常写添加PML层后的衰减系数文件：("<<FileName<<")。"<<endl ;
	return true ;
}

///////////////实现的实现，获得在相应的颠倒了速度模型并添加了PML层后，任意网格点索引在不同方向、整半点上的衰减系数//////////////////////////////////////

float CFDInversion::GetDampFactor_PML(int column , int row , bool flag_RowOrientation , bool flag_IntegralPoint , bool flag_File) const 
{
	int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
	int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;

	if((column < 0) || (row < 0) || (column > total_Column-1) || (row > total_Row-1)) //超出了添加PML层所限定的一个大的矩形范围
		return -2.0f ;


	int tempRow = m_NumPML_Down + m_NumPML_Up ; //存放相应的颠倒模型添加PML层后网格点数

	if(column<m_NumPML_Left)//在左侧PML层中
		tempRow += m_InitalNonzeroCoordinates[0] ;
	else
		if(column<m_NumPML_Left + m_GrideNumX) //在正常网格点
			tempRow += m_InitalNonzeroCoordinates[column-m_NumPML_Left] ;
		else
			tempRow += m_InitalNonzeroCoordinates[m_GrideNumX-1] ;

	if(row > tempRow) //超出了可索引的范围，但未超出限定的大的矩形
		return -1.0f ;

	if(true == flag_RowOrientation) //是一个行方向，也即X方向
	{
		if(column < m_NumPML_Left) //在左侧PML层中
		{
			if(true == flag_IntegralPoint) //是一个整点的衰减系数索引
				return m_MaxDampFactor_Left*(1-cos(PI/2*(m_NumPML_Left - column)/m_NumPML_Left)) ;
			else
				return m_MaxDampFactor_Left*(1-cos(PI/2*(m_NumPML_Left - column -0.5)/m_NumPML_Left)) ;
		}


		column -= m_NumPML_Left ;
		if(column < m_GrideNumX) //在正常区域
		{
			if((false == flag_IntegralPoint) && (m_GrideNumX-1 == column)) //如果是正常区域最右点索引右半点
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
		if(true == flag_IntegralPoint) //是一个整点的衰减系数索引
			return m_MaxDampFactor_Right*(1 - cos(PI/2*(column+1)/m_NumPML_Right)) ;
		else
			if((m_NumPML_Right-1 == column) && (false == flag_File))
				return -1.0f ;
			else
				return m_MaxDampFactor_Right*(1 - cos(PI/2*(column+1+0.5)/m_NumPML_Right)) ;

	}
	else //是一个列方向，也即Z方向
	{
		if(row < m_NumPML_Down) //在颠倒模型并添加PML层后的上面PML层中
		{
			if(true == flag_IntegralPoint)//是一个整点的衰减系数索引
				return m_MaxDampFactor_Down*(1 - cos(PI/2*(m_NumPML_Down - row)/m_NumPML_Down)) ;
			else
				return m_MaxDampFactor_Down*(1 - cos(PI/2*(m_NumPML_Down - row -0.5)/m_NumPML_Down)) ;
		}

		row -= m_NumPML_Down ;
		tempRow -= m_NumPML_Down + m_NumPML_Up ; //只标记此列中正常网格点的数目
		if(row <= tempRow) //在正常区域
		{
			if((false == flag_IntegralPoint) && (tempRow == row)) //如果是正常区域最下点索引下半点
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
		if(true == flag_IntegralPoint) //整点索引
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
//////////////////////////////////////////////////////////反演的具体函数////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CFDInversion::LaplaceFourier_Inversion() 
{

	GetObjectObservationData() ; //获得目标模型的观测数据

	m_Gradient_Vector.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ; //对梯度矩阵分配空间
	m_PseudoHessianDiag_Vector.assign(m_GrideNumX , vector<float>(m_GrideNumZ , 0.0f)) ; //对伪黑塞矩阵对角元分配空间

	fWrite_Step.open("StepLength.txt" , fstream::out) ;

	vector<int> vecLapaceDepthBegin(7 , 0) ;
	vecLapaceDepthBegin[0] = 158 ; //208 ;
	vecLapaceDepthBegin[1] = 153 ; //203 ;
	vecLapaceDepthBegin[2] = 146 ; //194 ;
	vecLapaceDepthBegin[3] = 135 ; //181 ;
	vecLapaceDepthBegin[4] = 113 ; //153 ;
	vecLapaceDepthBegin[5] = 53 ; //71 ;
	vecLapaceDepthBegin[6] = 0 ; //0 ;

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i) //角频率点索引
	{
		for(int j=0 ; j<m_DampingConsts_RelateToAngularFrequencyIndex[i].size() ; ++j) //角频率点相对应的衰减系数
		{
			laplace_Depth_Begain = 0 ; //vecLapaceDepthBegin[j] ;


			MumpsInital() ; //初始化

			complex<float> complex_freq = complex<float>(2*PI*m_AngularFrequencyIndex[i]*m_SinglePointFreq_Gap , -m_DampingConsts_RelateToAngularFrequencyIndex[i][j]) ;

			GetPseudoHessianDiagData(complex_freq , i , j , true) ; //获得黑塞矩阵及相应的源估计(false表示没有进行源估计)

			for(int k=0 ; k<15 ; ++k) //对于每一个衰减系数和角频率对，进行10次迭代
			{
				m_StepLength_Value_List.assign(5 , Array_Length(0.0f , 0.0f)) ; //分配五个元素的空间，初始化为0.0f,求步长

				GetGradientData(complex_freq , i , j) ; //在其中 获得了目标函数值，求梯度

				MakeGradientPretreatment() ; //对梯度进行预处理

				GetStepLength_Used(complex_freq , i , j) ;//求步长

				if((m_StepLength_Value_List[4].value < 0) || ((fabs(m_StepLength_Value_List[3].value - m_StepLength_Value_List[0].value)<0.0001f*m_StepLength_Value_List[0].value))) 
					break ;//即所求的结果无效

				StepChange_InitalVelocityModel(m_StepLength_Value_List[3].len) ;
				StepChange_GradientVector(m_StepLength_Value_List[3].len) ;


				Create_VelocityModel_GradientVector_HessianVector(i , j , k , 1) ;
				Create_VelocityModel_GradientVector_HessianVector(i , j , k , 2) ;
				Create_VelocityModel_GradientVector_HessianVector(i , j , k , 3) ;

			}

			MumpsEnd() ; //清除所占用的空间及资源
		}
	}

	fWrite_Step.close() ;

}

/////////实现，获得目标速度模型所对应的观测数据/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetObjectObservationData()
{
	//int num_shot = m_GrideNumX/2 ;

	////以下的代码用于从一个BIN文件中获得观测数据
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

	//for(long long i=0 ; i<num_shot ; ++i) //炮点序
	//{
	//	cout<<"第"<<i<<"炮观测数据"<<endl ;
	//	for(long long j=0 ; j<num_shot ; ++j) //接收点序
	//	{
	//		for(long long k=0 ; k<2000 ; ++k) //时间点序
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

	//		for(int m=0 ; m<m_AngularFrequencyIndex.size() ; ++m) //角频率点索引
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


	MumpsInital() ; //初始化

	int index = 0 ; //用来记录索引下标

	for(int i=0 ; i<m_AngularFrequencyIndex.size() ; ++i) //角频率点索引
	{
		for(int j=0 ; j<m_DampingConsts_RelateToAngularFrequencyIndex[i].size() ; ++j) //角频率点相对应的衰减系数
		{
			complex<float> complex_freq = complex<float>(2*PI*m_AngularFrequencyIndex[i]*m_SinglePointFreq_Gap , -m_DampingConsts_RelateToAngularFrequencyIndex[i][j]) ;

			Set_NRHS_Matrix() ; //设置炮点
			Set_Coefficient_Matrix(M_As_Object , complex_freq) ; //设置系数矩阵

			m_Mumps.job = 6;
			cmumps_c(&m_Mumps);

			if(0 != m_Mumps.info[1-1])
			{
				cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
			}

	#pragma omp parallel for private(index)
			for(int k=0 ; k<m_Shot_Coords.size() ; ++k) //炮点
			{
				for(int m=0 ; m<m_Recv_Coords_RelateToShotCoords[k].size() ; ++m) //接收点
				{
					index = k*m_NumUsedElementPoint_AddPML ; 
					index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_RelateToShotCoords[k][m].index_x , m_Recv_Coords_RelateToShotCoords[k][m].index_z , 0 , 0) -1 ;

					m_ObjectObservationData[k][m][i][j] = complex<float>(m_Mumps.rhs[index].r , m_Mumps.rhs[index].i)*m_ObjectAssumedSource_RelateToAngularFrequencyIndex[i][j] ;
				}
			}
		}
	}

	MumpsEnd() ; //清除所占用的空间及资源
}



/////////实现，获得伪黑塞矩阵对角元的数据///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetPseudoHessianDiagData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume)
{
	Set_NRHS_Matrix_Hessian() ; //重新设置方程组的个数，并对此时的方程组的右边部分进行赋值

	Set_Coefficient_Matrix(M_As_Inital , complex_freq) ; //设置系数矩阵

	m_Mumps.job = 6;
	cmumps_c(&m_Mumps);

	if(0 != m_Mumps.info[1-1])
	{
		cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
	}

	SourceEstimate(i_frequ_index , j_dampconst_index , flag_sourceAssume) ; //进行源估计


	//获得伪黑塞矩阵的对角元
	
	float velocity = 0.0f ; //存放速度值
	float b = 1.0f ; //0.6284f ; //中心点所占的权重
	complex<float> temp ; //用来表示其中的一个系数
	complex<float> res ; //保存中间结果

	int index_s = 0 ; //炮点的索引
	int index_r = 0 ; //接收点的索引
	int index_base = 0 ; //索引的基

	//新添加BEGIN
	complex<float> compTempBase ;
	compTempBase = 2.0f*complex_freq*complex_freq*b ;
	compTempBase *= m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index]* \
			m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ;

	//新添加END

#pragma omp parallel for private(velocity , temp , index_base , index_s , index_r , res)
	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			velocity = m_InitalVelocityModel[i][j] ; //获得此点的速度值

			//temp = 2.0f*complex_freq*complex_freq*b/velocity/velocity/velocity ;
			//修正为
			temp = compTempBase/velocity/velocity/velocity ;
			

			m_PseudoHessianDiag_Vector[i][j] = 0.0f ; //设置黑塞的初始值为0

			index_base = GetIndex_InColumnStoredModel_AddPML(i , j , m_NumPML_Left , m_NumPML_Down) -1 ;
			
			
			for(int k=0 ; k<m_Shot_Coords.size() ; ++k) //炮点
			{
				for(int m=0 ; m<m_Recv_Coords_Set.size() ; ++m) //接收点
				{
					index_s = k*m_NumUsedElementPoint_AddPML ;
					index_s += index_base ;
					index_r = (m+m_Shot_Coords.size())*m_NumUsedElementPoint_AddPML ;
					index_r += index_base ;


					//res = complex<float>(m_Mumps.rhs[index_s].r , m_Mumps.rhs[index_s].i)* \
					//	m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index]* \
					//	m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ; //炮点正演的结果

					//修正为
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

///////////////实现的实现，对特定的衰减部分进行源估计///////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::SourceEstimate(int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume)
{
	if(false == flag_sourceAssume) //不估计
	{
		m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] = 1.0f ;
		return ;
	}

	complex<float> numerator(0.0f , 0.0f) ; //用来保存分子
	complex<float> denominator(0.0f , 0.0f) ; //用来保存分母

	complex<float> temp(0.0f , 0.0f) ;

	int index = 0 ; //用来记录索引下标

	for(int k=0 ;  k<m_Shot_Coords.size() ; ++k) //炮点
	{
		for(int m=0 ; m<m_Recv_Coords_RelateToShotCoords[k].size() ; ++m) //接收点
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



/////////实现，获得梯度矩阵/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetGradientData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index)
{
	Set_NRHS_Matrix() ; //设置炮点
	Set_Coefficient_Matrix(M_As_Inital , complex_freq) ; //设置系数矩阵

	m_Mumps.job = 6;
	cmumps_c(&m_Mumps);

	if(0 != m_Mumps.info[1-1])
	{
		cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
	}

	vector<complex<float> > forwardData(m_NumUsedElementPoint_AddPML*m_Mumps.nrhs , complex<float>(0.0f , 0.0f)) ; //用来存放正演的结果

	for(int i=0 ; i<forwardData.size() ; ++i) //赋值
	{
		forwardData[i] = complex<float>(m_Mumps.rhs[i].r , m_Mumps.rhs[i].i)* \
			m_InitalAssumedSourec_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index]* \
			m_SourceEstimateMulti_RelateToAngularFrequencyIndex[i_frequ_index][j_dampconst_index] ;
	}

	memset(m_Mumps.rhs , 0 , sizeof(CMUMPS_COMPLEX)*m_Mumps.nrhs*m_NumUsedElementPoint_AddPML) ; //对NRHS的内容进行全部赋为0

	int index = 0 ; //用来记录索引下标
	complex<float> temp ; 

	m_StepLength_Value_List[0].len = 0.0f ;
	m_StepLength_Value_List[0].value = 0.0f ; //为求目标函数做准备

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //炮点
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //接收点
		{
			index = i*m_NumUsedElementPoint_AddPML ; 
			index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_RelateToShotCoords[i][j].index_x , m_Recv_Coords_RelateToShotCoords[i][j].index_z , 0 , 0) -1 ;

			temp = conj(forwardData[index] - m_ObjectObservationData[i][j][i_frequ_index][j_dampconst_index]) ; //波场残差
			
			m_Mumps.rhs[index].r = temp.real() ;
			m_Mumps.rhs[index].i = temp.imag() ;

			m_StepLength_Value_List[0].value += (conj(temp)*temp).real() ;
		}
	}

	cout<<"第0个步长："<<m_StepLength_Value_List[0].len<<"\t\t\t"<<m_StepLength_Value_List[0].value<<endl ;

	m_Mumps.job = 3; //用3(performs the solution)的原因是，此时=1所对应的performs the analysis已经做了，=2所对应的performs the factorization也已经做了，
						//若仍使用=6，则会调用JOB=1,2和3，It must be preceded by a call to MUMPS with JOB=-1 on the same instance
						//故可以减少计算量
	cmumps_c(&m_Mumps);


	int index_base = 0 ; //索引的基
	float velocity = 0.0f ; //存放速度值
	float b = 1.0f ; //0.6284f ; //中心点所占的权重
	complex<float> res ;

#pragma omp parallel for private(velocity , temp , index_base , index , res)
	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		for(int j=laplace_Depth_Begain ; j<=m_InitalNonzeroCoordinates[i] ; ++j)
		{
			velocity = m_InitalVelocityModel[i][j] ; //获得此点的速度值
			temp = 2.0f*complex_freq*complex_freq*b/velocity/velocity/velocity ;

			index_base = GetIndex_InColumnStoredModel_AddPML(i , j , m_NumPML_Left , m_NumPML_Down) -1 ;
			
			m_Gradient_Vector[i][j] = 0.0f ;

			for(int k=0 ; k<m_Shot_Coords.size() ; ++k) //炮点
			{	
				index = k*m_NumUsedElementPoint_AddPML ;
				index += index_base ;

				res = forwardData[index]*temp*complex<float>(m_Mumps.rhs[index].r , m_Mumps.rhs[index].i) ;

				m_Gradient_Vector[i][j] += res.real() ;
			}

		}
	}


}

/////////实现，用伪黑塞矩阵对角元对所得的梯度矩阵进行预处理/////////////////////////////////////////////////////////////////////////////////////////////

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



/////////实现，获得最佳的修正步长///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CFDInversion::GetStepLength_Used(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index)
{
	cout<<"相对应的目标函数值："<<m_StepLength_Value_List[0].value<<endl ;
	float stepLength_Max = StepLength_MAX_Valid(0) ; 
	cout<<"最大步长为："<<stepLength_Max<<endl ;

	fWrite_Step<<"频率点索引"<<i_frequ_index<<endl ;
	fWrite_Step<<"相对应的目标函数值："<<m_StepLength_Value_List[0].value<<endl ;
	fWrite_Step<<"最大步长为："<<stepLength_Max<<endl<<endl  ;


	float length ; //步长
	float objectiveValue ; //相应的目标函数值

	///////////////////////////////////////////黄金分割搜索法

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

	////新增
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
	//	StepChange_InitalVelocityModel(leftSelectPoint) ; //修正速度模型
	//	objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	//	StepChange_InitalVelocityModel(-leftSelectPoint)  ; //修正为原来的速度模型

	//	cout<<"第"<<tempCount++<<"测试搜索区域的左边的选择点"<<endl ;
	//	fWrite_Step<<"第"<<tempCount-1<<"测试搜索区域的左边的选择点"<<endl ;
	//	fWrite_Step<<"\t左边搜索点"<<leftSelectPoint ;
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
	//StepChange_InitalVelocityModel(m_StepLength_Value_List[4].len) ; //修正速度模型
	//objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	//StepChange_InitalVelocityModel(-m_StepLength_Value_List[4].len)  ; //修正为原来的速度模型
	//m_StepLength_Value_List[4].value = objectiveValue ;


	//fWrite_Step<<"右边搜索点"<<m_StepLength_Value_List[4].len ;
	//fWrite_Step<<"\t"<<objectiveValue<<endl ;
	//
	//tempCount++ ;

	//if(m_StepLength_Value_List[4].len - m_StepLength_Value_List[3].len > 0.001*stepLength_Max)
	//{
	//	
	//	StepChange_InitalVelocityModel(m_StepLength_Value_List[2].len) ; //修正速度模型
	//	objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	//	StepChange_InitalVelocityModel(-m_StepLength_Value_List[2].len)  ; //修正为原来的速度模型
	//	m_StepLength_Value_List[2].value = objectiveValue ;


	//	fWrite_Step<<"右边界点"<<m_StepLength_Value_List[2].len ;
	//	fWrite_Step<<"\t"<<objectiveValue<<endl ;


	//	tempCount++ ;
	//}
	//else
	//{
	//	cout<<"不再进行黄金分割法搜索"<<endl ;
	//	fWrite_Step<<"不再进行黄金分割法搜索"<<endl ;
	//}
	//	

	//

	///////////////////////
	////rightSelectPoint = m_StepLength_Value_List[0].len + goldenRatio*(stepLength_Max - m_StepLength_Value_List[1].len) ;

	////m_StepLength_Value_List[2].len = rightSelectPoint ;
	////StepChange_InitalVelocityModel(m_StepLength_Value_List[2].len) ; //修正速度模型
	////objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	////StepChange_InitalVelocityModel(-m_StepLength_Value_List[2].len)  ; //修正为原来的速度模型
	////m_StepLength_Value_List[2].value = objectiveValue ;

	////leftSelectPoint = m_StepLength_Value_List[1].len + goldenRatioRemainder*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;
	////rightSelectPoint = m_StepLength_Value_List[1].len + goldenRatio*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;
	////StepChange_InitalVelocityModel(leftSelectPoint) ; //修正速度模型
	////objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	////StepChange_InitalVelocityModel(-leftSelectPoint)  ; //修正为原来的速度模型

	////m_StepLength_Value_List[3].len = leftSelectPoint ;
	////m_StepLength_Value_List[3].value = objectiveValue ;

	////StepChange_InitalVelocityModel(rightSelectPoint) ; //修正速度模型
	////objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	////StepChange_InitalVelocityModel(-rightSelectPoint)  ; //修正为原来的速度模型

	////m_StepLength_Value_List[4].len = rightSelectPoint ;
	////m_StepLength_Value_List[4].value = objectiveValue ;

	////int tempCount = 4 ;

	//while(1)
	//{
	//	if(m_StepLength_Value_List[4].len - m_StepLength_Value_List[3].len < 0.001*stepLength_Max)
	//		break ;

	//	//小值在左侧
	//	if(m_StepLength_Value_List[3].value < m_StepLength_Value_List[4].value)
	//	{
	//		m_StepLength_Value_List[2] = m_StepLength_Value_List[4] ;
	//		m_StepLength_Value_List[4] = m_StepLength_Value_List[3] ;
	//		leftSelectPoint = m_StepLength_Value_List[1].len + goldenRatioRemainder*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;

	//		StepChange_InitalVelocityModel(leftSelectPoint) ; //修正速度模型
	//		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	//		StepChange_InitalVelocityModel(-leftSelectPoint)  ; //修正为原来的速度模型

	//		m_StepLength_Value_List[3].len = leftSelectPoint ;
	//		m_StepLength_Value_List[3].value = objectiveValue ;

	//		fWrite_Step<<"区域相左收敛"<<endl ;
	//		fWrite_Step<<"\t左搜索点"<<leftSelectPoint ;
	//		fWrite_Step<<"\t"<<objectiveValue<<endl ;

	//	}
	//	else
	//	{
	//		m_StepLength_Value_List[1] = m_StepLength_Value_List[3] ;
	//		m_StepLength_Value_List[3] = m_StepLength_Value_List[4] ;
	//		rightSelectPoint = m_StepLength_Value_List[1].len + goldenRatio*(m_StepLength_Value_List[2].len - m_StepLength_Value_List[1].len) ;

	//		StepChange_InitalVelocityModel(rightSelectPoint) ; //修正速度模型
	//		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值
	//		StepChange_InitalVelocityModel(-rightSelectPoint)  ; //修正为原来的速度模型

	//		m_StepLength_Value_List[4].len = rightSelectPoint ;
	//		m_StepLength_Value_List[4].value = objectiveValue ;

	//		fWrite_Step<<"区域相右收敛"<<endl ;
	//		fWrite_Step<<"\t右搜索点"<<leftSelectPoint ;
	//		fWrite_Step<<"\t"<<objectiveValue<<endl ;
	//	}

	//	cout<<"第"<<tempCount++<<"次迭代搜索"<<endl ;
	//	fWrite_Step<<"第"<<tempCount-1<<"次迭代搜索"<<endl ;
	//}

	//if(m_StepLength_Value_List[3].value > m_StepLength_Value_List[4].value)
	//{
	//	m_StepLength_Value_List[3] = m_StepLength_Value_List[4] ;
	//}

	//if(m_StepLength_Value_List[3].value > m_StepLength_Value_List[0].value)
	//	m_StepLength_Value_List[4].value = -1.0f ;
	//else
	//	m_StepLength_Value_List[4].value = 1.0f ;

	//cout<<"结果的步长："<<m_StepLength_Value_List[3].len<<endl ;
	//cout<<"相对应的目标函数值："<<m_StepLength_Value_List[3].value<<endl ;
	//fWrite_Step<<"结果的步长："<<m_StepLength_Value_List[3].len<<endl ;
	//fWrite_Step<<"相对应的目标函数值："<<m_StepLength_Value_List[3].value<<endl ;
	//fWrite_Step<<"\n\n"<<endl ;
	//return ;

	////////////////////////////////////////上面的是黄金分割搜索
	

	

	float test_len = 0.009f ; //0.03f ; //0.009f ; //0.03f ; //测试步长

	for(int i=0 ; i<5+1 ; ++i)
	{
		if(5 == i) //也即无法找到相应的步长
		{
			m_StepLength_Value_List[4].value = -1.0f ;
			return ;
		}
			

		length = test_len*stepLength_Max ;

		StepChange_InitalVelocityModel(length) ; //修正速度模型

		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值

		StepChange_InitalVelocityModel(-length)  ; //修正为原来的速度模型

		if(objectiveValue < m_StepLength_Value_List[0].value)
		{
			m_StepLength_Value_List[1].len = length ;
			m_StepLength_Value_List[1].value = objectiveValue ;
			cout<<"第一个有效步长："<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;

			fWrite_Step<<"第一个有效步长："<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;

			break ;

		}
		else
			test_len /= 2.0f ;

		cout<<"第一个测试步长："<<length<<"\t\t\t"<<objectiveValue<<endl ;
		fWrite_Step<<"第一个测试步长："<<length<<"\t\t\t"<<objectiveValue<<endl<<endl ;
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

		StepChange_InitalVelocityModel(length) ; //修正速度模型

		objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值

		StepChange_InitalVelocityModel(-length)  ; //修正为原来的速度模型

		if(objectiveValue < m_StepLength_Value_List[1].value)
		{
			m_StepLength_Value_List[1].len = length ;
			m_StepLength_Value_List[1].value = objectiveValue ;

			test_len *= 2.0f ;

			cout<<"修正第一个有效步长："<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;
			fWrite_Step<<"修正第一个有效步长："<<m_StepLength_Value_List[1].len<<"\t\t\t"<<m_StepLength_Value_List[1].value<<endl ;
		}
		else
		{
			m_StepLength_Value_List[2].len = length ;
			m_StepLength_Value_List[2].value = objectiveValue ;
			cout<<"第二个有效步长："<<m_StepLength_Value_List[2].len<<"\t\t\t"<<m_StepLength_Value_List[2].value<<endl ;
			fWrite_Step<<"第二个有效步长："<<m_StepLength_Value_List[2].len<<"\t\t\t"<<m_StepLength_Value_List[2].value<<endl<<endl ;
			break ;
		}
	}


	float alpha[3],cost[3];
	cout<<"最大步长为："<<stepLength_Max<<endl ;
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

	StepChange_InitalVelocityModel(alphamin) ; //修正速度模型

	objectiveValue = GetObjectiveFunction_Value(complex_freq , i_frequ_index , j_dampconst_index) ; //获得此时的目标函数值

	StepChange_InitalVelocityModel(-alphamin)  ; //修正为原来的速度模型

	m_StepLength_Value_List[3].value = objectiveValue ;

	cout<<"抛物线法求出的步长："<<m_StepLength_Value_List[3].len<<"\t\t\t"<<m_StepLength_Value_List[3].value<<endl ;
	fWrite_Step<<"抛物线法求出的步长："<<m_StepLength_Value_List[3].len<<"\t\t\t"<<m_StepLength_Value_List[3].value<<endl<<endl ;

	if((m_StepLength_Value_List[3].len > stepLength_Max) || (objectiveValue > m_StepLength_Value_List[0].value))
		m_StepLength_Value_List[4].value = -1.0f ;
	else
		m_StepLength_Value_List[4].value = 1.0f ;
}

///////////////实现的实现，求解所允许的最大步长/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
			if(m_Gradient_Vector[i][j] > 1e-20) //也即>0
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

///////////////实现的实现，对初始模型按所选用的步长进行修正/////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////实现的实现，获得初始速度模型与目标速度模型之间的目标函数值//、///////////////////////////////////////////////////////////////////////////

float CFDInversion::GetObjectiveFunction_Value(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index)
{
	Set_NRHS_Matrix() ; //设置炮点
	Set_Coefficient_Matrix(M_As_Inital , complex_freq) ; //设置系数矩阵

	m_Mumps.job = 6;
	cmumps_c(&m_Mumps);

	if(0 != m_Mumps.info[1-1])
	{
		cout<<"The call to MUMPS was failed , ERROR!!!"<<endl ;
	}

	complex<float> temp ;
	int index = 0 ;

	float ret = 0.0f ;

	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //炮点
	{
		for(int j=0 ; j<m_Recv_Coords_RelateToShotCoords[i].size() ; ++j) //接收点
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



/////////实现，对用伪黑塞对角元矩阵预处理过的梯度矩阵，乘以步长以确定最终的初始速度修正量///////////////////////////////////////////////////////////////

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

/////////实现，对修正后的初始速度模型、修正所用的伪黑塞矩阵、修正所使用的最终初始速度模型修正量保存到BIN文件中//////////////////////////////////////////

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

	_itoa_s(i_dampconst_index+1 , buffer , sizeof(buffer) , 10) ; //把n转化为十进制数存放到buffer中

	FileName += buffer ;
	FileName += "_" ;

	_itoa_s(j_frequ_index+1 , buffer , sizeof(buffer) , 10) ; //把n转化为十进制数存放到buffer中

	FileName += buffer ;
	FileName += "_" ;

	_itoa_s(iter+1 , buffer , sizeof(buffer) , 10) ; //把n转化为十进制数存放到buffer中

	FileName += buffer ;
	FileName += "_" ;

	FileName += ".bin" ;

	fstream Write(FileName.c_str() , fstream::out | fstream::binary) ;

	if(!Write)
	{
		cout<<"创建Bin文件("<<FileName<<")失败!!!"<<endl ;
		return false ;
	}

	float tempValue = 0.0f ; //用来存放每一个要写的速度值
	int tempRow = 0 ; //用来存放某一列中速度模型的开始非零值的行号(从0开始)

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

	cout<<"可以正常写数据到Bin文件("<<FileName<<")。"<<endl ;
	return true ;
	
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////解方程的相关接口及实现////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//接口
void CFDInversion::MumpsInital()
{
	m_Mumps.job = -1 ; //Initalizes an instance of the package
	m_Mumps.par = 1 ;  //Host is involved in factorization/solve phases
	m_Mumps.sym = 0 ;  //A is unsymmetric

	cmumps_c(&m_Mumps) ;

	int number_Element_Normal = 0 ; //用来存放未添加PML层时的网格点的数目

	for(int i=0 ; i<m_GrideNumX ; ++i)
	{
		number_Element_Normal += m_InitalNonzeroCoordinates[i] +1 ;
	}

	int number_NozeroMatrixCoeff = 0 ; //系数矩阵中的非零元的数目
	
	int multi = 0 ; //用来标志一个点所用到的网格点数目

	if(true == m_SquareGride_HighOrderErr_Flag) //用混合网格
	{
		multi = 9 ;
		number_NozeroMatrixCoeff = m_NumUsedElementPoint_AddPML*multi ;

		cout<<"使用混合网格四阶差分"<<endl ;
	}
	else
	{
		if(Two_Order == m_NormalGride_Diff_Order)
		{
			multi = 5 ;
			number_NozeroMatrixCoeff = m_NumUsedElementPoint_AddPML*multi ;

			cout<<"使用普通网格二阶差分"<<endl ;
		}
		else
		{
			multi = 9 ;
			number_NozeroMatrixCoeff = m_NumUsedElementPoint_AddPML*5 + number_Element_Normal*4 ;
			cout<<"使用普通网格四阶差分"<<endl ;
		}
	}

	m_Mumps.irn = new int[number_NozeroMatrixCoeff]() ; //为记录行坐标的数组分配空间
	m_Mumps.jcn = new int[number_NozeroMatrixCoeff]() ; //为记录列坐标的数组分配空间
	m_Mumps.a   = new CMUMPS_COMPLEX[number_NozeroMatrixCoeff] ; //为记录在上两个数组中相同下标中的行、列坐标所指定的矩阵中的值的数组分配空间

	m_Mumps.n = m_NumUsedElementPoint_AddPML ;  //指定一个方程组中的方程的个数
	m_Mumps.nrhs = m_Shot_Coords.size() ; //相同系数的方程组的个数
	m_Mumps.lrhs = m_NumUsedElementPoint_AddPML ; //指定每一个右边项的长度
	m_Mumps.rhs = new CMUMPS_COMPLEX[m_NumUsedElementPoint_AddPML*m_Shot_Coords.size()] ; //给相同系数的方程组的右边常数项分配空间
	

	m_Mumps.icntl[20-1] = 0 ; //The right-hand side must be given in dense form in the structure componet RHS
	m_Mumps.icntl[21-1] = 0 ; //The solution vector will be assembled and stored in the structure component RHS
	m_Mumps.icntl[7-1] = 0 ; //Approximate Mininum Degree(AMD) is used
	m_Mumps.icntl[8-1] = 7 ; //Simultaneous row and column iterative scaling based on ... ;
								//computed during the numerical factorization phase
	m_Mumps.icntl[14-1] = 30 ; //It corresponds to the percentage increase in the estimated working space

	m_Mumps.icntl[1-1] = -1 ; //抑制显示信息
	m_Mumps.icntl[2-1] = -1 ; 
	m_Mumps.icntl[3-1] = -1 ;
	m_Mumps.icntl[4-1] = 0 ;
}

//接口
void CFDInversion::MumpsEnd()
{
	m_Mumps.job = -2 ; //Destroys an instance of the package

	cmumps_c(&m_Mumps) ;
	delete [] m_Mumps.irn ;
	delete [] m_Mumps.jcn ;
	delete [] m_Mumps.a ;
	delete [] m_Mumps.rhs ;
}

//接口
void CFDInversion::Set_NRHS_Matrix()
{
	if(m_Mumps.nrhs != m_Shot_Coords.size()) //也即炮点数目不匹配时
	{
		delete [] m_Mumps.rhs ;
		m_Mumps.nrhs = m_Shot_Coords.size() ; //重新设置炮点数目
		m_Mumps.rhs = new CMUMPS_COMPLEX[m_NumUsedElementPoint_AddPML*m_Shot_Coords.size()]() ; //重新分配空间
	}

	memset(m_Mumps.rhs , 0 , sizeof(CMUMPS_COMPLEX)*m_Mumps.nrhs*m_NumUsedElementPoint_AddPML) ; //对NRHS的内容进行全部赋为0

	int index = 0 ; //用来指示其在NRHS中的下标索引

#pragma omp parallel for private(index)
	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //对于每一个炮点
	{
		index = i*m_NumUsedElementPoint_AddPML ;
		index += GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0)-1 ; //变成一个索引从0开始的索引

		if(GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0) <= -1)
			cout<<"ERROR!!!"<<endl ;
		m_Mumps.rhs[index].r = 1.0f ;
		m_Mumps.rhs[index].i = 0.0f ;
	}
}

//接口
void CFDInversion::Set_NRHS_Matrix_Hessian()
{
	if(m_Mumps.nrhs != m_Shot_Coords.size() + m_Recv_Coords_Set.size()) //也即炮点数目不匹配时
	{
		delete [] m_Mumps.rhs ;
		m_Mumps.nrhs = m_Shot_Coords.size()+ m_Recv_Coords_Set.size() ; //重新设置炮点数目
		m_Mumps.rhs = new CMUMPS_COMPLEX[m_NumUsedElementPoint_AddPML*(m_Shot_Coords.size()+ m_Recv_Coords_Set.size())]() ; //重新分配空间
	}

	memset(m_Mumps.rhs , 0 , sizeof(CMUMPS_COMPLEX)*m_Mumps.nrhs*m_NumUsedElementPoint_AddPML) ; //对NRHS的内容进行全部赋为0

	int index = 0 ; //用来指示其在NRHS中的下标索引

#pragma omp parallel for private(index)
	for(int i=0 ; i<m_Shot_Coords.size() ; ++i) //对于每一个炮点
	{
		index = i*m_NumUsedElementPoint_AddPML ;
		index += GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0)-1 ; //变成一个索引从0开始的索引

		if(GetIndex_InColumnStoredModel_AddPML(m_Shot_Coords[i].index_x , m_Shot_Coords[i].index_z , 0 , 0) <= -1)
			cout<<"ERROR!!!"<<endl ;
		m_Mumps.rhs[index].r = 1.0f ;
		m_Mumps.rhs[index].i = 0.0f ;
	}

#pragma omp parallel for private(index)
	for(int i=m_Shot_Coords.size() ; i<m_Shot_Coords.size()+m_Recv_Coords_Set.size() ; ++i) //对于每一个接收点
	{
		index = i*m_NumUsedElementPoint_AddPML ;
		index += GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_x , m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_z , 0 , 0)-1 ; //变成一个索引从0开始的索引

		if(GetIndex_InColumnStoredModel_AddPML(m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_x , m_Recv_Coords_Set[i-m_Shot_Coords.size()].index_z , 0 , 0) <= -1)
			cout<<"ERROR!!!"<<endl ;
		m_Mumps.rhs[index].r = 1.0f ;
		m_Mumps.rhs[index].i = 0.0f ;
	}
}


//接口
void CFDInversion::Set_Coefficient_Matrix(VelocityModel_UsedAs ModelAs , complex<float> complex_frequency)
{
	int count_has_get = 0 ; //用来标记已经获得的系数的个数

	/*****混合网格四阶差分****************************************************************************************************************************/
	if(true ==m_SquareGride_HighOrderErr_Flag)
	{
		bool flag_velocity_oneOrder = true ; //为true时表示：使用速度的一阶近似，否则不使用速度的一阶近似

		bool flag_press_oneOrder = true ; //为true时表示，使用压强的一阶近似，否则不使用压强的一阶近似

		bool flag_TransDamp_approximate = true ; //为true时表示，使用原始各个方向的衰减系数直接近似，否则使用原始不同方向的加权均值来近似

		float a = 0.5461f ; //非旋转格点所占的比重
		float b = 0.6284f ; //中心格点所占的比重
		float c = (1-b)/4 ; //中心格点的上、下、左、右四个格点所占的比重

		float damp_x = 0.0f ; //中心点在X方向的衰减系数

		float damp_x_b1 = 0.0f ; //中心点右边点在的衰减系数
		float damp_x_b_1 = 0.0f ; //中心点左半点的衰减系数

		float damp_z = 0.0f ; //中心点在Z方向的衰减系数

		float damp_z_b1 = 0.0f ; //中心点在上半点的衰减系数
		float damp_z_b_1 = 0.0f ; //中心点在下半点的衰减系数


		float damp_tx = 0.0f ; //旋转坐标后中心点在X方向的衰减系数

		float damp_tx_b1 = 0.0f ; //旋转坐标后中心点右半点的衰减系数
		float damp_tx_b_1 = 0.0f ; //旋转坐标后中心点左半点的衰减系数

		float damp_tz = 0.0f ; //旋转坐标后中心点在Z方向的衰减系数

		float damp_tz_b1 = 0.0f ; //旋转坐标后中心点上半点的衰减系数
		float damp_tz_b_1 = 0.0f ; //旋转坐标后中心点下半点的衰减系数


		float temp1 = a/m_GrideSpaceX/m_GrideSpaceX ;
		float temp2 = (1-a)/m_GrideSpaceX/m_GrideSpaceX/2 ;
		complex<float> temp3_3 = b*complex_frequency*complex_frequency ;
		complex<float> temp4_4 = c*complex_frequency*complex_frequency ;

		float velocity = 0.0f ;//用来存放此中心点上的速度值

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


		complex<float> temp_coefficient_element(0.0f , 0.0f) ; //用来存放临时的元素系数
		complex<float> Unit_Imaginary_Trans = complex<float>(0.0f , 1.0f)/complex_frequency ; //定义单位纯虚数与复频率的商


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

				if(-1.0f < GetDampFactor_PML(i , j , true , false)) //右半点的索引存在
					damp_x_b1 = GetDampFactor_PML(i , j , true , false) ;
				else
					damp_x_b1 = GetDampFactor_PML(i-1 , j , true , false) ;


				if(-1.0f < GetDampFactor_PML(i-1 , j , true , false)) //左半点的索引存在
					damp_x_b_1 = GetDampFactor_PML(i-1 , j , true , false) ;
				else
					damp_x_b_1 = GetDampFactor_PML(i , j , true , false) ;


				damp_z = GetDampFactor_PML(i , j , false , true) ;

				if(-1.0f < GetDampFactor_PML(i , j , false , false)) //上半点的索引存在
					damp_z_b1 = GetDampFactor_PML(i , j , false , false) ;
				else
					damp_z_b1 = GetDampFactor_PML(i , j-1 , false , false) ;


				if(-1.0f < GetDampFactor_PML(i , j-1 , false , false)) //下半点的索引存在
					damp_z_b_1 = GetDampFactor_PML(i , j-1 , false , false) ;
				else
					damp_z_b_1 = GetDampFactor_PML(i , j , false , false) ;


				//对于旋转后的衰减系数进行设置

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


				//中心点
				temp_coefficient_element = temp3 - temp5 - temp6 - temp7 - temp8 - temp9 - temp10 - temp11 - temp12 ;

				m_Mumps.irn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
				m_Mumps.jcn[count_has_get] = GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 0) ;
				m_Mumps.a[count_has_get].r = temp_coefficient_element.real() ;
				m_Mumps.a[count_has_get].i = temp_coefficient_element.imag() ;
				++count_has_get ;

				//右边点
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 1 , 0)) //右边点存在
				{
					if(true == flag_velocity_oneOrder) //使用速度的一阶近似
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

				//左边点
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , -1 , 0)) //左边点存在
				{
					if(true == flag_velocity_oneOrder) //使用速度的一阶近似
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

				//上边点
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 0 , 1)) //上边点存在
				{
					if(true == flag_velocity_oneOrder) //使用速度的一阶近似
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

				//下边
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 0 , -1)) //下边点存在
				{
					if(true == flag_velocity_oneOrder) //使用速度的一阶近似
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

				//右上边点
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 1 , 1)) //右上边点存在
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

				//左下边点
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , -1 , -1)) //左下边点存在
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

				//左上边点
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , -1 , 1)) //左上边点存在
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

				//右下边点
				if(-1 < GetIndex_InColumnStoredModel_AddPML(i , j , 1 , -1)) //右下边点存在
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

	/*****非混合网格二阶差分***************************************************************************************************************************/

	if((false == m_SquareGride_HighOrderErr_Flag) && (Two_Order == m_NormalGride_Diff_Order))
	{
		cout<<"非混合网格二阶差分，未定义ERROR!!!"<<endl ;
	}

	/*****非混合网格四阶差分***************************************************************************************************************************/

	if((false == m_SquareGride_HighOrderErr_Flag) && (Four_Order == m_NormalGride_Diff_Order))
	{
		cout<<"非混合网格四阶差分，未定义ERROR!!!"<<endl ;
	}

	m_Mumps.nz = count_has_get ; //设置所得的非零的元素数目
}

/////////实现，返回按列排放颠倒并添加PML层后的速度模型中任意一个网格点在其中的位置(从1开始)/////////////////////////////////////////////////////////////
int CFDInversion::GetIndex_InColumnStoredModel_AddPML(int column , int row , int offset_c , int offset_r) const 
{
	column += offset_c ;
	row += offset_r ;


	{
	    int total_Column = m_NumPML_Left + m_GrideNumX + m_NumPML_Right ;
		int total_Row    = m_NumPML_Up + m_GrideNumZ + m_NumPML_Down ;
	
		if((column < 0) || (row < 0) || (column > total_Column-1) || (row > total_Row-1)) //超出了添加PML层所限定的一个大的矩形范围
			return -2 ;
     }


	{ //判断位置纵向索引是否出界，纵向由于起伏地表导致长度不一
		int tempRow = m_NumPML_Down + m_NumPML_Up ; //存放相应的颠倒模型添加PML层后网格点数    

		if(column<m_NumPML_Left)//在左侧PML层中
			tempRow += m_InitalNonzeroCoordinates[0] ;
		else
			if(column<m_NumPML_Left + m_GrideNumX) //在正常网格点
				tempRow += m_InitalNonzeroCoordinates[column-m_NumPML_Left] ;
			else
				tempRow += m_InitalNonzeroCoordinates[m_GrideNumX-1] ;

		if(row > tempRow) //超出了可索引的范围，但未超出限定的大的矩形
			return -1 ;
	}

	

	return m_UsedTotalGrideNum_PreColumn_AddPML[column] + row +1 ; //加1的原因在于要返回一个从1开始的索引
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////结束标志//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////