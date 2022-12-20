#include"initial.h"

vec3 Screen::CenterCal(int i, int j, int w, int h)
{
    float xur = ul[0]-(ul[0]-ur[0])*(i+1)/w;
    float zur = ul[2]-(ul[2]-ur[2])*(i+1)/w;
    float xul = ul[0]-(ul[0]-ur[0])*i/w;
    float zul = ul[2]-(ul[2]-ur[2])*i/w;
    float z1 = (zul-zur)/2;

    float yll = ul[1]-(ul[1]-ll[1])*(j+1)/h;
    float zll = ul[2]-(ul[2]-ll[2])*(j+1)/h;
    float ylu = ul[1]-(ul[1]-ll[1])*j/h;
    float zlu = ul[2]-(ul[2]-ll[2])*j/h;
    float z2 = (zlu-zll)/2;

    vec3 p;
    p.set((xul+xur)/2, (yll+ylu)/2, 0);//(z1+z2)/2);
    return p;
}

void Object::Initial(vec3 start)
{
    e = start;
    intersection = {0, 0, 0};
    index = 0;
    mode = 0;
    dis = 0;
    n = {0, 0, 0};
    l = {0, 0, 0};
    h = {0, 0, 0};
    v = {0, 0, 0};
}

void Object::Set(vec3 start, vec3 v, vec3 lightPosition, Scene s)
{
    e = start;
    //intersection = e+dis*v;
    if(mode==1)
    {
        n = intersection - s.sphere[index].O;
    }
    else if(mode==2)
    {
        vec3 A1, A2;
        A1 = s.triangle[index].P[1] - s.triangle[index].P[0];
        A2 = s.triangle[index].P[2] - s.triangle[index].P[0];
        n = A1^A2;
        if(n[1]<0)
        {
            n = n*(-1);
        }
    }
    n = n.normalize();
    l = lightPosition - intersection;
    v = start - intersection;
    //v = v.normalize();
    h = v.normalize()+l.normalize();
    h = h.normalize();
    l = l.normalize();
}
