/********************************************************************************************************************************************************
*版本：CFDInversion_Laplace_TheoryModel_Parallel
*编写者:细雨潜行
*********************************************************************************************************************************************************
*所做修正部分
***用m_AngularFrequencyIndex替换了m_DampingConsts
***用m_DampingConsts_RelateToAngularFrequencyIndex替换了m_AngularFrequency_Indexs_RelateToDampingConsts
*****也即：把原来的衰减系数-->角频率点索引结构转换为了角频率点索引-->衰减系数结构(相应的也修正了设置它们相关信息的函数)
*
***用m_ObjectAssumedSource_RelateToAngularFrequencyIndex替换了m_ObjectAssumedSource_RelateToDampingConsts
***用m_InitalAssumedSourec_RelateToAngularFrequencyIndex替换了m_InitalAssumedSourec_RelateToDampingConsts
***用m_SourceEstimateMulti_RelateToAngularFrequencyIndex替换了m_SourceEstimateMulti_RelateToDampingConsts
*****也即：把原来的衰减系数-->角频率点索引结构转换为了角频率点索引-->衰减系数结构(也修正了设置它们信息的相关函数)
*
***修改了m_ObjectObservationData中数据的存放结构由原来的炮点-->接收点-->衰减系数-->角频率点索引转换到炮点-->接收点-->角频率点索引-->衰减系数
*****并修正了设置其信息的相关函数
*
***
*****总之：是把原来的衰减系数-->角频率点索引结构转换到了角频率点索引-->衰减系数的结构
***
*********************************************************************************************************************************************************/

/********************************************************************************************************************************************************
*版本:CFDInversion_Laplace_TheoryModel
*编写者:细雨潜行
*********************************************************************************************************************************************************
*用途:用于Laplace域理论数据的波形反演
*********************************************************************************************************************************************************
*
*速度模型的来源：在这里设置为都是来自BIN文件，由宏定义InitalBinName_Model、ObjectBinName_Model来标定初始速度模型和目标速度模型BIN文件所在的位置
*起伏地表的数据来源：在这里可以是来源于速度模型的BIN文件，也可以是来源于其它的BIN文件，由枚举类型SurfaceData_Source来确定
*********************分别用宏InitalSurfaceDataSource和ObjectSurfaceDataSource来标记初始速度模型和目标速度模型的起伏地表数据来源
*
*********************************************************************************************************************************************************
*所得的结果存放的目录由宏SaveFileDirectory定义为".\\Result\\"
*********************************************************************************************************************************************************
*
*宏定义正演所用的总采样时间(TimeAll)、采样步长(TimeStep)
*宏定义是否在正演时使用高阶无穷小的正方网格SquareGride_HighOrderErr_Flag(若为false则不用，若为true则会在构造函数中通过对网格间隔进行判断来重新确定)
*宏定义在正演中正常网格所使用的差分阶次NormalGride_Diff_Order(若为Two_Order，则会根据是否在正演时使用高阶无穷小的正方网格来进行一次修正)
*
*********************************************************************************************************************************************************
*
*宏定义在正演时在上、下、左、右四个方向上添加的PML层的层数(NumPML_Left、NumPML_Right、NumPML_Up、NumPML_Down)(上下指通常意义上的上下，而非颠倒模型后的上下)
*宏定义在正演时在上、下、左、右四个方向上添加的PML层的最大衰减系数(MaxDampFactor_Left、MaxDampFactor_Right、MaxDampFactor_Up、MaxDampFactor_Down)
*
*********************************************************************************************************************************************************
*
*结构体：Point_x_z_rhsIndex
*用此来存放炮点及接收点在X方向和Z方向的索引，而且有时还会存放所有接收点所形成的集合中任意一个接收点在其中的索引值
*注意：其中的==重载操作符只判断index_x、index_z而不判断index_rhs
*
*m_Shot_Coords中存放炮点的坐标信息(在颠倒并添加PML层中的数据信息)，并且其中的index_rhs有的构造函数所赋的0值
*m_Recv_Coords_RelateToShotCoords中存放炮点所对应的接收点的坐标信息(在颠倒并添加PML层中的数据信息)，并且其中的index_rhs有的构造函数所赋的0值
*m_Recv_Coords_Set中存放炮点所对应的接收点的所有坐标所组成的集合，其中index_rhs为其在其中的下标索引
*
*m_DampingConsts中存放所选取的衰减系数信息
*m_AngularFrequency_Indexs_RelateToDampingConsts中存放衰减系数所对应的所有角频率点索引
*
*m_NumPointTime_Used表示所使用的正演时刻点数
*m_Rank_RelateToNumPointTime表示正演时刻点数所对应的2的阶数
*m_SinglePointFreq_Gap表示两个角频率索引点的频率间隔
*
*m_ObjectAssumedSource_RelateToDampingConsts用来放衰减常数所对应的每个角频率点上的观测数据的估计源(在此是自己定义的源)
*m_InitalAssumedSourec_RelateToDampingConsts用来放衰减常数所对应的每个角频率点上的初始数据的估计源(在此是自己定义的源)
*m_SourceEstimateMulti_RelateToDampingConsts用来放衰减常数所对应的每个角频率点上的初始数据的估计源的源估计乘子，结合初始数据的估计源可得源数据
*
***注意：我们的源数据是对于单个衰减系数所对应的单个角频率点上所有炮点而言的，也即是所有的炮点的源数据都是一样的，在此也就没有区分炮序********************
*
*
***修正步长的方法
*
*********************************************************************************************************************************************************/


#ifndef CONFIGURE_H
#define CONFIGURE_H


//宏定义所选用模型的X、Z方向的网格点数目(无量纲)
#define GrideNumX 60		
#define GrideNumZ 60


//宏定义所选用模型的X、Z方向的网格点间隔(单位为米)
#define GrideSpaceX 25.0f
#define GrideSpaceZ 25.0f


//用一个枚举类型来标记起伏地表的来源是速度模型BIN文件还是其它的BIN文件
enum SurfaceData_Source
{
	S_D_Same , //起伏地表数据来源于速度模型BIN文件
	S_D_Other  //起伏地表数据来源于其它BIN文件
};

//宏定义初始速度模型及目标速度模型的起伏地表数据的来源
#define InitalSurfaceDataSource S_D_Same
#define ObjectSurfaceDataSource S_D_Same


//宏定义初始速度模型和目标速度模型所在BIN文件，这里的速度是按列存放，未进行上下颠倒

#define InitalBinName_Model "60x60Start.bin"
#define ObjectBinName_Model "60x60True.bin"



//宏定义当起伏地表数据存放在速度模型文件中时，此BIN文件的位置，这里的速度是按列存放，未进行上下颠倒
#define InitalBinName_Surface "Marmousi737230_qifu.bin"
#define ObjectBinName_Surface "Marmousi737230_qifu.bin"


//用一个枚举类型来标记速度模型是初始速度模型还是目标速度模型
enum VelocityModel_UsedAs
{
	M_As_Inital , //此速度模型是初始速度模型
	M_As_Object   //此速度模型是目标速度模型
};


//宏定义要保存的数据所在的目录
#define SaveFileDirectory  ".\\Result\\"


//宏定义总采样时间、采样步长
#define TimeAll 3.0f
#define TimeStep 0.004f


//宏定义在正演时是否是使用高阶无穷小的正方网格
#define SquareGride_HighOrderErr_Flag true


//用一个枚举类型来定义在正演中正常网格点所使用的差分阶数
enum Differential_Order
{
	Two_Order = 1 , //二阶差分，非混合网格中的5点
	Four_Order , //四阶差分，混合网格中的9点，或非混合网格中的9点
};

//宏定义在正演中正常网格所使用的差分阶次
#define NormalGride_Diff_Order Two_Order


//宏定义添加的各个方向的PML层的层数，在这里的上下是指我们通常所说的上下，而非颠倒速度模型后的上下
#define NumPML_Left  20
#define NumPML_Right 20
#define NumPML_Up    20
#define NumPML_Down  20

//宏定义PI
#define PI 3.1415926f

//宏定义添加的各个方向的PML层的最大衰减系数
#define MaxDampFactor_Left  90.0f
#define MaxDampFactor_Right 90.0f
#define MaxDampFactor_Up    90.0f
#define MaxDampFactor_Down  90.0f


//定义一个结构体，用此来存放炮点及接收点在X方向和Z方向的索引，而且有时还会存放所有接收点所形成的集合中任意一个接收点在其中的索引值
struct Point_x_z_rhsIndex
{
	int index_x ; //X方向的索引值，也即第几列(坐标从0开始计算)
	int index_z ; //Z方向的索引值，也即第几行
	int index_rhs ; //用来存放所有的接收点所形成的集合中，每一个接收点的索引值(从0开始)


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


//宏定义一个结构体
struct Array_Length
{
	float len ; //在原来基础上的步长
	float value ; //此时的目标函数值

public :
	Array_Length(float one , float two):len(one) , value(two){}
};


#endif