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
	int laplace_Depth_Begain ; //用来存放在进行Laplace深度计算时
	//反转速度模型后，开始计算的层数

public :
	/**********************************************************速度模型的常规信息**********************************************************************/

	bool m_SurfaceSmooth_Flag ; //用来标记是否是起伏地表(若true则为平地表，若为false则为起伏地表)

	int m_GrideNumX ; //速度模型沿X方向的网格点数(在此把起伏地表填充为非起伏地表)
	int m_GrideNumZ ; //速度模型沿Z方向的网格点数

	float m_GrideSpaceX ; //速度模型沿X方向的网格点间的间隔
	float m_GrideSpaceZ ; //速度模型沿Z方向的网格点间的间隔

private :
	/**********************************************************速度模型的详细信息**********************************************************************/

	vector< vector<float> > m_InitalVelocityModel ; //用来存放初始的速度模型，在内部按列存放，但进行了上下反转
	vector< vector<float> > m_ObjectVelocityModel ; //用来存放目标速度模型(也即用于正演获得观测数据的速度模型)

	vector< vector<float> > m_PseudoHessianDiag_Vector ; //用来存放伪黑塞矩阵的对角元
	vector< vector<float> > m_Gradient_Vector ;  //用来存放梯度方向

	vector<int> m_InitalNonzeroCoordinates ; //用来存放初始速度模型中，每一列的最后一个非零速度值的坐标(坐标从0开始计数)(速度模型是上下反转后的模型)
	vector<int> m_ObjectNonzeroCoordinates ; //用来存放目标速度模型中，每一列的最后一个非零速度值的坐标


	float m_InitalMaxV ; //初始速度模型中的最大速度值
	float m_InitalMinV ; //初始速度模型中的最小速度值

	float m_ObjectMaxV ; //目标速度模型中的最大速度值
	float m_ObjectMinV ; //目标速度模型中的最小速度值

	SurfaceData_Source m_InitalSurfaceDataSource ; //用一个枚举类型标记所用的初始速度模型的起伏地表数据是来自初始速度模型BIN文件还是其它BIN文件
	SurfaceData_Source m_ObjectSurfaceDataSource ; //用一个枚举类型标记所用的目标速度模型的起伏地表数据是来自目标速度模型BIN文件还是其它BIN文件


public :
	/**********************************************************用于正演的基本信息***********************************************************************/

	float m_TimeAll ; //总采样时间
	float m_TimeStep ; //采样步长

	int m_NumPointTime_Used ; //在正演中实际使用的正演的时间点数
	int m_Rank_RelateToNumPointTime ; //正演中实际使用的正演的时间点数相对应的是2的多少阶数
	float m_SinglePointFreq_Gap ; //正演时变到频域时的两个索引点之间的频率间隔


	bool m_SquareGride_HighOrderErr_Flag ; //用来标记在正演时是否是使用高阶无穷小的正方网格
	Differential_Order m_NormalGride_Diff_Order ; //用一个枚举类型来标记在正演过程中正常网格所使用的差分阶次


private :
	/**********************************************PML层相关信息、在PML层中的相关衰减系数**************************************************************/

	int m_NumPML_Left ; //在模型左侧添加的PML层的层数
	int m_NumPML_Right ; //在模型右侧添加的PML层的层数
	int m_NumPML_Up ; //在模型上侧添加的PML层的层数
	int m_NumPML_Down ; //在模型下侧添加的PML层的层数

	float m_MaxDampFactor_Left ; //模型左侧添加的PML层的最大衰减系数
	float m_MaxDampFactor_Right ; //模型右侧添加的PML层的最大衰减系数
	float m_MaxDampFactor_Up ; //模型上侧添加的PML层的最大衰减系数
	float m_MaxDampFactor_Down ; //模型下侧添加的PML层的最大衰减系数

	vector<int> m_UsedTotalGrideNum_PreColumn_AddPML ; //用于存放在颠倒了速度模型并添加了PML层后前N列(不包括第N列)的总的网格点数目
														//此容器的大小为m_GrideNumX + m_NumPML_Left + m_NumPML_Right+1
														//下标为N的地方存储的是颠倒速度模型添加PML层后，下标从0到N-1的列的所有网格点数
														//注意：vector<int>中的int可能在模型较大时溢出，可以换为 long long类型或__int64类型
														//注意：在本机上测试sizeof(int) == sizeof(long) == 4

	int m_NumUsedElementPoint_AddPML ; //用来存放添加了PML层后的总的网格点数目

public :
	/**********************************************************用于正演的详细信息***********************************************************************/

	vector<Point_x_z_rhsIndex> m_Shot_Coords ; //用来存放炮点的坐标信息，先设置的为未颠倒及添加PML层的模型中的坐标，后在其中转换到颠倒并添加PML层后的模型中的坐标

	vector< vector<Point_x_z_rhsIndex> > m_Recv_Coords_RelateToShotCoords ; //用来存放炮点所对应的接收点的坐标信息，先设置的为未颠倒及添加PML层的模型中的坐标，后在其中转换到颠倒并添加PML层后的模型中的坐标

	vector<Point_x_z_rhsIndex> m_Recv_Coords_Set ; //用来存放炮点所对应的接收点的所有坐标所组成的集合，并对其中的index_rhs进行赋值，使之标志其在集合中的索引(从0开始)

	vector<int>  m_AngularFrequencyIndex ; //用来存放所使用的所有角频率点索引

	vector< vector<float> > m_DampingConsts_RelateToAngularFrequencyIndex ; //用来存放角频率点索引所对应的衰减常数的信息

	vector< vector<complex<float> > > m_ObjectAssumedSource_RelateToAngularFrequencyIndex ; //用来放角频率点索引所对应的每个衰减系数的观测数据的估计源(在此是自己定义的源)
	vector< vector<complex<float> > > m_InitalAssumedSourec_RelateToAngularFrequencyIndex ; //用来放角频率点索引所对应的每个衰减系数上的初始数据的估计源(在此是自己定义的源)
	vector< vector<complex<float> > > m_SourceEstimateMulti_RelateToAngularFrequencyIndex ; //用来放角频率点索引所对应的每个衰减系数上的初始数据的估计源的源估计乘子，结合初始数据的估计源可得源数据

	vector< vector< vector< vector< complex<float> > > > > m_ObjectObservationData ; //用来存放观测数据，衰减后的频域中数据
																					//结构为：炮点->接收点->衰减常数->角频率点索引(内部存放相应的具体值)

	vector<Array_Length> m_StepLength_Value_List ; //用来存放求步长所需的三组数据，最后二位存放所得的步长(大小为五个元素)分别为最后的结果，及标志位


	//接口
public :
	/**********************************************************用于解方程的信息************************************************************************/
	CMUMPS_STRUC_C m_Mumps ; //一个mumps类型


public :
	/*****构造函数*************************************************************************************************************************************/

	CFDInversion() ; //构造函数，进行一些基本参数的设置

	//接口
public :
	/*****设置速度模型的速度值及相应的起伏地表数据、判断初始模型和目标模型是否匹配*********************************************************************/

	bool SetVelocityModel(VelocityModel_UsedAs) ; //对初始速度模型、目标速度模型进行赋值，并设置其相应的起伏地表信息
												  //参数用来决定是对初始速度模型还是目标速度模型进行处理
												  //记得要测试返回值，不然有可能对速度模型的赋值不成功

	bool InitalModel_Matching_ObjectModel() ; //判断所设置的初始速度模型和目标速度模型是否匹配
											  //如果返回true，则可进行下面的反演，且m_InitalNonzeroCoordinates和m_ObjectNonzeroCoordinates中的内容相同
											  //如果返回false，则不能进行下面的反演，m_SurfaceSmooth_Flag此时失去标记是否为起伏地表的能力
											  //在其中修改了相应的初始、目标速度模型中的最大和最小速度值(前提是返回true时信息有效)

private :
	bool SetVelocityData_Inital() ; //把宏InitalBinName_Model定义的BIN文件中的数据读到m_InitalVelocityModel中，并对m_InitalNonzeroCoordinates进行第一次赋值
	bool SetSurfaceData_Inital() ;  //对m_InitalNonzeroCoordinates进行第二次处理从而确定出准确的起伏信息

	bool SetVelocityData_Object() ; //把宏ObjectBinName_Model定义的BIN文件中的数据读到m_ObjectVelocityModel中，并对m_ObjectNonzeroCoordinates进行第一次赋值
	bool SetSurfaceData_Object() ; //对m_ObjectNonzeroCoordinates进行第二次处理从而确定出准确的起伏信息


	//接口
public :
	/*****生成速度模型的BIN文件************************************************************************************************************************/

	bool CreateVelocityModelBin(VelocityModel_UsedAs , string , int =0) const ; //把文件存放到由宏SaveFileDirectory定义的目录下，文件名为VelocityModel_UsedFor + string + n + .bin
																				//参数VelocityModel_UsedAs指定是保存m_InitalVelocityModel还是m_ObjectVelocityModel中的速度数据


public :
	/*****完成正演前的基本数据的初始化*****************************************************************************************************************/

	void PrepareForForward() ; //对容器m_UsedTotalGrideNum_PreColumn_AddPML进行了赋值

	void SetShotCoordsInformation() ; //设置所有炮点的信息

	void SetRecvCoordsInformation_RelateToShotCoords() ; //设置所有炮点所对应的接收点的信息，每个炮点所对应的接收点放在一个Vector中

	void SetRecvCoordsSetInformation() ; //设置所有炮点所对应的所有接收点的坐标信息所形成的集合，并对其中的index_rhs进行赋值，使之标志其在集合中的索引(从0开始)

	void SetAngularFrequencyIndexInformation() ; //设置所使用的所有角频率点索引信息

	void SetDampingConstsInformation_RelateToAngularFrequencyIndexs() ; //设置所有角频率点索引所对应的衰减常数信息，每个角频率点索引所对应的衰减系数所在一个Vector中

	void SetAssumeSource_SourceEstimateInformation() ; //设置所使用到的目标数据的源数据，初始数据的源数据，对初始数据的源数据进行修正的源估计乘子

	void SetObjectObservationDataInformation() ; //设置目标观测数据的Vector信息


	//实现
private :
	complex<float> GetSourceDisplacement_InTimeDomain(int i) ; //获得震源在时域中任意时刻的位移值(未衰减)


	//接口
public :
	/*****生成添加了PML层后的速度模型的BIN文件*********************************************************************************************************/

	bool CreateVelocityModelBin_AddPML(VelocityModel_UsedAs , string , int =0) const ;//把文件存放到由宏SaveFileDirectory定义的目录下，文件名为VelocityModel_UsedFor + "AddPML" + string + n +"_模型大小_"+ .bin
																				//参数VelocityModel_UsedAs指定是保存m_InitalVelocityModel还是m_ObjectVelocityModel添加PML层后的速度数据
																		//默认假设，目标速度模型与初始速度模型匹配
	
private :
	float GetVelocity_PML_MappingToNormal(VelocityModel_UsedAs , int column , int row) const ; //获得在相应的颠倒速度模型并添加了PML层后，映射到只颠倒速度模型的速度值
																				//参数VelocityModel_UsedAs指定是获得m_InitalVelocityModel还是m_ObjectVelocityModel添加PML层后的速度数据
																				//返回值若为-1，则认为输入的参数有误
																				//返回值若为0，则说明输入的参数刚好在(添加了PML层后)起伏地表上面，但在限定的大矩形框中

	//接口
public :
	/*****生成添加了PML层后的速度模型的BIN文件*********************************************************************************************************/

	bool CreateDampFactorBin_AddPML(int =0) const ;//把文件存放到由宏SaveFileDirectory定义的目录下，文件名为string + n +"_模型大小_"+ .bin
															//默认假设，目标速度模型与初始速度模型匹配

private :
	bool CreateDampFactorBin_AddPML_OneCase(int num , bool flag_RowOrientation , bool flag_IntegralPoint) const ; 
																		//生成在相应的颠倒了速度模型并添加了PML层后，单方向、单种点类型衰减系数的Bin文件
																		//把文件存放到由宏SaveFileDirectory定义的目录下，文件名为"DampFactor_" + num + "方向和点类型" + 模型大小 +.bin 
																		//默认假设，目标速度模型与初始速度模型匹配


	float GetDampFactor_PML(int column , int row , bool flag_RowOrientation , bool flag_IntegralPoint , bool flag_File = false) const ;
															//获得在相应的颠倒了速度模型并添加了PML层后，任意网格点索引在不同方向、整半点上的衰减系数
															//在这里用约定：半点的值通过左或上一个整点来索引(这里的左、上指反转模型后的左、上)
															//参数意义：反转后的列号与行号索引
															//flag_RowOrientaton为是否为X方向，flag_IntegralPoint为是否为整点
												//flag_File为是否是用来生成文件，这是由于按上面的规则在颠倒并添加PML层的速度模型的最右列和最下行在分别向下和右索引半点时会索引我们未定义的点
										//返回值为：-3.0f，表示此程序本身有问题，应进行修正
										//返回值为：-2.0f，表示用来索引的整点超出了添加PML层后所限定的大的矩形框
										//返回值为：-1.0f，表示用来索引的整点超出了可索引的范围(在即处理颠倒并添加PML层后起伏的上面了)，但未超出限定的大的矩形

	//接口
public :
	/*****反演的具体函数*******************************************************************************************************************************/

	void LaplaceFourier_Inversion() ; 

	//实现
private :
	void GetObjectObservationData() ;   //获得目标速度模型所对应的观测数据


	void GetPseudoHessianDiagData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume) ; //获得伪黑塞矩阵对角元的数据
			//实现的实现
	void SourceEstimate(int i_frequ_index , int j_dampconst_index , bool flag_sourceAssume) ; //对特定的衰减部分进行源估计


	void GetGradientData(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index) ; //获得梯度矩阵

	void MakeGradientPretreatment() ;  //用伪黑塞矩阵对角元对所得的梯度矩阵进行预处理

	fstream fWrite_Step ;

	void GetStepLength_Used(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index) ; //获得最佳的修正步长
		//实现的实现
	float StepLength_MAX_Valid(float min_velocity) const ; //求解所允许的最大步长
		//实现的实现
	void StepChange_InitalVelocityModel(float length) ; //对初始模型按所选用的步长进行修正
		//实现的实现
	float GetObjectiveFunction_Value(complex<float> complex_freq , int i_frequ_index , int j_dampconst_index) ;
													//获得初始速度模型与目标速度模型之间的目标函数值


	void StepChange_GradientVector(float length) ; //对用伪黑塞对角元矩阵预处理过的梯度矩阵，乘以步长以确定最终的初始速度修正量

	bool Create_VelocityModel_GradientVector_HessianVector(int i_dampconst_index , int j_frequ_index , int iter , int flag) const ; 
													//对修正后的初始速度模型、修正所用的伪黑塞矩阵、修正所使用的最终初始速度模型修正量保存到BIN文件中
	


public :
	/**********************************************************用于解方程*****************************************************************************/

	void MumpsInital() ; //完成MUMPS的基本初始化

	void MumpsEnd() ;     //对MUMPS进行资源等方面的清理

	void Set_NRHS_Matrix() ; //设置相同系数方程组的右边部分，适用于真实炮点正演时的设置

	void Set_NRHS_Matrix_Hessian() ; //设置相同系数方程组的右边部分，只是为求黑塞时使用

	void Set_Coefficient_Matrix(VelocityModel_UsedAs , complex<float> complex_frequency) ;//设置初始速度、目标速度模型颠倒并添加PML层后所对应的同系数方程组的方程系数
																				//参数VelocityModel_UsedAs指定是获得m_InitalVelocityModel还是m_ObjectVelocityModel添加PML层后的速度数据

private :
	int GetIndex_InColumnStoredModel_AddPML(int column , int row , int offset_c , int offset_r) const ;
											//返回按列排放颠倒并添加PML层后的速度模型中任意一个网格点在其中的位置(从1开始)
											//返回值为：-2，表示用来索引的整点超出了添加PML层后所限定的大的矩形框
											//返回值为：-1，表示用来索引的整点超出了可索引的范围(在即处理颠倒并添加PML层后起伏的上面了)，但未超出限定的大的矩形

	//实现
private :
	//FFT变换
	void FFT(complex<float> * TD, complex<float> * FD, int r)
	{
		// 付立叶变换点数
		long count;

		// 循环变量
		int		i,j,k;

		// 中间变量
		int		bfsize,p;

		// 角度
		float angle;
		//W为要用到的旋转因子
		//X1,X2分别为各级运算的输入和输出数据

		complex<float> *W,*X1,*X2,*X;

		// 计算付立叶变换点数
		count = 1 << r;

		// 分配运算所需存储器
		W  = new complex<float>[count / 2]; 
		X1 = new complex<float>[count];
		X2 = new complex<float>[count];

		// 计算加权系数
		for(i = 0; i < count / 2; i++)
		{
			angle = -i * PI * 2 / count;
			//		W[i].real()=cos(angle);
			//		W[i].image=sin(angle);	
			W[i]=complex<float>(cos(angle),sin(angle));
		}

		// 将时域点写入X1
		memcpy(X1, TD, sizeof(complex<float>) * count);

		// 采用蝶形算法进行快速付立叶变换
		for(k = 0; k < r; k++)  //K为当前迭代级数,也为当前级中蝶组个数
		{
			for(j = 0; j < 1 << k; j++) //j 为当前级中的蝶组序号
			{
				bfsize = 1 << (r-k);    //bfsize/2为蝶距,即每个蝶组中蝶型运算个数
				for(i = 0; i < bfsize / 2; i++)
				{
					p = j * bfsize;

					//完成以下蝶型运算
					X2[i + p] = X1[i + p] + X1[i + p + bfsize / 2];
					X2[i + p + bfsize / 2] = (X1[i + p] - X1[i + p + bfsize / 2]) * W[i * (1<<k)];						

				}
			}
			//前级输出变后级输入
			X  = X1;
			X1 = X2;
			X2 = X;
		}

		// 计算倒序数,重新排序
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

		// 释放内存
		delete []W;
		delete []X1;
		delete []X2;
	}

	void IFFT(complex<float> * FD, complex<float> * TD, int r)
	{
		// 付立叶变换点数
		long count;

		// 循环变量
		int		i;

		complex<float> *X;

		// 计算付立叶变换点数
		count = 1 << r;

		// 分配运算所需存储器
		X = new complex<float>[count];

		// 将频域点写入X
		memcpy(X, FD, sizeof(complex<float>) * count);

		// 求共轭
		for(i = 0; i < count; i++)
		{
			//		X[i].real=.real;
			//		X[i].image=-X[i].image;
			X[i]=conj(X[i]);
		}

		// 调用快速付立叶变换
		FFT(X, TD, r);

		// 求时域点的共轭
		for(i = 0; i < count; i++)
		{
			TD[i]=conj(TD[i]/(float)count);	
			//TD[i]=conj(TD[i]);	
		}
		// 释放内存
		delete []X;
	}


private :
	//求步长
	float ParaFit(float x[], float y[], int n)                             //最小步长计算
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

	float PolyInt(float xa[],float ya[],int n,float x)                             //抛物线法求取步长
	{
		float *c = new float[n];
		float *d = new float[n];
		int i,j,ns=0;
		float ho,hp,w,den,dy;

		float dif = fabs(x-xa[0]);	
		for (i=0;i<n;i++)//dift指此点到三点中最邻近的点的距离，ns指相应的点，c,d中都存放ya的值
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