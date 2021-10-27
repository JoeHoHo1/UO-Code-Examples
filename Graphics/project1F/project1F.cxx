//	Access my movie at: https://drive.google.com/file/d/1JppwIJgfeHWt1p96HBDQ17x9gYta8DiV/view?usp=sharing

#include <iostream>
#include <vtkDataSet.h>
#include <vtkImageData.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#define _USE_MATH_DEFINES		//this makes visual studio code happy
#include <math.h>

//globals
#define WIDTH 1000
#define HEIGHT 1000
#define NORMALS
#define VIDEO 					//undefine for frame000.png only

#ifdef VIDEO
	#include <thread>
	using std::thread;
	#define FRAMES 1000			//number of frames to generate
	#define THREADS 32
#endif

using std::cerr;
using std::endl;

double ceil__441(double f)
{
	return ceil(f - 0.00001);
}

double floor__441(double f)
{
	return floor(f + 0.00001);
}

vtkImageData*
NewImage(int height, int width)
{
	vtkImageData* img = vtkImageData::New();
	img->SetDimensions(width, height, 1);
	img->AllocateScalars(VTK_UNSIGNED_CHAR, 3);

	return img;
}

void
WriteImage(vtkImageData* img, const char* filename)
{
	std::string full_filename = filename;
	full_filename += ".png";
	vtkPNGWriter* writer = vtkPNGWriter::New();
	writer->SetInputData(img);
	writer->SetFileName(full_filename.c_str());
	writer->Write();
	writer->Delete();
}



/* Hank's Matrix Class */
class Matrix
{
  public:
    double          A[4][4];  // A[i][j] means row i, column j

    void            TransformPoint(const double *ptIn, double *ptOut);
    static Matrix   ComposeMatrices(const Matrix &, const Matrix &);
    void            Print(ostream &o);
};

void
Matrix::Print(ostream &o)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        char str[256];
        sprintf_s(str, "(%.7f %.7f %.7f %.7f)\n", A[i][0], A[i][1], A[i][2], A[i][3]);
        o << str;
    }
}

Matrix
Matrix::ComposeMatrices(const Matrix &M1, const Matrix &M2)
{
    Matrix rv;
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
        {
            rv.A[i][j] = 0;
            for (int k = 0 ; k < 4 ; k++)
                rv.A[i][j] += M1.A[i][k]*M2.A[k][j];
        }

    return rv;
}

void
Matrix::TransformPoint(const double *ptIn, double *ptOut)
{
    ptOut[0] = ptIn[0]*A[0][0]
             + ptIn[1]*A[1][0]
             + ptIn[2]*A[2][0]
             + ptIn[3]*A[3][0];
    ptOut[1] = ptIn[0]*A[0][1]
             + ptIn[1]*A[1][1]
             + ptIn[2]*A[2][1]
             + ptIn[3]*A[3][1];
    ptOut[2] = ptIn[0]*A[0][2]
             + ptIn[1]*A[1][2]
             + ptIn[2]*A[2][2]
             + ptIn[3]*A[3][2];
    ptOut[3] = ptIn[0]*A[0][3]
             + ptIn[1]*A[1][3]
             + ptIn[2]*A[2][3]
             + ptIn[3]*A[3][3];
}

/* Hank's Camera Class */

class Camera
{
  public:
    double          near, far;
    double          angle;
    double          position[3];
    double          focus[3];
    double          up[3];

    Matrix          ViewTransform(void);
    Matrix          CameraTransform(void);
    Matrix          DeviceTransform(void);
};


double SineParameterize(int curFrame, int nFrames, int ramp)
{
    int nNonRamp = nFrames-2*ramp;
    double height = 1./(nNonRamp + 4*ramp/M_PI);
    if (curFrame < ramp)
    {
        double factor = 2*height*ramp/M_PI;
        double eval = cos(M_PI/2*((double)curFrame)/ramp);
        return (1.-eval)*factor;
    }
    else if (curFrame > nFrames-ramp)
    {
        int amount_left = nFrames-curFrame;
        double factor = 2*height*ramp/M_PI;
        double eval =cos(M_PI/2*((double)amount_left/ramp));
        return 1. - (1-eval)*factor;
    }
    double amount_in_quad = ((double)curFrame-ramp);
    double quad_part = amount_in_quad*height;
    double curve_part = height*(2*ramp)/M_PI;
    return quad_part+curve_part;
}

Camera
GetCamera(int frame, int nframes)
{
    double t = SineParameterize(frame, nframes, nframes/10);
    Camera c;
    c.near = 5;
    c.far = 200;
    c.angle = M_PI/6;
    c.position[0] = 40*sin(2*M_PI*t);
    c.position[1] = 40*cos(2*M_PI*t);
    c.position[2] = 40;
    c.focus[0] = 0;
    c.focus[1] = 0;
    c.focus[2] = 0;
    c.up[0] = 0;
    c.up[1] = 1;
    c.up[2] = 0;
    return c;
}

/* My addition to Hank's Camera Class */
Matrix Camera::CameraTransform(void)
{
    double origin[3] = {0,0,0};
    double* t = new double[3];
	double* u = new double[3];
	double* v = new double[3];
	double* w = new double[3];
	for(int i = 0; i < 3; i++){
    	t[i] = 0 - position[i];
		w[i] = position[i] - focus[i];
	}
	//cross product for u
	for(int i = 0; i < 3; i++)
		u[i] = ((up[(i+1)%3]*w[(i+2)%3]) - (up[(i+2)%3]*w[(i+1)%3]));
	
	//cross product for v
	for(int i = 0; i < 3; i++)
		v[i] = ((w[(i+1)%3]*u[(i+2)%3]) - (w[(i+2)%3]*u[(i+1)%3]));
    
    //normalize
	double norm = 0, norm2 = 0, norm3 = 0;
	for(int i = 0; i < 3; i++){
		norm += u[i]*u[i];
		norm2 += v[i]*v[i];
		norm3 += w[i]*w[i];

	}

	for(int i = 0; i < 3; i++){
    	u[i] /= sqrt(norm);
		v[i] /= sqrt(norm2);
    	w[i] /= sqrt(norm3);
	}    	
		
    Matrix* ct = new Matrix();

    for(int i = 0; i < 3; i++){
		ct->A[i][0] = u[i];
		ct->A[i][1] = v[i];
		ct->A[i][2] = w[i];
		ct->A[i][3] = 0;
		ct->A[3][i] = 0;
	}
    
	//dot product
	for(int i = 0; i < 3; i++){
		ct->A[3][0] += (u[i]*t[i]);
		ct->A[3][1] += (v[i]*t[i]);
		ct->A[3][2] += (w[i]*t[i]);
	}

    ct->A[3][3] = 1;
    
    return *ct;
}

Matrix Camera::ViewTransform(void)
{
    Matrix* vt = new Matrix();
    
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			vt->A[i][j] = 0;
		}
	}

    vt->A[0][0] = 1 / tan((angle / 2));
    vt->A[1][1] = 1 / tan((angle / 2));
    vt->A[2][2] = (far + near)/(far - near);
    vt->A[2][3] = -1;
    vt->A[3][2] = (2 * (far * near))/(far - near);
    
    return *vt;
}

Matrix Camera::DeviceTransform(void)
{
    Matrix* dt = new Matrix();
    
    //init
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			dt->A[i][j] = 0;
		}
	}

    dt->A[0][0] = WIDTH/2;
    dt->A[1][1] = HEIGHT/2;
    dt->A[2][2] = 1;
    dt->A[3][0] = WIDTH/2;
    dt->A[3][1] = HEIGHT/2;
    dt->A[3][3] = 1;
    
    return *dt;
}

/* End of my addition to camera class */

class Triangle
{
public:
	double      	X[3];
	double      	Y[3];
	double      	Z[3];
	double			colors[3][3];
	double 			normals[3][3];
	double			shading[3];
};

class Screen
{
public:
	unsigned char* buffer;
	double* zbuffer;
	int width, height;
};

std::vector<Triangle>
GetTriangles(void)
{
    vtkPolyDataReader *rdr = vtkPolyDataReader::New();
    rdr->SetFileName("proj1f_geometry.vtk");
    //cerr << "Reading" << endl;
    rdr->Update();
    //cerr << "Done reading" << endl;
    if (rdr->GetOutput()->GetNumberOfCells() == 0)
    {
        cerr << "Unable to open file!!" << endl;
        exit(EXIT_FAILURE);
    }
    vtkPolyData *pd = rdr->GetOutput();

    int numTris = pd->GetNumberOfCells();
    vtkPoints *pts = pd->GetPoints();
    vtkCellArray *cells = pd->GetPolys();
    vtkDoubleArray *var = (vtkDoubleArray *) pd->GetPointData()->GetArray("hardyglobal");
    double *color_ptr = var->GetPointer(0);
    //vtkFloatArray *var = (vtkFloatArray *) pd->GetPointData()->GetArray("hardyglobal");
    //float *color_ptr = var->GetPointer(0);
    vtkFloatArray *n = (vtkFloatArray *) pd->GetPointData()->GetNormals();
    float *normals = n->GetPointer(0);
    std::vector<Triangle> tris(numTris);
    vtkIdType npts;
    vtkIdType *ptIds;
    int idx;
    for (idx = 0, cells->InitTraversal() ; cells->GetNextCell(npts, ptIds) ; idx++)
    {
        if (npts != 3)
        {
            cerr << "Non-triangles!! ???" << endl;
            exit(EXIT_FAILURE);
        }
        double *pt = NULL;
        pt = pts->GetPoint(ptIds[0]);
        tris[idx].X[0] = pt[0];
        tris[idx].Y[0] = pt[1];
        tris[idx].Z[0] = pt[2];
#ifdef NORMALS
        tris[idx].normals[0][0] = normals[3*ptIds[0]+0];
        tris[idx].normals[0][1] = normals[3*ptIds[0]+1];
        tris[idx].normals[0][2] = normals[3*ptIds[0]+2];
#endif
        pt = pts->GetPoint(ptIds[1]);
        tris[idx].X[1] = pt[0];
        tris[idx].Y[1] = pt[1];
        tris[idx].Z[1] = pt[2];
#ifdef NORMALS
        tris[idx].normals[1][0] = normals[3*ptIds[1]+0];
        tris[idx].normals[1][1] = normals[3*ptIds[1]+1];
        tris[idx].normals[1][2] = normals[3*ptIds[1]+2];
#endif
        pt = pts->GetPoint(ptIds[2]);
        tris[idx].X[2] = pt[0];
        tris[idx].Y[2] = pt[1];
        tris[idx].Z[2] = pt[2];
#ifdef NORMALS
        tris[idx].normals[2][0] = normals[3*ptIds[2]+0];
        tris[idx].normals[2][1] = normals[3*ptIds[2]+1];
        tris[idx].normals[2][2] = normals[3*ptIds[2]+2];
#endif

        // 1->2 interpolate between light blue, dark blue
        // 2->2.5 interpolate between dark blue, cyan
        // 2.5->3 interpolate between cyan, green
        // 3->3.5 interpolate between green, yellow
        // 3.5->4 interpolate between yellow, orange
        // 4->5 interpolate between orange, brick
        // 5->6 interpolate between brick, salmon
        double mins[7] = { 1, 2, 2.5, 3, 3.5, 4, 5 };
        double maxs[7] = { 2, 2.5, 3, 3.5, 4, 5, 6 };
        unsigned char RGB[8][3] = { { 71, 71, 219 }, 
                                    { 0, 0, 91 },
                                    { 0, 255, 255 },
                                    { 0, 128, 0 },
                                    { 255, 255, 0 },
                                    { 255, 96, 0 },
                                    { 107, 0, 0 },
                                    { 224, 76, 76 } 
                                  };
        for (int j = 0 ; j < 3 ; j++)
        {
            float val = color_ptr[ptIds[j]];
            int r;
            for (r = 0 ; r < 7 ; r++)
            {
                if (mins[r] <= val && val < maxs[r])
                    break;
            }
            if (r == 7)
            {
                cerr << "Could not interpolate color for " << val << endl;
                exit(EXIT_FAILURE);
            }
            double proportion = (val-mins[r]) / (maxs[r]-mins[r]);
            tris[idx].colors[j][0] = (RGB[r][0]+proportion*(RGB[r+1][0]-RGB[r][0]))/255.0;
            tris[idx].colors[j][1] = (RGB[r][1]+proportion*(RGB[r+1][1]-RGB[r][1]))/255.0;
            tris[idx].colors[j][2] = (RGB[r][2]+proportion*(RGB[r+1][2]-RGB[r][2]))/255.0;
        }
    }

    return tris;
}


struct LightingParameters
{
    LightingParameters(void)
    {
         lightDir[0] = -0.6;
         lightDir[1] = 0;
         lightDir[2] = -0.8;
         Ka = 0.3;
         Kd = 0.7;
         Ks = 2.8;
         alpha = 50.5;
    };
  

    double lightDir[3]; // The direction of the light source
    double Ka;          // The coefficient for ambient lighting
    double Kd;          // The coefficient for diffuse lighting
    double Ks;          // The coefficient for specular lighting
    double alpha;       // The exponent term for specular lighting
};

LightingParameters lp;

LightingParameters
GetLighting(Camera c)
{
    LightingParameters lp;
    lp.lightDir[0] = c.position[0]-c.focus[0];
    lp.lightDir[1] = c.position[1]-c.focus[1];
    lp.lightDir[2] = c.position[2]-c.focus[2];
    double mag = sqrt(lp.lightDir[0]*lp.lightDir[0]
                    + lp.lightDir[1]*lp.lightDir[1]
                    + lp.lightDir[2]*lp.lightDir[2]);
    if (mag > 0)
    {
        lp.lightDir[0] /= mag;
        lp.lightDir[1] /= mag;
        lp.lightDir[2] /= mag;
    }

    return lp;
}

//interpolation algorithm
double Interpolate(double A, double B, double FA, double FB, double X){
	//calculations
	double FX = FA + (X - A) / (B - A) * (FB - FA);
	//return y-value of given x-value
	return FX;
}


//moved main functions here for ease of multi-threading
void Generate(int framenum){
	//set screen size
	Screen screen;
	screen.width = WIDTH;
	screen.height = HEIGHT;
	vtkImageData* image = NewImage(screen.height, screen.width);

	const int npixels = screen.width * screen.height;
	unsigned char* buffer =
		(unsigned char*)image->GetScalarPointer(0, 0, 0);
	double* zbuffer = new double[npixels]; 
	//init buffers
	for (int i = 0; i < npixels; i++) {
		buffer[i * 3] = 0;
		buffer[i * 3 + 1] = 0;
		buffer[i * 3 + 2] = 0;
		zbuffer[i] = -1;
	}
	std::vector<Triangle> triangles = GetTriangles();
	std::vector<Triangle> copytris = triangles;
	Camera camera;
	screen.zbuffer = zbuffer;
	screen.buffer = buffer;

	Matrix vt, dt, ct, trans;	

	//camera init
	camera = GetCamera(framenum, 1000);
	ct = camera.CameraTransform();
	vt = camera.ViewTransform();
	dt = camera.DeviceTransform();

	//copy array and transpose
	std::copy(&triangles[0], &triangles[0]+triangles.size(), &copytris[0]);
	trans = Matrix::ComposeMatrices(Matrix::ComposeMatrices(ct, vt), dt);

	for (int i = 0; i < triangles.size(); i++) {
		Triangle* t = &copytris[i];			
		
		//Phong shading for triangle
		for(int i = 0; i < 3; i++){
			double LN = 0.0, RV = 0.0, c[3], v[3], r[3];

			//normalize camera position
			double norm1 = 0.0;
			for(int j = 0; j < 3; j++){
				c[j] = camera.position[j];
				norm1  += c[j]*c[j];
			}

			for(int j = 0; j < 3; j++){
				c[j] /= sqrt(norm1);
			}

			//dot product for lighting and normals
			for(int j = 0; j < 3; j++)
				LN += t->normals[i][j] * c[j];
				
			//calculate v
			v[0] = camera.position[0] - t->X[i];
			v[1] = camera.position[1] - t->Y[i];
			v[2] = camera.position[2] - t->Z[i];

			//calculate r
			for(int j = 0; j < 3; j++)
				r[j] = ((2 * LN * (t->normals[i][j]))) - c[j];

			//normalize
			double norm = 0.0, norm2 = 0.0;
			for(int j = 0; j < 3; j++){
				norm  += v[j]*v[j];
				norm2 += r[j]*r[j];
			}

			for(int j = 0; j < 3; j++){
				v[j] /= sqrt(norm);
				r[j] /= sqrt(norm2);
			}

			//dot product for r and v
			for(int j = 0; j < 3; j++)
				RV += (r[j] * v[j]);

			double diff = fmax(0.0, lp.Kd * LN);
			double spec = fmax(0.0, lp.Ks * pow(RV, lp.alpha)); 


			t->shading[i] = (lp.Ka + diff + spec);

		} 

		double temppoints[4];
		double newpoints[4];
		
		for(int j = 0; j < 3; j++)
		{
			temppoints[0] = t->X[j];
			temppoints[1] = t->Y[j];
			temppoints[2] = t->Z[j];
			temppoints[3] = 1;
			
			trans.TransformPoint(temppoints, newpoints);
			for(int k = 0; k < 3; k++)
				newpoints[k] = newpoints[k]/newpoints[3];
			
			t->X[j] = newpoints[0];
			t->Y[j] = newpoints[1];
			t->Z[j] = newpoints[2];
		}
		

		//read triangles
		Triangle tri = t[0], midline; //triangles
		
		//init
		int left = -1, mid = -1, right = -1;
		double splitY;
		double interMin[3], interMax[3];
		double interZMin, interZMax, interZ, interShadeMin, interShadeMax;


		//establish points left to right
		if (tri.X[0] < tri.X[1]) {
			if (tri.X[0] < tri.X[2]) {
				left = 0;
				if (tri.X[1] < tri.X[2]) {
					mid = 1;
					right = 2;
				}
				else {
					mid = 2;
					right = 1;
				}
			}
			else {
				left = 2;
				mid = 0;
				right = 1;
			}
		}
		else {
			if (tri.X[1] < tri.X[2]) {
				left = 1;
				if (tri.X[0] < tri.X[2]) {
					mid = 0;
					right = 2;
				}
				else {
					mid = 2;
					right = 0;
				}
			}
			else {
				left = 2;
				mid = 1;
				right = 0;
			}
		}
		
		//using point-slope form, establish splitY
		splitY = (tri.X[mid] - tri.X[left]) * (tri.Y[right] - tri.Y[left]) / (tri.X[right] - tri.X[left]) + tri.Y[left];

		//find greater y of mid/split and establish upper/lower of midline
		if (tri.Y[mid] > splitY){
			midline.X[0] = tri.X[mid]; //1 upper, 0 lower
			midline.X[1] = tri.X[mid];
			midline.Y[0] = splitY;
			midline.Y[1] = tri.Y[mid];
			midline.Z[0] = Interpolate(tri.X[left], tri.X[right], tri.Z[left], tri.Z[right], tri.X[mid]); 
			midline.Z[1] = tri.Z[mid];
			midline.colors[0][0] = Interpolate(tri.X[left], tri.X[right], tri.colors[left][0], tri.colors[right][0], tri.X[mid]);
			midline.colors[0][1] = Interpolate(tri.X[left], tri.X[right], tri.colors[left][1], tri.colors[right][1], tri.X[mid]);
			midline.colors[0][2] = Interpolate(tri.X[left], tri.X[right], tri.colors[left][2], tri.colors[right][2], tri.X[mid]);
			midline.colors[1][0] = tri.colors[mid][0];
			midline.colors[1][1] = tri.colors[mid][1];
			midline.colors[1][2] = tri.colors[mid][2];
			midline.shading[0] = Interpolate(tri.X[left], tri.X[right], tri.shading[left], tri.shading[right], tri.X[mid]);
			midline.shading[1] = tri.shading[mid];
		}
		else {
			midline.X[0] = tri.X[mid]; //1 upper, 0 lower
			midline.X[1] = tri.X[mid];
			midline.Y[0] = tri.Y[mid];
			midline.Y[1] = splitY;
			midline.Z[0] = tri.Z[mid];
			midline.Z[1] = Interpolate(tri.X[left], tri.X[right], tri.Z[left], tri.Z[right], tri.X[mid]);
			midline.colors[0][0] = tri.colors[mid][0];
			midline.colors[0][1] = tri.colors[mid][1];
			midline.colors[0][2] = tri.colors[mid][2];
			midline.colors[1][0] = Interpolate(tri.X[left], tri.X[right], tri.colors[left][0], tri.colors[right][0], tri.X[mid]);
			midline.colors[1][1] = Interpolate(tri.X[left], tri.X[right], tri.colors[left][1], tri.colors[right][1], tri.X[mid]);
			midline.colors[1][2] = Interpolate(tri.X[left], tri.X[right], tri.colors[left][2], tri.colors[right][2], tri.X[mid]);
			midline.shading[0] = tri.shading[mid];
			midline.shading[1] = Interpolate(tri.X[left], tri.X[right], tri.shading[left], tri.shading[right], tri.X[mid]);
		}

		//left right-triangle
		for (int j = ceil__441(tri.X[left]); j <= floor__441(tri.X[mid]); j++) {
			if (j >= 0 && j < screen.width) {
								
				//calculate column floor and ceiling (used point-slope)
				//for column
				if (tri.X[mid] - tri.X[left] != 0) { //don't divide by 0
					double colFloor = ((j - tri.X[left]) * (midline.Y[0] - tri.Y[left]) / (tri.X[mid] - tri.X[left]) + tri.Y[left]);
					double colCeil = ((j - tri.X[left]) * (midline.Y[1] - tri.Y[left]) / (tri.X[mid] - tri.X[left]) + tri.Y[left]);
					double temp = colFloor;
					if (colFloor > colCeil) {
						colFloor = colCeil;
						colCeil = temp;
					}

					//depth calculations using interpolation
					for (int l = 0; l < 3; l++) {
						interMin[l] = Interpolate(tri.X[left], midline.X[0], tri.colors[left][l], midline.colors[0][l], j);
						interMax[l] = Interpolate(tri.X[left], midline.X[1], tri.colors[left][l], midline.colors[1][l], j);
					}
					
					interShadeMin = Interpolate(tri.X[left], midline.X[0], tri.shading[left], midline.shading[0], j);
					interShadeMax = Interpolate(tri.X[left], midline.X[1], tri.shading[left], midline.shading[1], j);
					interZMin = Interpolate(tri.X[left], midline.X[0], tri.Z[left], midline.Z[0], j);
					interZMax = Interpolate(tri.X[left], midline.X[1], tri.Z[left], midline.Z[1], j);

					//for row
					for (int k = ceil__441(colFloor); k < ceil__441(colCeil); k++) {
						if (k >= 0 && k <= screen.height) {
							//z of point
							interZ = Interpolate(colFloor, colCeil, interZMin, interZMax, k);

							int pixel = j + k * screen.width;
							if (interZ > screen.zbuffer[pixel])
							{
								screen.zbuffer[pixel] = interZ;
								for (int l = 0; l < 3; l++) {
									if (pixel <= npixels * 3 && pixel >= 0) {
										buffer[pixel * 3 + l] = ceil__441(fmin(Interpolate(colFloor, colCeil, interMin[l], interMax[l], k) * 
											Interpolate(colFloor, colCeil, interShadeMin, interShadeMax, k) * 255.0, 255.0));
									}
								}
							}
						}
					}
				}
			}
		}
		//right right-triangle
		for (int j = ceil__441(tri.X[mid]); j <= floor__441(tri.X[right]); j++) {
			if (j >= 0 && j < screen.width) {
				
				//calculate column floor and ceiling (used point-slope)
				//for column
				if (tri.X[right] - tri.X[mid] != 0) { //don't divide by 0
					double colFloor = ((j - tri.X[right]) * (midline.Y[0] - tri.Y[right]) / (tri.X[mid] - tri.X[right]) + tri.Y[right]);
					double colCeil = ((j - tri.X[right]) * (midline.Y[1] - tri.Y[right]) / (tri.X[mid] - tri.X[right]) + tri.Y[right]);
					double temp = colFloor;
					if (colFloor > colCeil) {
						colFloor = colCeil;
						colCeil = temp;
					}


					//depth calculations using interpolation
					for (int l = 0; l < 3; l++) {
						interMin[l] = Interpolate(tri.X[right], midline.X[0], tri.colors[right][l], midline.colors[0][l], j);
						interMax[l] = Interpolate(tri.X[right], midline.X[1], tri.colors[right][l], midline.colors[1][l], j);
					}
					interShadeMin = Interpolate(tri.X[right], midline.X[0], tri.shading[right], midline.shading[0], j);
					interShadeMax = Interpolate(tri.X[right], midline.X[1], tri.shading[right], midline.shading[1], j);
					interZMin = Interpolate(tri.X[right], midline.X[0], tri.Z[right], midline.Z[0], j);
					interZMax = Interpolate(tri.X[right], midline.X[1], tri.Z[right], midline.Z[1], j);

					//for row
					for (int k = ceil__441(colFloor); k < ceil__441(colCeil); k++) {
						if (k >= 0 && k <= screen.height) {
							// z of point
							interZ = Interpolate(colFloor, colCeil, interZMin, interZMax, k);

							int pixel = j  + k * screen.width;
							if (interZ > screen.zbuffer[pixel])
							{
								screen.zbuffer[pixel] = interZ;
								for (int l = 0; l < 3; l++) {
									if (pixel <= npixels * 3 && pixel >= 0) {
										buffer[pixel * 3 + l] = ceil__441(fmin(Interpolate(colFloor, colCeil, interMin[l], interMax[l], k) * 
											Interpolate(colFloor, colCeil, interShadeMin, interShadeMax, k)* 255.0, 255.0));
									}
								}
							}
						}
					}
				}
			}
		}
		}
		//filename
		char loc[9] = "frame";
		sprintf(loc, "frame%03d", framenum);
		
		WriteImage(image, loc);
		
}

int main()
{
	#ifdef VIDEO
		//multi-threaded for time
		for(int i = 0; i < FRAMES; i += THREADS){
			thread *th = new thread[THREADS];
			for(int j = 0; j < THREADS && (j + i) < FRAMES; j++){
				cerr << "Reading " << (j + i) << endl;
				th[j] = thread(Generate, j + i);
			}

			//join threads
			for(int j = 0; j < THREADS && (j + i) < FRAMES; j++){
				th[j].join();
				cerr << "Done reading " << (j + i) << endl;
			}
		}
	#else
		Generate(0);
	#endif
}