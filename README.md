# CrashHandle
UE4 崩溃处理上报（Linux,Android,IOS）

配合 https://github.com/qqwx1986/CrashService 使用

已支持功能：<br>
1) Error 日志上报
2) 自动生成CrashReportClient版本信息

未完成功能：<br>
1) android crash上报
2) android crash上报

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
;bAndroidCrashReport
;是否在IOS启用
;bIOSCrashReport
;是否在Linux启用
;bLinuxCrashReport
```