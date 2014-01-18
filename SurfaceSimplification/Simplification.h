#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <math.h>
#include <list>
#include <time.h>
void set_time_base(void);
double get_time_in_seconds(void);
using namespace std;

#define SMALL(x) (abs(x)<0.01f? 1:0)
#define ASSERT assert

namespace SimpleOBJ
{
    //面片的abcd四个参数
    class Surface
    {
    public:
        int index[3];
        double para[4];
    };

    class VPair
    {
    public:
        VPair(int _i,int _j,double _c=0):i(_i),j(_j),cost(_c){}
        VPair(){}
        int i;
        int j;
        double cost;

        static bool costcompflag;
        bool operator<(VPair &r)
        {
            if(costcompflag)
                return cost>r.cost;
            if(i<r.i)
                return true;
            if(i>r.i)
                return false;
            return j<r.j;
        }
        bool operator>(VPair &r)
        {
            if(costcompflag)
                return cost<r.cost;
            if(i>r.i)
                return true;
            if(i<r.i)
                return false;
            return j>r.j;
        }
        bool operator==(VPair &r)
        {
            if(costcompflag)
                return cost==r.cost;
            else
                return i==r.i&&j==r.j;
        }
    };

    class CSimpleObject;

    class Heap
    {
    public:
        Heap(int tnum);
        vector<VPair> vpairs;
        void HeapUp(int i);
        void HeapDown(int i);
        void Build();
        void HeapSort();
        void UpdateCost(int i,double cost);
        VPair RemoveMinVPair();
        void RemoveVPair(VPair& vp,CSimpleObject* pobj);
        int heapsize;
    };

    class QMat {
    public:
        QMat() {memset(_q,0,10*sizeof(double));}
        QMat(double _q11, double _q12, double _q13, double _q14, 
                         double _q22, double _q23, double _q24,
                                     double _q33, double _q34,
                                                 double _q44) 
        {
            q11=_q11;q12=_q12;q13=_q13;q14=_q14;
                     q22=_q22;q23=_q23;q24=_q24;
                              q33=_q33;q34=_q34;
                                       q44=_q44;
        }
        QMat(double* tripara)
        {
            double a = tripara[0];
            double b = tripara[1];
            double c = tripara[2];
            double d = tripara[3];
            q11=a*a; q12=a*b; q13=a*c; q14=a*d;
                     q22=b*b; q23=b*c; q24=b*d;
                              q33=c*c; q34=c*d;
                                       q44=d*d;
        }

        double operator[](int c) {return _q[c];}

        QMat operator+(QMat& r)
        { 
            return QMat( 
                _q[0]+r[0], _q[1]+r[1], _q[2]+r[2], _q[3]+r[3], 
                            _q[4]+r[4], _q[5]+r[5], _q[6]+r[6], 
                                        _q[7]+r[7], _q[8]+r[8],
                                                    _q[9]+r[9]);
        }
        
        QMat& operator+=(QMat& r)
        {
            for(int i=0;i<10;++i)
                _q[i]+=r[i];
            return *this;
        }
    public:
        union
        {
            struct
            { double _q[10]; };
            struct
            { double q11,q12,q13,q14,q22,q23,q24,q33,q34,q44; };
        };
    };
}