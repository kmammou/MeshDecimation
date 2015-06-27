/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

//#define _CRTDBG_MAP_ALLOC

#ifdef _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _CRTDBG_MAP_ALLOC

#include "mdMeshDecimator.h"

using namespace MeshDecimation;
using namespace std;

void CallBack(const char * msg)
{
    cout << msg;
}
bool LoadOFF(const string         & fileName, 
             vector< Vec3<Float> > & points, 
             vector< Vec3<int> > & triangles);
bool LoadOBJ(const string         & fileName, 
		     vector< Vec3<Float> > & points,
             vector< Vec3<int> > & triangles);
bool SaveOBJ(const std::string          & fileName, 
		     const vector< Vec3<Float> > & points,
             const vector< Vec3<int> > & triangles);
void GetFileExtension(const string & fileName, string & fileExtension);

int main(int argc, char * argv[])
{
    if (argc != 6)
    { 
        cout << "Usage: ./MeshSimplification fileName targetNTrianglesDecimatedMesh targetNVerticesDecimatedMesh maxDecimationError fileNameOut.obj"<< endl;
        cout << "Recommended parameters: ./MeshSimplification input.obj 1000 500 1.0 decimated.obj"<< endl;
        return -1;
    }

    const string  fileName(argv[1]);                                      // table.obj
    size_t        targetNTrianglesDecimatedMesh = atoi(argv[2]);          // 1000
	size_t		  targetNVerticesDecimatedMesh  = atoi(argv[3]);          // 500
	Float		  maxDecimationError = static_cast<Float>(atof(argv[4])); // 1.0
    const string  fileNameOut(argv[5]);                                   // decimated_table.obj

    cout << "+ Parameters" << std::endl;
    cout << "\t input                      " << fileName                      << std::endl;
    cout << "\t target # triangles         " << targetNTrianglesDecimatedMesh << std::endl;
	cout << "\t target # vertices          " << targetNVerticesDecimatedMesh  << std::endl;
	cout << "\t max. decimation error      " << maxDecimationError		      << std::endl;
	cout << "\t output                     " << fileNameOut				      << std::endl;

    cout << "+ Load mesh " << endl;

	// Load input mesh
	vector< Vec3<Float> > points;
    vector< Vec3<int> > triangles;
    string fileExtension;
    GetFileExtension(fileName, fileExtension);
    if (fileExtension == ".OFF")
    {
        if (!LoadOFF(fileName, points, triangles))
        {
            return -1;
        }
    }
    else if (fileExtension == ".OBJ")
    {
        if (!LoadOBJ(fileName, points, triangles))
        {
            return -1;
        }
    }
    else
    {
        cout << "Format not supported!" << endl;
        return -1;
    }

	// decimate mesh
    MeshDecimator myMDecimator;
    myMDecimator.SetCallBack(&CallBack);
    myMDecimator.Initialize(points.size(), triangles.size(), &points[0], &triangles[0]);
	myMDecimator.Decimate(targetNVerticesDecimatedMesh, 
						  targetNTrianglesDecimatedMesh, 
						  maxDecimationError);

	// allocate memory for decimated mesh
	vector< Vec3<Float> > decimatedPoints;
    vector< Vec3<int> > decimatedtriangles;
    decimatedPoints.resize(myMDecimator.GetNVertices());
    decimatedtriangles.resize(myMDecimator.GetNTriangles());

	// retreive decimated mesh
    myMDecimator.GetMeshData(&decimatedPoints[0], &decimatedtriangles[0]);

    SaveOBJ(fileNameOut, decimatedPoints, decimatedtriangles);

#ifdef _CRTDBG_MAP_ALLOC
    _CrtDumpMemoryLeaks();
#endif // _CRTDBG_MAP_ALLOC
    return 0;
}

void GetFileExtension(const string & fileName, string & fileExtension) 
{
    size_t lastDotPosition = fileName.find_last_of(".");
    if (lastDotPosition == string::npos)
    {
        fileExtension = "";
    }
    else
    {
        fileExtension = fileName.substr(lastDotPosition, fileName.size());
        transform(fileExtension.begin(), fileExtension.end(),fileExtension.begin(), ::toupper);
    }
}
bool LoadOFF(const string & fileName, 
			 vector< Vec3<Float> > & points,
             vector< Vec3<int> > & triangles) 
{    
    FILE * fid = fopen(fileName.c_str(), "r");
    if (fid) 
    {
        const string strOFF("OFF");
        char temp[1024];
        fscanf(fid, "%s", temp);
        if (string(temp) != strOFF)
        {
            printf( "Loading error: format not recognized \n");
            fclose(fid);
            return false;
        }
        else
        {
            int nv = 0;
            int nf = 0;
            int ne = 0;
            fscanf(fid, "%i", &nv);
            fscanf(fid, "%i", &nf);
            fscanf(fid, "%i", &ne);
            points.resize(nv);
            triangles.resize(nf);
			Vec3<Float> coord;
            float x = 0;
            float y = 0;
            float z = 0;
            for (int p = 0; p < nv ; p++) 
            {
                fscanf(fid, "%f", &x);
                fscanf(fid, "%f", &y);
                fscanf(fid, "%f", &z);
                points[p].X() = x;
                points[p].Y() = y;
                points[p].Z() = z;
            }        
            int i = 0;
            int j = 0;
            int k = 0;
            int s = 0;
            for (int t = 0; t < nf ; ++t) {
                fscanf(fid, "%i", &s);
                if (s == 3)
                {
                    fscanf(fid, "%i", &i);
                    fscanf(fid, "%i", &j);
                    fscanf(fid, "%i", &k);
                    triangles[t].X() = i;
                    triangles[t].Y() = j;
                    triangles[t].Z() = k;
                }
                else            // Fix me: support only triangular meshes
                {
                    for(int h = 0; h < s; ++h) fscanf(fid, "%i", &s);
                }
            }
            fclose(fid);
        }
    }
    else 
    {
        printf( "Loading error: file not found \n");
        return false;
    }
    return true;
}
bool LoadOBJ(const string & fileName, 
			 vector< Vec3<Float> > & points,
             vector< Vec3<int> > & triangles) 
{   
    const char ObjDelimiters[]=" /";
    const unsigned int BufferSize = 1024;
    FILE * fid = fopen(fileName.c_str(), "r");
    
    if (fid) 
    {        
        char buffer[BufferSize];
        Vec3<int> ip;
        Vec3<int> in;
        Vec3<int> it;
        char * pch;
        char * str;
        size_t nn = 0;
        size_t nt = 0;
		Vec3<Float> x;
        while (!feof(fid)) 
        {
            if (!fgets(buffer, BufferSize, fid))
            {
                break;
            }
            else if (buffer[0] == 'v')
            {
                if (buffer[1] == ' ')
                {
                    str = buffer+2;
                    for(int k = 0; k < 3; ++k)
                    {
                        pch = strtok (str, " ");
						if (pch) x[k] = static_cast<Float>(atof(pch));
                        else
                        {
                            return false;
                        }
                        str = NULL;
                    }
                    points.push_back(x);
                }
                else if (buffer[1] == 'n')
                {
                    ++nn;
                }
                else if (buffer[1] == 't')
                {
                    ++nt;
                }
            }
            else if (buffer[0] == 'f')
            {

                str = buffer+2;
                for(int k = 0; k < 3; ++k)
                {
                    pch = strtok (str, ObjDelimiters);
                    if (pch) ip[k] = atoi(pch) - 1;
                        else
                        {
                            return false;
                        }
                    str = NULL;
                    if (nt > 0)
                    {
                        pch = strtok (NULL, ObjDelimiters);
                        if (pch)  it[k] = atoi(pch) - 1;
                        else
                        {
                            return false;
                        }
                    }
                    if (nn > 0)
                    {
                        pch = strtok (NULL, ObjDelimiters);
                        if (pch)  in[k] = atoi(pch) - 1;
                        else
                        {
                            return false;
                        }
                    }
                }
                triangles.push_back(ip);
            }
        }
        fclose(fid);
    }
    else 
    {
        cout << "File not found" << endl;
        return false;
    }
    return true;
}
bool SaveOBJ(const std::string          & fileName, 
			 const vector< Vec3<Float> > & points,
             const vector< Vec3<int> > & triangles)
{
    std::cout << "Saving " <<  fileName << std::endl;
    std::ofstream fout(fileName.c_str());
    if (fout.is_open()) 
    {           
        const size_t nV = points.size();
        const size_t nT = triangles.size();
        for(size_t v = 0; v < nV; v++)
        {
            fout << "v " << points[v][0] << " " 
                         << points[v][1] << " " 
                         << points[v][2] << std::endl;
        }
        for(size_t f = 0; f < nT; f++)
        {
            fout <<"f " << triangles[f][0]+1 << " " 
                        << triangles[f][1]+1 << " "
                        << triangles[f][2]+1 << std::endl;
        }
        fout.close();
        return true;
    }
    return false;
}


