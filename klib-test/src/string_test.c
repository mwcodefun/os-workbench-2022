#include <klib.h>

char* itostr(int value,int radix,char *target);
void print(const char *value);

void test_strcat(){
    char value[10] = "1234";
    char *pdst = value;
    char *src = "456";
    char *res = "1234456";
    strcat(pdst,src);
    assert(strcmp(res,value) == 0);
}

void test_itostr(){
    // int a = 1000;
    char result[10];
    // char *expect = "1000";
    // itostr(a,10,result);
    // print(result);
    // assert(strcmp(result,expect) == 0);
    itostr(-100,10,result);
    print(result);
}
void testStrcmp() {
    assert(strcmp("\0","\0") == 0);
    assert(strcmp("a","\0") > 0);
    assert(strcmp("\0","a") < 0);
}
void testStrLen(){
    int i = rand();
    char str[i];
    for(int j = 0;j < i;j++) {
        str[j] = 'a';
    }
    assert(strlen(str) == i);
    char *src = "123";
    assert(strlen(src) == 3);
}

void testStrCpy() {
    char *src = "123";
    char dst[4];

    char *ans = strcpy(dst,src);
    assert(strlen(src) == strlen(ans));
    while(*src){
        assert(*src == *ans);
        src++;
        ans++;
    }
    assert(*ans == '\0');
}

void testStrncmp(){
    char s1[] = "1112222";
    char s2[] = "1113333";
    assert(strncmp(s1,s2,3) == 0);
    assert(strncmp(s1,s2,4) < 0 );
}

void testMemSet(){
    int r = rand();
    char mem[r];
    memset(mem,'a',r);
    for(int i = 0;i<r;i++){
        assert(mem[i] == 'a');
    }
}
void testPrintf(){
    printf("%s-%d\n","123",10);
}


int main() {
    // testStrLen();
    // testStrCpy();
    // test_strcat();
    // testStrcmp();
    // testStrncmp();
    // testMemSet();

    test_itostr();
    // testPrintf();
    return 0;
}