#include "stdafx.h"
#include "Mycv.h"
#include "FilteringMask.h"


CMycv::CMycv()
{
}


CMycv::~CMycv()
{
}

/*
int:RGB通道图片，输出图片的高度宽度
out:RGB通道直方图
*/
Mat CMycv::RGB_Histogram(Mat* img, int hi, int wi)
{
	Mat r = RGB_Extract(img, 2);
	Mat g = RGB_Extract(img, 1);
	Mat b = RGB_Extract(img, 0);
	int mxr = GreyValue(&r, true);
	int mxg = GreyValue(&g, true);
	int mxb = GreyValue(&b, true);
	int max = mxr;
	if (max < mxg)max = mxg;
	if (max < mxb)max = mxb;
	Mat hr = Histogram(&r, hi, wi,max);
	Mat hg = Histogram(&g, hi, wi, max);
	Mat hb = Histogram(&b, hi, wi, max);
	Mat rgb = RGB_Synthesis(&hr, &hg, &hb);
	return rgb;
}
/*
int:3通道图片
out:均值化后的3通道图
*/
Mat CMycv::RGB_Equalization(Mat* img)
{
	Mat r = RGB_Extract(img, 2);
	Mat g = RGB_Extract(img, 1);
	Mat b = RGB_Extract(img, 0);
	Mat er = Equalization(&r);
	Mat eg = Equalization(&g);
	Mat eb = Equalization(&b);
	Mat rgb = RGB_Synthesis(&er, &eg, &eb);
	return rgb;
}
/*
int:3通道图片  转换算法（1、平均值 2、浮点算法 3、整数法 4、移位法 5、只取绿色）
out:灰度图
*/
Mat CMycv::RGB_Gray(Mat* img, int _type)
{
	int i, j;
	int hi = img->rows, wi = img->cols;
	Mat gray(hi, wi, CV_8U, Scalar(0));
	for (i = 0; i < hi; i++)
	{
		for (j = 0; j < wi; j++)
		{
			Vec3b co=img->at<Vec3b>(i,j);
			int nc;
			switch (_type)
			{
			case 1:nc = (co[2] + co[1] + co[0]) / 3.0; break;
			case 2:nc = co[2] * 0.3 + co[1] * 0.59 + co[0] * 0.11; break;
			case 3:nc = (co[2] * 30 + co[1] * 59 + co[0] * 11) / 100; break;
			case 4:nc = (co[2] * 76 + co[1] * 151 + co[0] * 28) >> 8; break;
			case 5:nc = co[1]; break;
			default:nc = (co[2] + co[1] + co[0]) / 3.0;
				break;
			}
			gray.at<uchar>(i, j) = nc;
		}
	}
	return gray;
}
/*
int:单通道图片，输出图片的高度宽度
out:该通道直方图
*/
Mat CMycv::Histogram(Mat* img, int hi, int wi) 
{
	int* c;
	int i, j;

	c = Histogram_Group(img);

	Mat zft(hi, wi, CV_8U, Scalar(0));

	float max = 0;
	//int i;
	for (i = 0; i < 256; i++)
	{
		if (max < c[i]) max = c[i];
	}
	for (i = 0; i < 256; i++)
	{
		rect(&zft, Point(hi*(1 - c[i] / max), wi / 256.0 * i), Point(hi - 1, wi / 256 * (i + 1) - 1), 255);
	}
	return zft;
}

/*
int:单通道图片，输出图片的高度宽度,图片最高可表示的灰度数
out:该通道直方图
*/
Mat CMycv::Histogram(Mat* img, int hi, int wi, float  max)
{
	int* c;
	int i, j;

	c = Histogram_Group(img);

	Mat zft(hi, wi, CV_8U, Scalar(0));

	for (i = 0; i < 256; i++)
	{
		rect(&zft, Point(hi*(1 - c[i] / max), wi / 256.0 * i), Point(hi - 1, wi / 256 * (i + 1) - 1), 255);
	}
	delete c;
	return zft;
}
/*
int:单通道图片
out:均值化后的单通道图
*/
Mat  CMycv::Equalization(Mat* img)
{
	int* c;
	float *l;
	int i, j;
	c = Histogram_Group(img);
	l = Cumulative(c, img->rows*img->cols);
	return Histogram_Equalization(img, l);
}
/*
int:单通道图片  累积概率数组
out:均值化后的单通道图
*/
Mat CMycv::Histogram_Equalization(Mat* img, float* l)
{
	int i, j;
	int hi = img->rows, wi = img->cols;
	Mat newimg(hi, wi, CV_8U, Scalar(0));
	for (i = 0; i < hi; i++)
	{
		for (j = 0; j < wi; j++)
		{
			newimg.at<uchar>(i, j) = l[img->at<uchar>(i,j)] * 255;
		}
	}
	return newimg;
}


/*
int:RGB通道图片，提取通道(0 1 2)
out:该通道直方图
*/
Mat CMycv::RGB_Extract(Mat* img, int p)
{
	int i,j; 
	int hi = img->rows;
	int wi = img->cols;
	Mat out(hi, wi, CV_8U, Scalar(0));
	for (i = 0; i < hi; i++)
	{
		for (j = 0; j < wi; j++)
		{
			out.at<uchar>(i, j) = img->at<Vec3b>(i, j)[p];
		}
	}
	return out;
}
/*
int:R G B 3个通道的单通道图片
out:rgb图片
*/
Mat CMycv::RGB_Synthesis(Mat* r, Mat* g, Mat* b)
{
	int hi=0, wi=0;
	int i,j;
	if (r->rows > hi)hi = r->rows;
	if (g->rows > hi)hi = g->rows;
	if (b->rows > hi)hi = b->rows;
	if (r->cols > wi)wi = r->cols;
	if (g->cols > wi)wi = g->cols;
	if (b->cols > wi)wi = b->cols;
	Mat rgb(hi, wi, CV_8UC3, Scalar(0, 0, 0));
	for (i = 0; i < hi; i++)
	{
		for (j = 0; j < wi; j++)
		{
			Vec3b co;
			co[0] = b->at<uchar>(i, j);
			co[1] = g->at<uchar>(i, j);
			co[2] = r->at<uchar>(i, j);
			rgb.at<Vec3b>(i, j) = co;
		}
	}
	return rgb;
}




/*
int:单通道图片，输出最大还是最小(true_max,flase_min)
out:最大，最小灰度值
*/
int CMycv::GreyValue(Mat* img, bool maxormin)
{
	int c[256];
	int max = 0, min = 256;
	int i,j;
	memset(c, 0, sizeof(c));

	int rs = img->rows, cs = img->cols;
	for (i = 0; i < rs; i++)
	{
		for (j = 0; j < cs; j++)
		{
			c[img->at<uchar>(i, j)]++;
		}
	}

	for (i = 0; i < 256; i++)
	{
		if (max < c[i]) max = c[i];
		if (min > c[i]) min = c[i];
	}
	if (maxormin)
	{
		return max;
	}
	return min;
}
/*
int:直方图数组
out:累积直方图数组
*/
float* CMycv::Cumulative(int* c, float size)
{
	int i;
	float * l = new float[256];
	for (i = 0; i < 256; i++)
	{
		l[i] = 0;
		if (i != 0)
		{
			l[i] = l[i - 1];
		}
		l[i] += c[i]/size;
	}
	return l;
}


/*
int:单通道图片
out:灰度图数组
*/
int* CMycv::Histogram_Group(Mat* img)
{
	int* c = new int[256];
	int i, j;
	for (i = 0; i < 256; i++)
	{
		c[i] = 0;
	}

	int rs = img->rows, cs = img->cols;
	for (i = 0; i < rs; i++)
	{
		for (j = 0; j < cs; j++)
		{
			c[img->at<uchar>(i, j)]++;
		}
	}
	return c;
}


/*
int:单通道图片，对角两点，颜色
*/
void CMycv::rect(Mat *img, Point sp, Point ep, int co)
{
	int i, j;
	for (i = sp.x; i <= ep.x; i++)
	{
		for (j = sp.y; j <= ep.y; j++)
		{
			//cout << sp.x << "==" << sp.y<<endl;
			//cout << ep.x << "==" << ep.y<<endl;
			img->at<uchar>(i, j) = co;
		}
	}
}

/*
int:单通道图片,模糊模板大小(宽高)
out:模糊后图片
*/
Mat CMycv::Filter_Blur_Line(Mat* img, int size_w, int size_h)
{
	if (size_w % 2 == 1 && size_h % 2 == 1)
	{
		int * mod = new int[size_w*size_h];
		for (int i = 0; i < size_w*size_h; i++)
		{
			mod[i] = 1;
		}
		CFilteringMask mask(size_w, size_h, 1.0 / (size_w*size_h), mod);
		return mask.ALLProcess(img);
	}
	return Mat(256,256,CV_8U,Scalar(0));
}

/*
int:单通道图片 中值滤波模板大小
out:中值滤波后图片
*/
Mat CMycv::Filter_Median(Mat* img,int size)
{
	int wi = img->cols;
	int hi = img->rows;
	Mat out(hi, wi, CV_8U, Scalar(0));
	for (int i = 0; i < hi; i++)
	{
		for (int j = 0; j < wi; j++)
		{
			out.at<uchar>(i, j) = Filter_Median(img, i, j, size);
		}
	}
	return out;
}


/*
int:单通道图片 坐标 中值滤波模板大小
out:该坐标的值
*/
int CMycv::Filter_Median(Mat* img, int posx, int posy, int size)
{
	vector<int> nums;
	int wi = img->cols;
	int hi = img->rows;
	int c = (size - 1) / 2;
	int med = 0;
	for (int i = -c; i <= c; i++)
	{
		for (int j = -c; j <= c; j++)
		{
			int py = posy - i;
			int px = posx - j;
			if (py < 0) py = 0;
			if (px < 0) px = 0;
			if (py >= wi) py = wi - 1;
			if (px >= hi) px = hi - 1;
			nums.push_back(img->at<uchar>(px, py));
		}
	}
	sort(nums.begin(), nums.end());
	return nums.at(size*size/2);
}

/*
int:单通道图片 算子大小(中心)1(9) 2(5) 3(-5) 4(-9)
out:滤波后的图
*/
Mat CMycv::Filter_Laplasse_operator(Mat* img, int type)
{
	if (type<1 || type>4) return Mat(256, 256, CV_8U, Scalar(0));


	switch (type)
	{
	case 1:{int x[] = { -1, -1, -1,
					-1, 9, -1,
					-1, -1, -1 };
		   CFilteringMask mask(3,3, 1.0, x);
		   return mask.ALLProcess(img); }
	case 2:{int x[] = { 0, -1, 0,
					-1, 5, -1,
					0, -1, 0 };
		   CFilteringMask mask(3,3, 1.0, x);
		   return mask.ALLProcess(img); }
	case 3:{int x[] = { 0, 1, 0,
					1, -5, 1,
					0, 1, 0 };
		   CFilteringMask mask(3,3, -1.0, x);
		   return mask.ALLProcess(img); }
	case 4:{int x[] = { 1, 1, 1,
					1, -9, 1,
					1, 1, 1 };
		   CFilteringMask mask(3,3, -1.0,x);
		   return mask.ALLProcess(img); }
	}
}

/*
int:单通道图片 
out:傅里叶谱
*/
Mat CMycv::DFT(Mat img)//来自opencv官网
{

	//扩展到适合FFT的尺寸
	Mat padded;                         
	int m = getOptimalDFTSize(img.rows);
	int n = getOptimalDFTSize(img.cols); // on the border add zero values

	copyMakeBorder(img, padded, 0, m - img.rows, 0, n - img.cols, BORDER_CONSTANT, Scalar::all(0));

	//扩展为实部 虚部
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI;
	//合成一张双通道图像
	merge(planes, 2, complexI);      


	//DFT变换（依然是双通道图像）
	dft(complexI, complexI);            // this way the result may fit in the source matrix

	//旋转
	complexI = DFT_Rotate(complexI);


	return complexI;
}


/*
int:傅里叶谱(双通道图) 原图像： 宽 高
out:单通道图片（反变换后的图片）
*/
Mat CMycv::IDFT(Mat dftimg,int hi,int wi)
{

	Mat ifft;
	dftimg = DFT_Rotate(dftimg);
	idft(dftimg, ifft, DFT_REAL_OUTPUT);
	normalize(ifft, ifft, 0, 1, CV_MINMAX);
	ifft = ifft(Rect(0, 0, wi, hi));
	

	return ifft;
}


/*
int:DFT图像（双通道图像）
out:幅度谱
*/
Mat CMycv::DFT_AmplitudeSpectrum(Mat dftimg)
{
	Mat planes[] = { Mat_<float>(dftimg), Mat::zeros(dftimg.size(), CV_32F) };

	//Mat test=complexI.clone();
	//idft(test, test);
	//split(test, planes);
	//magnitude(planes[0], planes[1], planes[0]);
	//imshow("test2", planes[0]);


	// compute the magnitude and switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))

	//拆分到2个图像
	split(dftimg, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude  
	Mat magI = planes[0];

	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);

	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	// rearrange the quadrants of Fourier image  so that the origin is at the image center        

	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a 
	// viewable image form (float between values 0 and 1).

	//imshow("Input Image", img);    // Show the result
	//imshow("spectrum magnitude", magI);

	return magI;
}

/*
int:DFT图像（双通道图像）
out:相位谱
*/
Mat CMycv::DFT_PhaseSpectrum(Mat dftimg)
{
	Mat planes[] = { Mat_<float>(dftimg), Mat::zeros(dftimg.size(), CV_32F) };

	//拆分到2个图像
	split(dftimg, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))

	int hi = dftimg.rows;
	int wi = dftimg.cols;
	for (int i = 0; i < hi; i++)
	{
		for (int j = 0; j < wi; j++)
		{
			planes[0].at<float>(i, j) = atan(planes[1].at<float>(i, j) / planes[0].at<float>(i, j));
		}
	}
	Mat magI = planes[0];

	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);

	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	// rearrange the quadrants of Fourier image  so that the origin is at the image center        

	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a 
	// viewable image form (float between values 0 and 1).

	//imshow("Input Image", img);    // Show the result
	//imshow("spectrum magnitude", magI);

	return magI;
}

/*
int:DFT图像（双通道图像）
out:实部谱
*/
Mat CMycv::DFT_RealPart(Mat dftimg, bool _log)
{
	Mat planes[] = { Mat_<float>(dftimg), Mat::zeros(dftimg.size(), CV_32F) };

	//拆分到2个图像
	split(dftimg, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))

	Mat magI = planes[0];
	if(_log) log(magI, magI);
	magI += Scalar::all(1);                    // switch to logarithmic scale

	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
     

	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a 

	return magI;
}

/*
int:DFT图像（双通道图像）
out:虚部谱
*/
Mat CMycv::DFT_ImaginaryPart(Mat dftimg, bool _log)
{
	Mat planes[] = { Mat_<float>(dftimg), Mat::zeros(dftimg.size(), CV_32F) };

	//拆分到2个图像
	split(dftimg, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))

	Mat magI = planes[1];
	if (_log) log(magI, magI);
	magI += Scalar::all(1);                    // switch to logarithmic scale

	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
	normalize(magI, magI, 0, 1, CV_MINMAX);

	return magI;
}



/*
int:待旋转图像
out:旋转后图像
*/
Mat CMycv::DFT_Rotate(Mat img)
{
	Mat _magI = img.clone();
	// rearrange the quadrants of Fourier image  so that the origin is at the image center        
	int cx = img.cols / 2;
	int cy = img.rows / 2;

	Mat q0(_magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant 
	Mat q1(_magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(_magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(_magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);

	return _magI;
}

/*
int:DFT图像（双通道图像） 滤波器图像
out:过滤后图像
*/
Mat CMycv::DFT_Filter(Mat dftimg, Mat filter)
{
	int hi = dftimg.rows;
	int wi = dftimg.cols;

	Mat out;
	Mat planes[2];


	split(dftimg, planes);

	for (int i = 0; i < hi; i++)
	{
		for (int j = 0; j < wi; j++)
		{
			planes[0].at<float>(i, j) = planes[0].at<float>(i, j)* filter.at<float>(i, j);
			planes[1].at<float>(i, j) = planes[1].at<float>(i, j)* filter.at<float>(i, j);
		}
	}

	merge(planes, 2, out);

	return  out;
}

/*
int:DFT图像（双通道图像）的宽高   D0
out:滤波器
*/
Mat CMycv::DFT_ILPF(int hi,int wi, float D0)
{
	Mat filter(hi,wi,CV_32F,Scalar(0));
	for (int i = 0; i < hi; i++)
	{
		for (int j = 0; j < wi; j++)
		{
			if (PointDistance(i, j, hi / 2, wi / 2) <= D0) filter.at<float>(i, j) = 1;
			else filter.at<float>(i, j) = 0;
		}
	}
	return  filter;
}

/*
int:DFT图像（双通道图像）的宽高   D0	n阶数
out:滤波器
*/
Mat CMycv::DFT_BLPF(int hi, int wi, float D0,int n)
{
	Mat filter(hi, wi, CV_32F, Scalar(0));
	for (int i = 0; i < hi; i++)
	{
		for (int j = 0; j < wi; j++)
		{
			filter.at<float>(i, j) = 1 / (1 + powf((PointDistance(i, j, hi / 2, wi / 2) / D0), 2 * n));
			
		}
	}
	return  filter;
}

/*
int:DFT图像（双通道图像）的宽高   D0
out:滤波器
*/
Mat CMycv::DFT_GLPF(int hi, int wi, float D0)
{
	Mat filter(hi, wi, CV_32F, Scalar(0));
	for (int i = 0; i < hi; i++)
	{
		for (int j = 0; j < wi; j++)
		{
			float d = PointDistance(i, j, hi / 2, wi / 2);
			filter.at<float>(i, j) = powf(C_E, -1 * (d / D0)*(d / D0) / 2);
		}
	}
	//cout << filter.at<float>(100, 100) << "==" << d << "==" << -1*(d / D0)*(d / D0)/2 << endl;
	return  filter;
}



void CMycv::DFT_Filter_Show(Mat filter, char* name)
{
	normalize(filter, filter, 0, 1, CV_MINMAX);
	imshow(name, filter);
}

/*
int:两点坐标
out:距离
*/
float CMycv::PointDistance(float p1x, float p1y, float p2x, float p2y)
{
	float x = p1x - p2x;
	float y = p1y - p2y;
	return sqrt(x*x + y*y);
}