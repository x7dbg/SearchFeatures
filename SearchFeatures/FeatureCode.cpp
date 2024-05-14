#include "stdafx.h"
#include "FeatureCode.h"
using namespace std;

FeatureCode::FeatureCode(void)
{}

FeatureCode::~FeatureCode(void)
{}


DWORD FeatureCode::FindMatchingCode(HANDLE hProcess, string markCode, DWORD memBeginAddr, DWORD memEndAddr, DWORD retAddr[], int deviation, bool isCall, bool isAll)
{
    //----------------------处理特征码----------------------//
    //去除所有空格
    if (!markCode.empty())
    {
        int index = 0;
        while ((index = markCode.find(' ', index)) >= 0)
        {
            markCode.erase(index, 1);
        }
        index = 0;
        while (true)
        {
            //删掉头部通配符
            index = markCode.find("??", index);
            if (index == 0) {
                markCode.erase(index, 2);
            }
            else {
                break;
            }
        }
    }


    //特征码长度不能为单数
    if (markCode.length() % 2 != 0) return 0;

    //特征码长度
    int len = markCode.length() / 2;

    //Sunday算法模板数组的长度
    int nSundayLen = len;

    //将特征码转换成byte型
    BYTE *pMarkCode = new BYTE[len];
    BYTE *pWildcard = new BYTE[len];
    for (int i = 0; i < len; i++) {
        string tempStr = markCode.substr(i * 2, 2);
        if (tempStr == "??") {
            pWildcard[i] = 0xFF;
            if (nSundayLen == len) nSundayLen = i;	//记录第一个通配符的索引，该索引越靠后，效率越高
        }
        else {
            pWildcard[i] = 0x00;
        }
        pMarkCode[i] = strtoul(tempStr.c_str(), 0, 16);
    }
    //--------------------------end-------------------------//

    //Sunday算法模板数组赋值，+1防止特征码出现FF时越界
    int aSunday[0xFF + 1] = { 0 };
    for (int i = 0; i < nSundayLen; i++) {
        aSunday[pMarkCode[i]] = i + 1;
    }

    //起始地址
    const DWORD dwBeginAddr = memBeginAddr;
    //结束地址
    const DWORD dwEndAddr = memEndAddr;
    //当前读取的内存块地址
    DWORD dwCurAddr = dwBeginAddr;
    //存放内存数据的缓冲区
    BYTE *pMemBuffer = NULL;
    //计算参数retAddr[]数组的长度，该参数传入前一定要清0
    int nArrayLength = 0;
    for (int i = 0;; i++) {
        if (*(retAddr + i) != 0) {
            nArrayLength = i;
            break;
        }
    }
    //偏移量
    int nOffset;
    //数组下标：内存、特征码、返回地址
    int i = 0, j = 0, nCount = 0;

    //内存信息
    MEMORY_BASIC_INFORMATION mbi;


    //扫描内存
    while (dwCurAddr < dwEndAddr)
    {
        //查询地址空间中内存地址的信息
        memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));
        if (::VirtualQueryEx(hProcess, (LPCVOID)dwCurAddr, &mbi, sizeof(mbi)) == 0) {
            goto end;
        }

        //添加一句代码，修改当前地址到这块内存页的起始地址，编辑于2022.4.4，没有这句会有概率搜不到想要的内存
        dwCurAddr = (DWORD_PTR)mbi.BaseAddress;
        //-----------------------------------------------------//

        //过滤内存空间, 根据内存的状态和保护属性进行过滤
        //一般扫描（读写及执行）即可，速度极快，扫不到的话在尝试添加（读写）这一属性
        if (MEM_COMMIT == mbi.State &&			//已分配的物理内存
            MEM_PRIVATE == mbi.Type ||		//私有内存，不被其他进程共享
            MEM_IMAGE == mbi.Type &&
            PAGE_READONLY == mbi.Protect ||	//只读
            PAGE_EXECUTE_READ == mbi.Protect ||	//读及执行
            PAGE_READWRITE == mbi.Protect ||	//读写
            PAGE_EXECUTE_READWRITE == mbi.Protect)	//读写及执行
        {
            //申请动态内存
            if (pMemBuffer) {
                delete[] pMemBuffer;
                pMemBuffer = NULL;
            }
            pMemBuffer = new BYTE[mbi.RegionSize];
            //读取进程内存
            ReadProcessMemory(hProcess, (LPCVOID)dwCurAddr, pMemBuffer, mbi.RegionSize, 0);
            i = 0;
            j = 0;
            while (j < len)
            {
            nextAddr:
                if (pMemBuffer[i] == pMarkCode[j] || pWildcard[j] == 0xFF)
                {
                    i++;
                    j++;
                }
                else
                {
                    nOffset = i - j + nSundayLen;
                    //判断偏移量是否大于缓冲区
                    if (nOffset > mbi.RegionSize - len) break;
                    //判断 aSunday模板数组 里有没有 内存偏移后的值，有则回溯，否则+1
                    if (aSunday[pMemBuffer[nOffset]])
                    {
                        i = nOffset - aSunday[pMemBuffer[nOffset]] + 1;
                        j = 0;
                    }
                    else
                    {
                        i = nOffset + 1;
                        j = 0;
                    }
                }
            }

            if (j == len)
            {
                //计算找到的目标地址：
                //特征码地址 = 当前内存块基址 + i偏移 - 特征码长度
                //目标地址 = 特征码地址 + 偏移距离
                //CALL（E8）跳转的地址 = E8指令后面的4个字节地址 + 下一条指令地址(也就是目标地址 + 5)
                retAddr[nCount] = dwCurAddr + i - len + deviation;
                if (isCall) {
                    DWORD temp;
                    memcpy(&temp, &pMemBuffer[i - len + deviation + 1], 4);
                    retAddr[nCount] += 5;
                    retAddr[nCount] += temp;
                }

                if (++nCount >= nArrayLength)
                {
                    //传入的数组下标越界就结束搜索
                    goto end;
                }

                if (isAll) {
                    i = i - len + 1;
                    j = 0;
                    goto nextAddr;
                }
                else {
                    goto end;
                }
            }
            dwCurAddr += mbi.RegionSize; //取下一块内存地址
        }
        else
        {
            dwCurAddr += mbi.RegionSize;
        }
    }


end:
    //释放内存
    if (pMemBuffer) {
        delete[] pMemBuffer;
        pMemBuffer = NULL;
    }
    delete[] pMarkCode;
    pMarkCode = NULL;
    delete[] pWildcard;
    pWildcard = NULL;
    return nCount;
}