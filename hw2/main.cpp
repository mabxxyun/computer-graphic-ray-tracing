#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <cmath>
#include <algorithm>
#include "algebra3.h"
#include "initial.h"
using namespace std;
#define PI 3.1415926

vec3 E, D, U;
vec3 L;
int W, H;
float A;
Material m;
Scene scene;
Screen screen;
float dist = 1;
vec3 Li = {1, 1, 1};

int readFile()
{
    FILE *fp = fopen("hw2_input.txt", "r");

    if(fp==0)
    {
        return 1;
    }
    while(!feof(fp))
    {
        char c = fgetc(fp);
        fgetc(fp);
        if(c=='E')
        {
            float x, y, z;
            fscanf(fp, "%f %f %f", &x, &y, &z);
            E.set(x, y, z);
        }
        if(c=='R')
        {
            fscanf(fp, "%d %d", &W, &H);
        }
        if(c=='V')
        {
            float x1, y1, z1, x2, y2, z2;
            fscanf(fp, "%f %f %f %f %f %f", &x1, &y1, &z1, &x2, &y2, &z2);
            D.set(x1, y1, z1);
            U.set(x2, y2, z2);
        }
        if(c=='F')
        {
            fscanf(fp, "%f", &A);
            A *= (PI/180.0f);
        }
        if(c=='M')
        {
            fscanf(fp, "%f %f %f %f %f %f %f %f", &m.color[0], &m.color[1], &m.color[2], &m.Ka, &m.Kd, &m.Ks, &m.exp, &m.Reflect);
        }
        if(c=='S')
        {
            float x, y,z;
            fscanf(fp, "%f %f %f %f", &x, &y, &z, &scene.sphere[scene.sIndex].R);
            scene.sphere[scene.sIndex].O.set(x, y, z);
            scene.sphere[scene.sIndex].M = m;
            scene.sIndex += 1;
        }
        if(c=='T')
        {
            for(int i=0; i<3; i+=1)
            {
                float x, y, z;
                fscanf(fp, "%f %f %f", &x, &y, &z);
                scene.triangle[scene.tIndex].P[i].set(x, y, z);
            }
            scene.triangle[scene.tIndex].M = m;
            scene.tIndex += 1;
        }
        if(c=='L')
        {
            float x, y, z;
            fscanf(fp, "%f %f %f", &x, &y, &z);
            L.set(x, y, z);
        }
        fgetc(fp);
    }

    fclose(fp);
    return 0;
}


struct Pixel
{
    unsigned char R, G, B;  // Blue, Green, Red
};

class ColorImage
{
    Pixel *pPixel;
    int xRes, yRes;
public:
    ColorImage();
    ~ColorImage();
    void init(int xSize, int ySize);
    void clear(Pixel background);
    Pixel readPixel(int x, int y);
    void writePixel(int x, int y, Pixel p);
    void outputPPM(char *filename);
};

ColorImage::ColorImage()
{
    pPixel = 0;
}

ColorImage::~ColorImage()
{
    if (pPixel) delete[] pPixel;
    pPixel = 0;
}

void ColorImage::init(int xSize, int ySize)
{
    Pixel p = {0,0,0};
    xRes = xSize;
    yRes = ySize;
    pPixel = new Pixel[xSize*ySize];
    clear(p);
}

void ColorImage::clear(Pixel background)
{
    int i;

    if (! pPixel) return;
    for (i=0; i<xRes*yRes; i++) pPixel[i] = background;
}

Pixel ColorImage::readPixel(int x, int y)
{
    assert(pPixel); // die if image not initialized
    return pPixel[x + y*yRes];
}

void ColorImage::writePixel(int x, int y, Pixel p)
{
    assert(pPixel); // die if image not initialized
    pPixel[x + y*xRes] = p;
}

void ColorImage::outputPPM(char *filename)
{
    FILE *outFile = fopen(filename, "wb");

    assert(outFile); // die if file can't be opened

    fprintf(outFile, "P6 %d %d 255\n", xRes, yRes);
    fwrite(pPixel, 1, 3*xRes*yRes, outFile );

    fclose(outFile);
}

void ScreenCal()
{
    vec3 V = U^D;
    vec3 K = D^V;
    vec3 CenterPoint = E + dist*D.normalize();
    float d = dist*tan(A);
    vec3 BorderR = CenterPoint;
    BorderR[0] -= d;
    vec3 BorderL = CenterPoint;
    BorderL[0] += d;
    screen.ul = BorderL;
    screen.ul[1] += d;
    screen.ur = BorderR;
    screen.ur[1] += d;
    screen.ll = BorderL;
    screen.ll[1] -= d;
    screen.lr = BorderR;
    screen.lr[1] -= d;
}

Object obj;
bool Intersect(Ray r, int mode, int index)
{
    obj.Initial(r.S);
    bool HasGraph = false;
    for(int i=0; i<scene.sIndex; i+=1)
    {
        if((mode==1 && i!=index)||(mode!=1))
        {
            float a = r.V.length2();
            float b = 2*r.V*(r.S-scene.sphere[i].O);
            vec3 t = r.S-scene.sphere[i].O;
            float c = t.length2() - powf(scene.sphere[i].R, 2);
            if((b*b-4*a*c) >= 0 )
            {
                float t = ((-1)*b-sqrt(b*b-4*a*c))/(2*a);
                if(t>=0)
                {
                    if(HasGraph)
                    {
                        if(t<obj.dis)
                        {
                            obj.mode = 1;
                            obj.index = i;
                            obj.dis = t;
                            obj.intersection = r.S + t*r.V;
                        }
                    }
                    else if(!HasGraph)
                    {
                        HasGraph = true;
                        obj.mode = 1;
                        obj.index = i;
                        obj.dis = t;
                        obj.intersection = r.S + t*r.V;
                    }
                }
            }
        }
    }
    for(int i=0; i<scene.tIndex; i+=1)
    {
        if((mode==2 && i!=index) || mode!=2)
        {
            vec3 A = scene.triangle[i].P[1]-scene.triangle[i].P[0];
            vec3 B = scene.triangle[i].P[2]-scene.triangle[i].P[0];
            mat3 Mat = {{(-1)*r.V[0], A[0], B[0]}, {(-1)*r.V[1], A[1], B[1]}, {(-1)*r.V[2], A[2], B[2]}};
            float Det = r.V[0]*(B[1]*A[2]-A[1]*B[2])+r.V[1]*(A[0]*B[2]-B[0]*A[2])+r.V[2]*(B[0]*A[1]-A[0]*B[1]);
            if(Det!=0)
            {
                mat3 InverseMat = Mat.inverse();
                vec3 C = r.S - scene.triangle[i].P[0];
                float t = InverseMat[0]*C;
                float u = InverseMat[1]*C;
                float v = InverseMat[2]*C;
                if(t>=0 && (u+v)<=1 && u>=0 && v>=0 && u<=1 && v<=1)
                {
                    if(HasGraph)
                    {
                        if(t<obj.dis)
                        {
                            obj.mode = 2;
                            obj.index = i;
                            obj.dis = t;
                            obj.intersection = r.S + t*r.V;
                        }
                    }
                    else
                    {
                        HasGraph = true;
                        obj.mode = 2;
                        obj.index = i;
                        obj.dis = t;
                        obj.intersection = r.S + t*r.V;
                    }
                }
            }
        }

    }
    return HasGraph;
}

vec3 PhongLight(Object o)
{
    float Ambient, Diffuse, Specular;
    vec3 c;
    if(o.mode==1)
    {
        Ambient = scene.sphere[o.index].M.Ka;
        Diffuse = scene.sphere[o.index].M.Kd*max(o.n*o.l, 0.0f);
        Specular = scene.sphere[o.index].M.Ks*powf(max(o.n*o.h, 0.0f), scene.sphere[o.index].M.exp);
        c = (Ambient + Diffuse + Specular)*scene.sphere[o.index].M.color;
    }
    else if(o.mode==2)
    {
        Ambient = scene.triangle[o.index].M.Ka;
        Diffuse = scene.triangle[o.index].M.Kd*max(o.n*o.l, 0.0f);
        Specular = scene.triangle[o.index].M.Ks*powf(max(o.n*o.h, 0.0f), scene.triangle[o.index].M.exp);
        c = (Ambient + Diffuse + Specular)*scene.triangle[o.index].M.color;
    }

    return c;
}
int MaxRecursive = 10;
int cnt=0;
vec3 ColorCal(Ray r, Object o)
{
    o.Set(r.S, r.V, L, scene);
    vec3 origin_color = PhongLight(o);
    if(o.mode == 1)
    {
        if(scene.sphere[o.index].M.Reflect == 0)
        {
            return origin_color;
        }
        else
        {
            Ray reflect;
            reflect.S = o.intersection;
            reflect.V = r.V - 2*(r.V*o.n)*o.n;
            reflect.V = reflect.V.normalize();
            if(Intersect(reflect, o.mode, o.index))
            {
                cnt+=1;
                if(cnt<MaxRecursive)
                {
                    vec3 reflect_color = ColorCal(reflect, obj);
                    return origin_color*(1-scene.sphere[o.index].M.Reflect)+reflect_color*scene.sphere[o.index].M.Reflect;
                }
                else
                    return origin_color;
            }
            else
            {
                return origin_color;
            }
        }
    }
    else if(o.mode == 2)
    {
        if(scene.triangle[o.index].M.Reflect == 0)
        {
            return origin_color;
        }
        else
        {
            Ray reflect;
            reflect.S = o.intersection;
            reflect.V = r.V - 2*(r.V*o.n)*o.n;
            reflect.V.normalize();
            if(Intersect(reflect, o.mode, o.index))
            {
                cnt+=1;
                if(cnt<MaxRecursive)
                {
                    vec3 reflect_color = ColorCal(reflect, obj);
                    return origin_color*(1-scene.triangle[o.index].M.Reflect)+reflect_color*scene.triangle[o.index].M.Reflect;
                }
                else
                    return origin_color;
            }
            else
            {
                return origin_color;
            }
        }
    }
    return 0;
}

// A test program that generates varying shades of reds.
int main(int argc, char* argv[])
{
    if(readFile()==1)
    {
        cout<<"The file can't be opened."<<endl;
    }

    ScreenCal();

    Ray R;
    R.S = E;

    ColorImage image;
    int x, y;
    Pixel p= {0,0,0};

    image.init(W, H);
    for (y=0; y<H; y++)
    {
        for (x=0; x<W; x++)
        {
            obj.Initial(E);
            vec3 ScreenPixel = screen.CenterCal(x, y, W, H);
            R.S = E;
            R.V = ScreenPixel - R.S;
            if(Intersect(R, 0, MaxNum+1))
            {
                cnt=0;
                vec3 color = ColorCal(R, obj);
                color = color*255;
                int r = color[0];
                int g = color[1];
                int b = color[2];
                if(r>255)
                    r = 255;
                if(g>255)
                    g = 255;
                if(b>255)
                    b = 255;
                if(r<0)
                    r = 0;
                if(g<0)
                    g = 0;
                if(b<0)
                    b = 0;
                p = {r, g, b};
            }
            else
            {
                p = {255, 255, 255};
            }
            image.writePixel(x, y, p);
        }
    }

    image.outputPPM("HW2.ppm");
}

