#include "SimpleObject.h"
//
using namespace SimpleOBJ;
int main(int argc,char** argv)
{
    if(argc<2)
    {
        printf("wrong parameters£°\n");
        printf("<inputfile> <option>\n");
        printf("available parameters\n");
        printf("-r <rate> : Set the coefficient of simplification\n");
        printf("-h        : Show help\n");
        return 0;
    }
    int fn=10;
    double rate=0.8f;
    for(int i=1;i<argc;++i)
    {
        if(argv[i][0]!='-')
        {
            if(i<fn)
                fn=i;
            continue;
        }
        if(argv[i][1]=='h'&&argv[i][2]=='\0')
        {
            printf("<inputfile> <option>\n");
            printf("available parameters\n");
            printf("-r <rate> : Set the coefficient of simplification\n");
            printf("-h        : Show help\n");
            return 0;
        }
        else if(argv[i][1]=='r'&&argv[i][2]=='\0')
        {
            rate=double(atof(argv[++i]));
        }
    }
    CSimpleObject obj;
    //∂¡»°
    set_time_base();
    if(!obj.LoadFromObj(argv[fn]))
    {
        printf("can not open file %s",argv[fn]);
        return 0;
    }
    float t1=float(get_time_in_seconds());
    //ºÚªØ
    set_time_base();
    obj.Simplification(rate);
    float t2=float(get_time_in_seconds());

    char ofname[100];
    sprintf(ofname,"%.4f_%s",rate,argv[fn]);
    obj.SaveToObj(ofname);

    printf("\nload from %s took %f seconds\n",argv[fn],t1);
    printf("Simplification took %f seconds\n",t2);
    return 0;
}