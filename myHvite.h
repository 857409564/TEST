#ifndef __YX_HTK_USER_
#define __YX_HTK_USER_

/*********************************************
*   libmyHvite.so
*   ������  �������HTK3.4.1Դ�룬��������
*           HVite��HCopy���ֳ��򣬲�������
*           ��̬�ⷽ���ⲿ���á�
*********************************************/



/*********************************************
*   hcopymain��
*           ����HCopy���������ļ�·���̶�Ϊ:
*               ./trans/analysis.conf��
*           inputΪ�����ļ���
*           ouputΪ����ļ���
*********************************************/
int hcopymain(char * input, char * ouput);



/*********************************************
*   hvitemain��
*           ����HVite���������ļ��̶�Ϊ����·��:
*               ģ���ļ� -- ./reco/hmmsdef.mmf");
*               ����ļ� -- reco.mlf
*               �����ļ� -- ./reco/net.slf
*               �ֵ��ļ� -- ./reco/dict.txt
*               ģ���б� -- ./reco/hmmlist.txt
*           mfccnameΪ��ʶ�������ļ���
*           resultΪ���ؽ���ַ���
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