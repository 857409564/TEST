#ifndef __YX_HTK_USER_
#define __YX_HTK_USER_

/*********************************************
*   libmyHvite.so
*   声明：  本库基于HTK3.4.1源码，仅更改了
*           HVite及HCopy部分程序，并独立成
*           静态库方便外部调用。
*********************************************/



/*********************************************
*   hcopymain：
*           代表HCopy程序，配置文件路径固定为:
*               ./trans/analysis.conf。
*           input为输入文件名
*           ouput为输出文件名
*********************************************/
int hcopymain(char * input, char * ouput);



/*********************************************
*   hvitemain：
*           代表HVite程序，配置文件固定为以下路径:
*               模型文件 -- ./reco/hmmsdef.mmf");
*               结果文件 -- reco.mlf
*               网表文件 -- ./reco/net.slf
*               字典文件 -- ./reco/dict.txt
*               模型列表 -- ./reco/hmmlist.txt
*           mfccname为待识别特征文件名
*           result为返回结果字符串
*********************************************/
int hvitemain(char * mfccname, char * result);



/*
for example:

int main(int argc, char *argv[]){
    hcopymain("word5.sig", "word5.mfcc");
    char result[100]={0};
    hvitemain("word5.mfcc", result);
    printf("%s\n",result);
    return 0;
}
*/
#endif /*__YX_HTK_USER_*/