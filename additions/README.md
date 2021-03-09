# tesUt 追加機能

## tesUt lib
tesUtの追加機能で使用する関数群です。  
ユーザー側の関数と切り替えることができます。
__但し、下記関数はユーザー側で登録をお願いします。__  
* TESU_putchar関数 - コンソール（文字列）出力に使用します。環境によって出力環境が異なるため、この関数にて表示環境を作成する必要があります。

## tesUt Console
tesUtのハンドラ機能をしようして結果をコンソール出力します。  
結果出力をするためには、最低、tesUt libのTESU_putcharの登録が必要となります。   

出力例 :
```
 ASSERT [ PASSED ] additions/usage-console.c:47  TESU_ASSERT_EQUAL(sum,6447)
 TEST   [ PASSED ] TEST No.    1 : SUM_ALL_TEST00

 ASSERT [ PASSED ] additions/usage-console.c:56  sum > 100
 TEST   [ PASSED ] TEST No.    2 : SUM_ALL_TEST01

 ASSERT [ PASSED ] additions/usage-console.c:65  TESU_ASSERT_NOT_EQUAL(sum,200)
 TEST   [ PASSED ] TEST No.    3 : SUM_ALL_TEST02

additions/usage-console.c:150
    1 Test Suite: Suite0
--Run Summary: Type      Total     Ran    Passed    Failed
               tests         3       3         3         0
               asserts       3       3         3         0
===============================================================
```
使用方法は、usage-console.cを参照してください。
