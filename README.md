# CrashHandle
UE4 崩溃处理上报（Linux,Android,IOS）

配合 https://github.com/qqwx1986/CrashService 使用

已支持功能：<br>
1) Error 日志上报
2) 自动生成CrashReportClient版本信息
3) Android crash 上报

未完成功能：<br>
1) ios crash上报

## 配置

DefaultCrashHandle.ini

```angular2html
[/Script/CrashHandle.CrashHandleSetting]
;服务器上报Url
ServerUrl="http://localhost:13333"
;开发版本上报Url
DevServerUrl="http://localhost:13333"
;开发版本（非Shipping）是否启用
bEnableDev=True
;是否在Android启用
bAndroidCrashReport=True
;是否在IOS启用
;bIOSCrashReport
;是否在Linux启用（暂时废弃，上报方式和windows平台一样）
;bLinuxCrashReport
;版本号，这个是和符号表对应的版本号
;如果不填，windows和linux取得是二进制执行文件的md5码作为版本号，android模式是latest，如果用jenkins打包，建议每次出版本可以用脚步修改该版本号再打包
Version=
```