@echo off 

rem �������س���Ľ������ͳ���·�����ɸ�����Ҫ�����޸�

set AppName=run.exe

set AppArgs= --run-with-camera --wait-uart --show-armor-box

set AppPath=C:\Users\sjturm\Desktop\AutoAim\build\Release\

title ���̼��

cls

echo.

echo ���̼�ؿ�ʼ����

echo.

rem ����ѭ����

:startjc

   rem �ӽ����б��в���ָ������

   rem  �������Ҳ��д�� qprocess %AppName% >nul �����鷢���󲹳䣩

   qprocess|findstr /i %AppName% >nul

   rem ����errorlevel��ֵ����0��ʾ���ҵ����̣�����û�в��ҵ�����

   if %errorlevel%==0 (

         echo ^>%date:~0,10% %time:~0,8% �����������С���

    )else (

           echo ^>%date:~0,10% %time:~0,8% û�з��ֳ������

           echo ^>%date:~0,10% %time:~0,8% ����������������

           start %AppPath%%AppName%%AppArgs% 2>nul && echo ^>%date:~0,10% %time:~0,8% ��������ɹ�

   )

   rem ��ping������ʵ����ʱ����

   ping -n 2 -w 1000 1.1.1.1>nul
   
   goto startjc

echo on