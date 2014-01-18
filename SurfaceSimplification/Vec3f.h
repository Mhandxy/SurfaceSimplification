#pragma once
#include "Simplification.h"

namespace SimpleOBJ
{
    class Vec3f
    {
    public:

        //Constructors
        Vec3f();
        Vec3f(double x,double y, double z);
        Vec3f(const Vec3f& v);
        //Deconstructor
        virtual ~Vec3f();
    public:
        //Operators

        //Operator []
        __forceinline double& operator [](int index)
        {
            ASSERT(index>=0&&index<3);
            return _p[index];
        }
        __forceinline const double& operator [](int index) const
        {
            ASSERT(index>=0&&index<3);
            return _p[index];
        }
        
        //Operator =
        Vec3f& operator = (const Vec3f& v);

        //Operators +=,-=, *=, /=
        void operator +=(const Vec3f& v);
        void operator +=(double f);
        void operator -=(const Vec3f& v);
        void operator -=(double f);
        void operator *=(const Vec3f& v);
        void operator *=(double f);
        void operator /=(const Vec3f& v);
        void operator /=(double f);

        //Operators +,-.*,/
        Vec3f operator +(const Vec3f&v) const;
        Vec3f operator +(double f) const;
        Vec3f operator -(const Vec3f&v) const;
        Vec3f operator -(double f) const;
        Vec3f operator *(const Vec3f&v) const;
        Vec3f operator *(double f) const;
        Vec3f operator /(const Vec3f&v) const;
        Vec3f operator /(double f) const;

        Vec3f operator -() const;
        
        double Length();
        friend double DotProduct(Vec3f& l,Vec3f& r);
        friend Vec3f CrossProduct(Vec3f& l,Vec3f& r);
    public:
        void Normalize();
        double L2Norm_Sqr();
     
    public:
        union
        {
            struct
            { double _p[3]; };
            struct
            { double x,y,z; };
            struct
            { double r,g,b; };
        };
        enum {_len = 3};   
        
    };
}

 