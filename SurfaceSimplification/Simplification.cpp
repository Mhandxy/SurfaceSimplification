#include "SimpleObject.h"
#define PARENT(i) (i>>1)
#define LEFT(i) (i<<1)
#define RIGHT(i) (1+(i<<1))
#define HEAPSIZE (int(vpairs.size()))
#define VP(i) (vpairs[(i)-1])

#define COST(v,Q) (\
        (v).x*((v).x*(Q).q11+(v).y*(Q).q12+(v).z*(Q).q13+(Q).q14)+\
        (v).y*((v).x*(Q).q12+(v).y*(Q).q22+(v).z*(Q).q23+(Q).q24)+\
        (v).z*((v).x*(Q).q13+(v).y*(Q).q23+(v).z*(Q).q33+(Q).q34)+\
              ((v).x*(Q).q14+(v).y*(Q).q24+(v).z*(Q).q34+(Q).q44))

#define DET(q11,q12,q13,q21,q22,q23,q31,q32,q33) \
    ((q11)*(q22)*(q33)+(q12)*(q23)*(q31)+(q13)*(q21)*(q32)\
    -(q13)*(q22)*(q31)-(q12)*(q21)*(q33)-(q11)*(q23)*(q32))

namespace SimpleOBJ
{
    bool VPair::costcompflag=false;

    Heap::Heap(int tnum):vpairs(3*tnum,VPair())
    {
    }
    
    void Heap::HeapUp(int i)
    {
        while(i>1&&VP(PARENT(i))<VP(i))
        {
            swap(VP(PARENT(i)),VP(i));
            i=PARENT(i);
        }
    }

    VPair Heap::RemoveMinVPair()
    {
        VPair ret=VP(1);
        swap(VP(1),VP(heapsize));
        heapsize--;
        HeapDown(1);
        vpairs.erase(--vpairs.end());
        return ret;
    }

    void Heap::RemoveVPair(VPair& vp,CSimpleObject* pobj)
    {
        //删除多余顶点对
        int hs=heapsize;
        for(int i=heapsize;i>=1;--i)
        {
            if(VP(i).i==vp.j||VP(i).j==vp.j||VP(i).i==vp.i||VP(i).j==vp.i)
            {
                swap(VP(i),VP(heapsize));
                heapsize--;
                HeapDown(i);
            }
        }
        auto iter=vpairs.end();
        int dvp=hs-heapsize;
        for(int i=0;i<dvp;++i)
        {
            iter--;
            if(iter->i==vp.j) iter->i=vp.i;
            else if(iter->j==vp.j) iter->j=vp.i;
            if(iter->i>iter->j)
                swap(iter->i,iter->j);
        }
        hs=HEAPSIZE;
        VPair::costcompflag=false;
        sort(iter,vpairs.end());
        vpairs.erase(unique(iter,vpairs.end()),vpairs.end());
        VPair::costcompflag=true;
        //更新代价
        for(int i=0;i<dvp-(hs-HEAPSIZE);++i)
        {
            heapsize++;
            VP(heapsize).cost=pobj->GetCost(VP(heapsize).i,VP(heapsize).j);
            HeapUp(heapsize);
        }
    }

    void Heap::HeapSort()
    {
        for(int i=HEAPSIZE;i>=2;--i)
        {
            swap(VP(1),VP(i));
            heapsize--;
            HeapDown(1);
        }
    }

    void Heap::HeapDown(int i)
    {
        int l=LEFT(i);
        int r=RIGHT(i);
        int max;
        if(l<=heapsize&&VP(l)>VP(i))
            max=l;
        else
            max=i;
        if(r<=heapsize&&VP(r)>VP(max))
            max=r;
        if(max!=i)
        {
            swap(VP(i),VP(max));
            HeapDown(max);
        }
    }

    void Heap::UpdateCost(int i,double cost)
    {
        VP(i).cost=cost;
        HeapUp(i);
        HeapDown(i);
    }

    void Heap::Build()
    {
        VPair::costcompflag=true;
        heapsize=HEAPSIZE;
        for(int i=int(HEAPSIZE/2);i>=1;--i)
            HeapDown(i);
    }

    bool Solve(QMat& Q,Vec3f& v)
    {
        double D=DET(Q.q11,Q.q12,Q.q13,Q.q12,Q.q22,Q.q23,Q.q13,Q.q23,Q.q33);
        if(D)
        {
            v.x=(DET(-Q.q14,Q.q12,Q.q13,-Q.q24,Q.q22,Q.q23,-Q.q34,Q.q23,Q.q33))/D;
            v.y=(DET(Q.q11,-Q.q14,Q.q13,Q.q12,-Q.q24,Q.q23,Q.q13,-Q.q34,Q.q33))/D;
            v.z=(DET(Q.q11,Q.q12,-Q.q14,Q.q12,Q.q22,-Q.q24,Q.q13,Q.q23,-Q.q34))/D;
            return true;
        }
        return false;
    }

    bool CSimpleObject::Simplification(double level)
    {
        if(level>1)
            return false;
        SimplificationInit();
        int tn=int(faces.size());
        while(double(faces.size())/tn>level)
        {
            //在堆中移除顶点
            VPair vp=heap->RemoveMinVPair();
            //计算最优目标顶点
            Vec3f v;
            GetCost(vp.i,vp.j,v);
            vertexs[vp.i]=v;
            qmats[vp.i]+=qmats[vp.j];
            //在object中移除顶点和面，调整QMat和代价
            RemoveVertice(vp);
        }
        Adjust();

        return true;
    }

    void CSimpleObject::SimplificationInit()
    {
        //读入所有顶点和面
        vertexs=vector<Vec3f>(m_nVertices);
        for(int i=0;i<m_nVertices;++i)
        {
            vertexs[i]=m_pVertexList[i];
        }
        faces=vector<Surface>(m_nTriangles);
        for(int i=0;i<m_nTriangles;++i)
        {
            faces[i].index[0]=m_pTriangleList[i][0];
            faces[i].index[1]=m_pTriangleList[i][1];
            faces[i].index[2]=m_pTriangleList[i][2];
            Vec3f& p0=m_pVertexList[m_pTriangleList[i][0]];
            Vec3f& p1=m_pVertexList[m_pTriangleList[i][1]];
            Vec3f& p2=m_pVertexList[m_pTriangleList[i][2]];
            Vec3f l[2];
            l[0]=p1-p0;
            l[1]=p2-p0;
            Vec3f n=CrossProduct(l[0],l[1]);
            n.Normalize();
            double d=-DotProduct(n,p0);
            faces[i].para[0]=n.x;
            faces[i].para[1]=n.y;
            faces[i].para[2]=n.z;
            faces[i].para[3]=d;
        }
        //初始化误差矩阵QMat
        InitQMat();
        //选择点和构造堆
        SelectVPairs();
    }
    void CSimpleObject::InitQMat()
    {
        qmats=vector<QMat>(int(vertexs.size()));
        for(int i=0;i<int(faces.size());++i)
        {
            for(int j=0;j<3;++j)
            {
                qmats[faces[i].index[j]]+=QMat(faces[i].para);
            }
        }
    }
    
    void CSimpleObject::SelectVPairs()
    {
        if(heap==NULL)
            heap=new Heap(int(faces.size()));
        //边
        for(int i=0;i<int(faces.size());++i)
        {
            heap->vpairs[3*i].i=min(faces[i].index[0],faces[i].index[1]);
            heap->vpairs[3*i].j=max(faces[i].index[0],faces[i].index[1]);
            heap->vpairs[3*i+1].i=min(faces[i].index[0],faces[i].index[2]);
            heap->vpairs[3*i+1].j=max(faces[i].index[0],faces[i].index[2]);
            heap->vpairs[3*i+2].i=min(faces[i].index[2],faces[i].index[1]);
            heap->vpairs[3*i+2].j=max(faces[i].index[2],faces[i].index[1]);
        }
        if(threshold>0)
        {
            //非边
            for(int i=0;i<int(vertexs.size());++i)
            {
                for(int j=i+1;j<int(vertexs.size());++j)
                {
                    if((vertexs[i]-vertexs[j]).Length()<threshold)
                    {
                        heap->vpairs.push_back(VPair(i,j));
                    }
                }
            }
        }
        //去重
        VPair::costcompflag=false;
        sort(heap->vpairs.begin(),heap->vpairs.end());
        heap->vpairs.erase(unique(heap->vpairs.begin(),heap->vpairs.end()),heap->vpairs.end());
        VPair::costcompflag=true;
        for(auto iter=heap->vpairs.begin();iter!=heap->vpairs.end();++iter)
        {
            int aa=iter->i;
            iter->cost=GetCost(iter->i,iter->j);
        }
        heap->Build();
    }
    double CSimpleObject::GetCost(int v1, int v2,Vec3f& nv)
    {
        QMat QQ=qmats[v1]+qmats[v2];
        double mincost;
        Vec3f& p1=vertexs[v1];
        Vec3f& p2=vertexs[v2];
        Vec3f p0=(p1+p2)/2.0f;
        if(!Solve(QQ,nv))
        {
            nv=(p1+p2)/2.0f;
        }
        mincost=min(min(COST(nv,QQ),COST(p0,QQ)),min(COST(p1,QQ),COST(p2,QQ)));
        if(mincost==COST(p0,QQ)) nv=p0;
        if(mincost==COST(p1,QQ)) nv=p1;
        if(mincost==COST(p2,QQ)) nv=p2;
        return mincost;
    }
    
    double CSimpleObject::GetCost(int v1, int v2)
    {
        Vec3f unuse;
        return GetCost(v1,v2,unuse);
    }
    
    void CSimpleObject::RemoveVertice(VPair& v)
    {
        //删除面
        auto delfaciter=faces.end();
        for(int i=faces.size()-1;i>=0;--i)
        {
            for(int j=0;j<3;++j)
            {
                if(faces[i].index[j]==v.j)
                {
                    if(faces[i].index[0]==v.i||faces[i].index[1]==v.i||faces[i].index[2]==v.i)
                    {
                        delfaciter--;
                        swap(faces[i].index[0],delfaciter->index[0]);
                        swap(faces[i].index[1],delfaciter->index[1]);
                        swap(faces[i].index[2],delfaciter->index[2]);
                        swap(faces[i].para[0],delfaciter->para[0]);
                        swap(faces[i].para[1],delfaciter->para[1]);
                        swap(faces[i].para[2],delfaciter->para[2]);
                        swap(faces[i].para[3],delfaciter->para[3]);
                    }
                    else
                    {
                        faces[i].index[j]=v.i;
                    }
                    break;
                }
            }
        }
        faces.erase(delfaciter,faces.end());
        //删除顶点
        //TODO: remove vertex
        //删除顶点对
        heap->RemoveVPair(v,this);
    }
    
    void CSimpleObject::Adjust()
    {
        m_nTriangles=faces.size();
        for(int i=0;i<m_nTriangles;i++)
        {
            m_pTriangleList[i][0]=faces[i].index[0];
            m_pTriangleList[i][1]=faces[i].index[1];
            m_pTriangleList[i][2]=faces[i].index[2];
        }
        m_nVertices=vertexs.size();
        for(int i=0;i<m_nVertices;i++)
        {
            m_pVertexList[i]=vertexs[i];
        }
        vertexs.clear();
        faces.clear();
        qmats.clear();
        if(heap)
            delete heap;
        heap=NULL;
    }
        
}